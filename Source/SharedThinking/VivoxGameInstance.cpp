#include "VivoxGameInstance.h"

void UVivoxGameInstance::Init()
{
	Super::Init();

	// 1. Load and initialize the Vivox Voice Client
	FVivoxCoreModule* VivoxModule = &FModuleManager::Get().LoadModuleChecked<FVivoxCoreModule>(TEXT("VivoxCore"));
	if (VivoxModule)
	{
		VivoxVoiceClient = &VivoxModule->VoiceClient();
		VivoxCoreError Status = VivoxVoiceClient->Initialize();

		if (Status != VxErrorSuccess)
		{
			UE_LOG(LogTemp, Error, TEXT("Vivox Initialize failed: %s"), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Vivox Initialized successfully!"));
		}
	}
}

void UVivoxGameInstance::Shutdown()
{
	// Cleanly uninitialize Vivox when the game closes
	if (VivoxVoiceClient)
	{
		VivoxVoiceClient->Uninitialize();
	}

	Super::Shutdown();
}

void UVivoxGameInstance::LoginToVivox(const FString& PlayerName)
{
	if (!VivoxVoiceClient) return;

	// 2. Create the AccountId using your Issuer, unique PlayerName, and Domain
	LoggedInAccountId = AccountId(VivoxIssuer, PlayerName, VivoxDomain);
	VivoxLoginSession = &VivoxVoiceClient->GetLoginSession(LoggedInAccountId);

	// 3. Generate a login token (valid for 90 seconds)
	FTimespan TokenExpiration = FTimespan::FromSeconds(90);
	FString LoginToken = VivoxLoginSession->GetLoginToken(VivoxSecretKey, TokenExpiration);

	// 4. Bind the login callback and begin logging in
	ILoginSession::FOnBeginLoginCompletedDelegate OnLoginComplete;
	OnLoginComplete.BindLambda([this](VivoxCoreError Error)
	{
		if (Error == VxErrorSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("Vivox Login Succeeded!"));
			
			// Automatically join a test channel once logged in (set second param to false for multiplayer)
			JoinVoiceChannel(TEXT("MainLobby"), true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Vivox Login Failed: %s"), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Error)));
		}
	});

	VivoxLoginSession->BeginLogin(VivoxServer, LoginToken, OnLoginComplete);
}

void UVivoxGameInstance::JoinVoiceChannel(const FString& ChannelName, bool bIsEchoChannel)
{
	if (!VivoxLoginSession || VivoxLoginSession->State() != LoginState::LoggedIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot join channel: Not logged into Vivox."));
		return;
	}

	// 5. Choose ChannelType::Echo for solo mic testing, or ChannelType::NonPositional for team/lobby chat
	ChannelType Type = bIsEchoChannel ? ChannelType::Echo : ChannelType::NonPositional;
	ChannelId TargetChannel = ChannelId(VivoxIssuer, ChannelName, VivoxDomain, Type);

	IChannelSession& ChannelSession = VivoxLoginSession->GetChannelSession(TargetChannel);

	// 6. Generate a channel join token
	FTimespan TokenExpiration = FTimespan::FromSeconds(90);
	FString JoinToken = ChannelSession.GetConnectToken(VivoxSecretKey, TokenExpiration);

	// 7. Bind the connect callback and join the channel
	IChannelSession::FOnBeginConnectCompletedDelegate OnConnectComplete;
	OnConnectComplete.BindLambda([ChannelName](VivoxCoreError Error)
	{
		if (Error == VxErrorSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully joined voice channel: %s"), *ChannelName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to join voice channel: %s"), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Error)));
		}
	});

	// Parameters: bConnectAudio (true), bConnectText (false), bSwitchTransmission (true)
	ChannelSession.BeginConnect(true, false, true, JoinToken, OnConnectComplete);
}
