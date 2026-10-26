// Fill out your copyright notice in the Description page of Project Settings.


#include "GameTimer.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

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
		Elapsed = 0.0f;
	}
}

void AGameTimer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGameTimer, Elapsed);
}

void AGameTimer::Start()
{
	if (!HasAuthority()) return;

	if (GetWorld()) 
	{
		GetWorldTimerManager().ClearTimer(Handle);
	}

	Elapsed = 0.0f;

	OnTick.Broadcast(Elapsed);

	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(
			Handle,
			this,
			&AGameTimer::Increment,
			Interval,
			true
		);
	}
}

void AGameTimer::Increment()
{
	if (!HasAuthority()) return;

	Elapsed += Interval;
	OnTick.Broadcast(Elapsed);
}

void AGameTimer::OnRep_Elapsed()
{
	OnTick.Broadcast(Elapsed);
}

void AGameTimer::Stop()
{
	if (!HasAuthority()) return;

	if (GetWorld() && Handle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(Handle);

		OnStopped.Broadcast(Elapsed);
	}
}