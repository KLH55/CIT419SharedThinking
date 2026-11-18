 /*
 * #####################################################################################
 *  Unity Vivox Plugin for Unreal Engine Copyright © 2019 Unity Technologies
 * #####################################################################################
 */

#pragma once
#include "CoreMinimal.h"
#include "Utils/ModerationPimplPtr.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utils/ModerationUtils.h"
#include "Models/VivoxModerationReport.h"
#include "Models/VivoxModerationReportResponse.h"
#include "VivoxCore.h"
#include "IClient.h"

#include "VivoxModerationSubsystem.generated.h"

/* Forward declarations */
namespace VivoxModeration
{
	class FVivoxModerationApi;
}

/**
 * @brief Subsystem for handling connections to the Unity Moderation system.
 */
UCLASS()
class VIVOXMODERATION_API UVivoxModerationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	/**
	 * @brief Default constructor for the Moderation subsystem.
	 */
	UVivoxModerationSubsystem();

	/**
	 * @brief Default destructor for the Moderation subsystem.
	 */
	virtual ~UVivoxModerationSubsystem();

	/**
	 * @brief        Override constructor for forward declaration.
	 * @param Helper Helper class to invoke specialized hot-reload constructor.
	 */
	UVivoxModerationSubsystem(FVTableHelper& Helper);

	/* Subsystem functions, overrides from USubsystem. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;



	/**
	 * @brief                 Signs in the current user anonymously. No credentials are required and the session is confined to the current device.
	 * @param Report          A constructed Report object with the Reported User's Unity Authentication PlayerId and the Report Reason
	 * @param UASToken		  The Token provided with a login to the Unity Authentication Service
	 * @param ResponseHandler (OPTIONAL) Handler that executes when the API has returned a response.
	 */
	void ReportPost(FVivoxModerationReport Report,FString UASToken, VivoxModeration::THandler<FVivoxModerationReportResponse> ResponseHandler = VivoxModeration::THandler<FVivoxModerationReportResponse>());

	/**
	 * @brief                   Gets a Report object constructed
	 * @param ReportedPlayer    The Vivox Account Id that is being reported
	 * @param ReportReason      The reason that the player is being reported
	 */
	FVivoxModerationReport GetReport(AccountId ReportedVivoxAccountId, EVivoxModerationReportReason ReportReason);

	/**
	 * @brief                   Gets a Report object constructed
	 * @param ReportedPlayer    The Unity Authentication Player Id that is being reported
	 * @param ReportReason      The reason that the player is being reported
	 */
	FVivoxModerationReport GetReport(FString ReportedUASPlayerId, EVivoxModerationReportReason ReportReason);

	/**
	 * @brief					Begins the Vivox Caching process, constructing a TMap of every VivoxChannel joined, and every Participant added to those channels, for addition with Moderation Requests
	 * @param LocalVivoxAccount (OPTIONAL) The AccountId of the LoginSession to begin caching for, if there are multiple LoginSessions logged in
	 */
	void BeginVivoxCache(AccountId LocalVivoxAccount = AccountId());

	/**
	 * @brief					Ends the Vivox Caching process, and clears the current cache
	 */
	void EndVivoxCache();

	/* @brief Sets the cache timeout for the VivoxCache to determine how far back the cache sends recent players */
	FTimespan CacheTimeout = FTimespan::FromMinutes(45);

private:

	/*!< Pointer to API implementation. */
	VivoxModeration::TPimplPtr<VivoxModeration::FVivoxModerationApi> Api;

	bool VivoxCaching = false;
	FVivoxCoreModule *vModule = nullptr;
	IClient *VivoxVoiceClient = nullptr;
	ILoginSession* LoginSession = nullptr;
	TMap<ChannelId, TMap<AccountId, FDateTime>> VivoxCache;

	FString reportReasonToString(EVivoxModerationReportReason reason);

	void SetLoginSession();
	void HandleChannelJoined(const IChannelSession &channelSession);
	void HandleChannelLeft(const IChannelSession &channelSession);
	void HandleParticipantEditted(const IParticipant &participant);
	void HandleLoginStateChanged(LoginState state);
	void CleanVivoxCache();
};