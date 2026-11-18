 /*
 * #####################################################################################
 *  Unity Vivox Plugin for Unreal Engine Copyright © 2019 Unity Technologies
 * #####################################################################################
 */


#include "VivoxModeration/Public/VivoxModerationSubsystem.h"
#include "VivoxModerationApi.h"
#include "VivoxModerationModule.h"
#include "WrappedModerationApi.h"

// Necessary to avoid triggering C4150 error for TUniquePtr<T> because the classes/structs are forward declared.
// See documentation in TDefaultDelete<T>::operator() for an explanation.
UVivoxModerationSubsystem::UVivoxModerationSubsystem() = default;
UVivoxModerationSubsystem::~UVivoxModerationSubsystem() = default;
UVivoxModerationSubsystem::UVivoxModerationSubsystem(FVTableHelper& Helper) {}

void UVivoxModerationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogModeration, Verbose, TEXT("UVivoxModerationSubsystem::Initialize()"));

	Super::Initialize(Collection);

	/* Api wrapper instance pointer */
	Api = VivoxModeration::MakePimpl<VivoxModeration::FVivoxModerationApi>(MakeUnique<VivoxModeration::FWrappedModerationApi>());
}

void UVivoxModerationSubsystem::Deinitialize()
{
	UE_LOG(LogModeration, Verbose, TEXT("UVivoxModerationSubsystem::Deinitialize()"));

	Super::Deinitialize();
}

FVivoxModerationReport UVivoxModerationSubsystem::GetReport(FString ReportedUnityPlayerId, EVivoxModerationReportReason ReportReason)
{
	UE_LOG(LogTemp, Log, TEXT("UVivoxModerationSubsystem::GetReport()"));

	SetLoginSession();

	AccountId reportedAccountId = AccountId(LoginSession->LoginSessionId().Issuer(), ReportedUnityPlayerId, LoginSession->LoginSessionId().Domain(), TOptional<FString>(), TOptional<TArray<FString>>(), LoginSession->LoginSessionId().UnityEnvironmentId());

	return GetReport(reportedAccountId, ReportReason);
}

FVivoxModerationReport UVivoxModerationSubsystem::GetReport(AccountId ReportedVivoxAccountId, EVivoxModerationReportReason ReportReason)
{
	UE_LOG(LogModeration, Verbose, TEXT("UVivoxModerationSubsystem::GetReport()"));

	SetLoginSession();

	CleanVivoxCache();

	FVivoxModerationReport reportReturn;

	reportReturn.ReportedUnityPlayerID = ReportedVivoxAccountId.Name();
	if (VivoxCaching)
	{
		for (auto cacheChannel : VivoxCache)
		{
			if (!cacheChannel.Value.Contains(ReportedVivoxAccountId))
			{
				continue;
			}
			reportReturn.VivoxChannels.Add(cacheChannel.Key);
			TArray<AccountId> channelAccounts;
			cacheChannel.Value.GetKeys(channelAccounts);
			reportReturn.VivoxChannels[cacheChannel.Key] = channelAccounts;
		}
	}
	reportReturn.ReportReason = reportReasonToString(ReportReason);
	reportReturn.ReportedVivoxURI = ReportedVivoxAccountId.ToString();
	reportReturn.ReportingVivoxURI = LoginSession->LoginSessionId().ToString();

	return reportReturn;
}

void UVivoxModerationSubsystem::SetLoginSession()
{
	vModule = static_cast<FVivoxCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")));
	if (vModule == nullptr)
	{
		UE_LOG(LogModeration, Verbose, TEXT("The Vivox Module is not loaded, Moderation will not compile and post Vivox Channel Participants"));
		return;
	}
	VivoxVoiceClient = &vModule->VoiceClient();
	TMap<AccountId, TSharedPtr<ILoginSession>> loginSessions = VivoxVoiceClient->LoginSessions();
	if (loginSessions.Num() == 0)
	{
		UE_LOG(LogModeration, Verbose, TEXT("Vivox does not have any LoginSessions, get a LoginSession using IClient::GetLoginSession() prior to calling Moderation functionality"));
	}
	for (auto _loginSession : loginSessions)
	{
		LoginSession = _loginSession.Value.Get();
		if (LoginSession->State() == LoginState::LoggedIn)
		{
			break;
		}
	}
}

FString UVivoxModerationSubsystem::reportReasonToString(EVivoxModerationReportReason reportReason)
{
	switch (reportReason)
	{
	case EVivoxModerationReportReason::PlayerName:
		return "player name";
	case EVivoxModerationReportReason::AimSnapping:
		return "aim snapping";
	case EVivoxModerationReportReason::Boosting:
		return "boosting";
	case EVivoxModerationReportReason::Exploiting:
		return "exploiting";
	case EVivoxModerationReportReason::Hacking:
		return "hacking";
	case EVivoxModerationReportReason::Smurfing:
		return "smurfing";
	case EVivoxModerationReportReason::UnrealisticMovement:
		return "unrealistic movement";
	case EVivoxModerationReportReason::CollusionWithOpponent:
		return "collusion with opponent";
	case EVivoxModerationReportReason::LeftTheMatch:
		return "left the match";
	case EVivoxModerationReportReason::Inactive:
		return "inactive";
	case EVivoxModerationReportReason::Sabotage:
		return "sabotage";
	case EVivoxModerationReportReason::Spamming:
		return "spamming";
	case EVivoxModerationReportReason::HateSpeech:
		return "hate speech";
	case EVivoxModerationReportReason::PredatoryBehaviour:
		return "predatory behaviour";
	case EVivoxModerationReportReason::NoiseDisruption:
		return "noise disruption";
	case EVivoxModerationReportReason::Scamming:
		return "scamming";
	case EVivoxModerationReportReason::Ads:
		return "ads";
	case EVivoxModerationReportReason::Threat:
		return "threat";
	case EVivoxModerationReportReason::VerbalAbuse:
		return "verbal abuse";
	}
	return "";
}

void UVivoxModerationSubsystem::ReportPost(FVivoxModerationReport Report, FString UASToken, VivoxModeration::THandler<FVivoxModerationReportResponse> ResponseHandler)
{
	UE_LOG(LogModeration, Verbose, TEXT("UVivoxModerationSubsystem::ReportPost()"));

	Api->ReportPost(Report, UASToken, ResponseHandler);
}

void UVivoxModerationSubsystem::HandleChannelJoined(const IChannelSession& channelSession)
{
	if (LoginSession->ChannelSessions()[channelSession.Channel()].Get() == nullptr)
	{
		return;
	}
	if (!VivoxCache.Contains(channelSession.Channel()))
	{
		VivoxCache.Add(channelSession.Channel());
	}
	IChannelSession* session = LoginSession->ChannelSessions()[channelSession.Channel()].Get();
	session->EventAfterParticipantAdded.AddLambda([this](const IParticipant &participant) {
		HandleParticipantEditted(participant);
		});
	session->EventBeforeParticipantRemoved.AddLambda([this](const IParticipant &participant) {
		HandleParticipantEditted(participant);
		});
	//ChannelJoined can happen after some participant events have been received for the IChannelSession - do a loop through the participants to ensure proper caching
	for (auto participant : channelSession.Participants())
	{
		HandleParticipantEditted(*participant.Value);
	}
}

void UVivoxModerationSubsystem::HandleChannelLeft(const IChannelSession &channelSession)
{
	if (LoginSession->ChannelSessions()[channelSession.Channel()].Get() == nullptr)
	{
		return;
	}
	IChannelSession* session = LoginSession->ChannelSessions()[channelSession.Channel()].Get();
	session->EventAfterParticipantAdded.RemoveAll(this);
	session->EventBeforeParticipantRemoved.RemoveAll(this);
	for (auto participant : channelSession.Participants())
	{
		HandleParticipantEditted(*participant.Value);
	}
}

void UVivoxModerationSubsystem::HandleParticipantEditted(const IParticipant &participant)
{
	//When a participant is added or removed, check if that participant is Self (and ignore it)
	if (participant.IsSelf())
	{
		return;
	}
	//Add the channel and account of the participant if the channel is not in the cache, or just add the participant if the channel is present but the participant is not
	if (!VivoxCache.Contains(participant.ParentChannelSession().Channel()))
	{
		VivoxCache.Add(participant.ParentChannelSession().Channel());
		VivoxCache[participant.ParentChannelSession().Channel()].Add(participant.Account());
	}
	else if (!VivoxCache[participant.ParentChannelSession().Channel()].Contains(participant.Account()))
	{
		VivoxCache[participant.ParentChannelSession().Channel()].Add(participant.Account());
	}
	//Set the time of this participants interation to current time
	VivoxCache[participant.ParentChannelSession().Channel()][participant.Account()] = FDateTime::Now();
}

void UVivoxModerationSubsystem::HandleLoginStateChanged(LoginState state)
{
	if (state == LoginState::LoggedIn)
	{
		LoginSession->EventChannelJoined.AddLambda([this](const IChannelSession& channelSession) {
			HandleChannelJoined(channelSession);
			});
		LoginSession->EventChannelLeft.AddLambda([this](const IChannelSession& channelSession) {
			HandleChannelLeft(channelSession);
			});
	}
	if (state == LoginState::LoggedOut)
	{
		LoginSession->EventChannelJoined.RemoveAll(this);
		LoginSession->EventChannelLeft.RemoveAll(this);
	}
}

void UVivoxModerationSubsystem::BeginVivoxCache(AccountId accountId)
{
	if (VivoxCaching)
	{
		UE_LOG(LogModeration, Verbose, TEXT("The Vivox Cache has already been started - use EndVivoxCache to stop it before restarting"));
		return;
	}
	//Attempt to grab, then check that grabbing the Vivox Module was successful
	vModule = static_cast<FVivoxCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")));

	if (vModule == nullptr)
	{
		UE_LOG(LogModeration, Verbose, TEXT("The Vivox Module is not loaded, Moderation will not compile and post Vivox Channel Participants"));
		return;
	}

	//Grab the VoiceClient, then check if a current LoginSession belongs to the AccountId provided
	VivoxVoiceClient = &vModule->VoiceClient();
	if (!VivoxVoiceClient->LoginSessions().Contains(accountId) && !accountId.IsEmpty())
	{
		UE_LOG(LogModeration, Verbose, TEXT("Vivox does not have a LoginSession that matches the accountId provided - make sure to call IClient::GetLoginSession() with the accountId before beginning the Vivox Cache"));
	}
	else if(accountId.IsEmpty())
	{
		//If AccountId was not provided, check if any LoginSessions exist, and connect to either the one most recently connected, or the one currently logged in (if any)
		TMap<AccountId, TSharedPtr<ILoginSession>> loginSessions = VivoxVoiceClient->LoginSessions();
		if (loginSessions.Num() == 0)
		{
			UE_LOG(LogModeration, Verbose, TEXT("Vivox does not have any LoginSessions, get a LoginSession using IClient::GetLoginSession() prior to calling ModerationSubsystem::BeginVivoxCache"));
		}
		for (auto _loginSession : loginSessions)
		{
			LoginSession = _loginSession.Value.Get();
			if (LoginSession->State() == LoginState::LoggedIn)
			{
				break;
			}
		}
	}
	else
	{
		LoginSession = VivoxVoiceClient->LoginSessions()[accountId].Get();
	}
	VivoxCaching = true;
	LoginSession->EventStateChanged.AddLambda([this](LoginState state) {
		HandleLoginStateChanged(state);
		});
	if (LoginSession->State() == LoginState::LoggedIn)
	{
		LoginSession->EventChannelJoined.AddLambda([this](const IChannelSession& session) {
			HandleChannelJoined(session);
			});
		LoginSession->EventChannelLeft.AddLambda([this](const IChannelSession& session) {
			HandleChannelLeft(session);
			});
		for (auto _channelSession : LoginSession->ChannelSessions())
		{
			_channelSession.Value.Get()->EventAfterParticipantAdded.AddLambda([this](const IParticipant& participant) {
				HandleParticipantEditted(participant);
				});
			_channelSession.Value.Get()->EventBeforeParticipantRemoved.AddLambda([this](const IParticipant& participant) {
				HandleParticipantEditted(participant);
				});
		}
	}
}

void UVivoxModerationSubsystem::EndVivoxCache()
{
	for (auto channelSession : LoginSession->ChannelSessions())
	{
		channelSession.Value.Get()->EventAfterParticipantAdded.RemoveAll(this);
		channelSession.Value.Get()->EventBeforeParticipantRemoved.RemoveAll(this);
	}
	LoginSession->EventChannelJoined.RemoveAll(this);
	LoginSession->EventChannelLeft.RemoveAll(this);
	for (auto cacheEntry : VivoxCache)
	{
		cacheEntry.Value.Empty();
	}
	VivoxCache.Empty();
	VivoxCaching = false;
}

void UVivoxModerationSubsystem::CleanVivoxCache()
{
	TArray<ChannelId> staleChannels;
	TArray<AccountId> staleAccounts;
	for (auto cacheEntry : VivoxCache)
	{
		if (LoginSession->ChannelSessions().Contains(cacheEntry.Key))
		{
			for (auto cachedPlayer : cacheEntry.Value)
			{
				if (LoginSession->ChannelSessions()[cacheEntry.Key]->Participants().Contains(cachedPlayer.Key.Name()))
				{
					cachedPlayer.Value = FDateTime::Now();
				}
				if (cachedPlayer.Value + CacheTimeout < FDateTime::Now())
				{
					staleAccounts.Add(cachedPlayer.Key);
				}
			}
		}
		else
		{
			for (auto cachedPlayer : cacheEntry.Value)
			{
				if (cachedPlayer.Value + CacheTimeout < FDateTime::Now())
				{
					staleAccounts.Add(cachedPlayer.Key);
				}
			}
		}
		for (auto staleAccount : staleAccounts)
		{
			cacheEntry.Value.Remove(staleAccount);
		}
		if (cacheEntry.Value.Num() == 0)
		{
			staleChannels.Add(cacheEntry.Key);
		}
		staleAccounts.Empty();
	}
	for (auto staleChannel : staleChannels)
	{
		VivoxCache.Remove(staleChannel);
	}
	staleChannels.Empty();
}

bool UVivoxModerationSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}