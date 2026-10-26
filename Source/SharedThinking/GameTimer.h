#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "GameTimer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTick, float, Elapsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStopped, float, Final);

UCLASS()
class SHAREDTHINKING_API AGameTimer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameTimer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FTimerHandle Handle;

	void Increment();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Interval = 0.1f;

	UPROPERTY(ReplicatedUsing = OnRep_Elapsed, BlueprintReadOnly)
	float Elapsed;

	UFUNCTION()
	void OnRep_Elapsed();

public:	
	UFUNCTION(BlueprintCallable)
	void Start();

	UFUNCTION(BlueprintCallable)
	void Stop();

	UPROPERTY(BlueprintAssignable)
	FTick OnTick;

	UPROPERTY(BlueprintAssignable)
	FStopped OnStopped;
};