// Fill out your copyright notice in the Description page of Project Settings.


#include "GameTimer.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

// Sets default values
AGameTimer::AGameTimer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

}

// Called when the game starts or when spawned
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

	request->SetURL(TEXT("https://webhook.site/6e67f156-0c3f-4aad-b7a8-ba589796dfbe"));
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
	request->ProcessRequest();
}