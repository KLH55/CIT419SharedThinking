#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VivoxCore.h"
#include "VivoxGameInstance.generated.h"

UCLASS()
class UVivoxGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	// Call this from C++ or Blueprints when the player is ready to connect
	UFUNCTION(BlueprintCallable, Category = "Vivox")
	void LoginToVivox(const FString& PlayerName);

	// Call this to join a voice channel after logging in
	UFUNCTION(BlueprintCallable, Category = "Vivox")
	void JoinVoiceChannel(const FString& ChannelName, bool bIsEchoChannel = false);

private:
	// Replace these four values with your Unity Cloud Dashboard credentials
	const FString VivoxServer = TEXT("https://unity.vivox.com/appconfig/24755-ass4-93464");
	const FString VivoxDomain = TEXT("mtu1xp.vivox.com");
	const FString VivoxIssuer = TEXT("24755-ass4-93464");
	const FString VivoxSecretKey = TEXT("u8jFCd3ENlTJ6zcuXELFl8Uw1z9Ln6gq");

	IClient* VivoxVoiceClient = nullptr;
	AccountId LoggedInAccountId;
	ILoginSession* VivoxLoginSession = nullptr;
};
