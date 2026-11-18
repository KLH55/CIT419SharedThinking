// Fill out your copyright notice in the Description page of Project Settings.


#include "GameTimer.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GameFramework/Controller.h"

AGameTimer::AGameTimer()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AGameTimer::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		elapsed = 0.0f;
	}
}

void AGameTimer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGameTimer, elapsed);
	DOREPLIFETIME(AGameTimer, hostName);
	DOREPLIFETIME(AGameTimer, clientName);
	DOREPLIFETIME(AGameTimer, client2Name);
	DOREPLIFETIME(AGameTimer, client3Name);
}

void AGameTimer::RegisterPlayerName(AController* PlayerController, const FString& PlayerName)
{
	if (!HasAuthority() || !PlayerController) return;

	// If this controller is local to the Listen Server, it is the Host
	if (PlayerController->IsLocalController())
	{
		hostName = PlayerName;
		return;
	}

	// Otherwise, find or assign a client slot (0 = clientName, 1 = client2Name, 2 = client3Name)
	int32 SlotIndex = RegisteredClientControllers.Find(PlayerController);
	if (SlotIndex == INDEX_NONE)
	{
		if (RegisteredClientControllers.Num() < 3)
		{
			SlotIndex = RegisteredClientControllers.Add(PlayerController);
		}
		else
		{
			return;
		}
	}

	if (SlotIndex == 0)
	{
		clientName = PlayerName;
	}
	else if (SlotIndex == 1)
	{
		client2Name = PlayerName;
	}
	else if (SlotIndex == 2)
	{
		client3Name = PlayerName;
	}
}
void AGameTimer::Start()
{
	if (!HasAuthority()) return;

	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(handle);
	}

	elapsed = 0.0f;

	OnTick.Broadcast(elapsed);

	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(
			handle,
			this,
			&AGameTimer::Increment,
			interval,
			true
		);
	}
}

void AGameTimer::Increment()
{
	if (!HasAuthority()) return;

	elapsed += interval;
	OnTick.Broadcast(elapsed);
}

void AGameTimer::OnRep_Elapsed()
{
	OnTick.Broadcast(elapsed);
}

void AGameTimer::Stop()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Stop Called"));
	
	if (GetWorld() && handle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(handle);
		OnStopped.Broadcast(elapsed);
	}

	if (!HasAuthority()) return;

	FHttpModule* http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = http->CreateRequest();

	request->SetURL(TEXT("http://localhost/SharedThinking/save_session.php/"));
	request->SetVerb(TEXT("POST"));
	request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	int32 finalTime = FMath::RoundToInt(elapsed * 1000.0f);

	auto formatJson = [](const FString& username)->FString
	{
		return (username.IsEmpty() || username.Equals(TEXT("None"))) ? TEXT("null") : FString::Printf(TEXT("\"%s\""), *username);
	};

	FString clientVal = formatJson(clientName);
	FString client2Val = formatJson(client2Name);
	FString client3Val = formatJson(client3Name);

	FString jsonBody = FString::Printf(
		TEXT("{\"elapsed_time\": %d, \"host\": \"%s\", \"client\": %s, \"client_2\": %s, \"client_3\": %s}"),
		finalTime,
		*hostName,
		*clientVal,
		*client2Val,
		*client3Val
	);

	request->SetContentAsString(jsonBody);
	request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (bSuccess && Res.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("MAMP Response: %s"), *Res->GetContentAsString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to reach MAMP script. Check if server is running."));
		}
	});

	request->ProcessRequest();
}