#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "GameTimer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTick, float, elapsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStopped, float, Final);

UCLASS()
class SHAREDTHINKING_API AGameTimer : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameTimer();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FTimerHandle handle;

	void Increment();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float interval = 0.1f;

	UPROPERTY(ReplicatedUsing = OnRep_Elapsed, BlueprintReadOnly)
	float elapsed;

	UFUNCTION()
	void OnRep_Elapsed();

	// Tracks which controller is assigned to client, client_2, and client_3 so re-submitting updates the right slot
	UPROPERTY()
	TArray<AController*> RegisteredClientControllers;

public:	
	UFUNCTION(BlueprintCallable)
	void Start();

	UFUNCTION(BlueprintCallable)
	void Stop();

	/** Assigns a player's username to hostName, clientName, client2Name, or client3Name on the server */
	UFUNCTION(BlueprintCallable, Category = "Player Names")
	void RegisterPlayerName(AController* PlayerController, const FString& PlayerName);

	UPROPERTY(BlueprintAssignable)
	FTick OnTick;

	UPROPERTY(BlueprintAssignable)
	FStopped OnStopped;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FString hostName = TEXT("");

	UPROPERTY(Replicated, BlueprintReadWrite)
	FString clientName = TEXT("");

	UPROPERTY(Replicated, BlueprintReadWrite)
	FString client2Name = TEXT("");

	UPROPERTY(Replicated, BlueprintReadWrite)
	FString client3Name = TEXT("");
};
