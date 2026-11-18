/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once

#include "ILoginSession.h"
#include "VivoxCoreCommonImpl.h"
#include "TextToSpeech.h"
#include "VxcEvents.h"

/**
 *
 */
class LoginSession : public ILoginSession, public TSharedFromThis<LoginSession>
{
    AccountId _loginSessionId;
    FString _groupHandle;
    TMap<ChannelId, TSharedPtr<IChannelSession> > _channelSessions;
    TMap<AccountId, TSharedPtr<IPresenceSubscription> > _presenceSubscriptions;
    TSet<AccountId> _blockedSubscriptions;
    TSet<AccountId> _allowedSubscriptions;
    TSet<AccountId> _crossMutedCommunications;
    LoginState _state;
    Presence _currentPresence;
    ClientImpl &_client;
    FDelegateHandle _eventSdkEventRaised;
    TransmissionMode _transmissionMode;
    ParticipantSpeakingUpdateRate _participantUpdateRate;
    ChannelId _transmittingChannel;
    bool _presenceEnabled;
    bool _isAudioInjecting;
    TextToSpeech _ttsSubSystem;

    void SetState(LoginState state);
    void HandleEvent(const vx_evt_base_t &evt);

    void InitEventHandler();
    void CleanupEventHandler();
    void CleanupLoginSessionState();
public:
	LoginSession(ClientImpl &client, const AccountId &loginSessionId);
	~LoginSession();
    const TMap<ChannelId, TSharedPtr<IChannelSession> >& ChannelSessions() const override;
    const TMap<AccountId, TSharedPtr<IPresenceSubscription> >& PresenceSubscriptions() const override;
    const TSet<AccountId>& BlockedSubscriptions() const override;
    const TSet<AccountId>& AllowedSubscriptions() const override;
    const TSet<AccountId>& CrossMutedCommunications() const override;
    LoginState State() const override;
    const Presence& CurrentPresence() override;
    VivoxCoreError SetCurrentPresence(const Presence& value) override;
    const AccountId& LoginSessionId() const override;
    VivoxCoreError BeginLogin(const FString& server, const FString& accessToken, SubscriptionMode subscriptionMode, const TSet<AccountId>& presenceSubscriptions, const TSet<AccountId>& blockedPresenceSubscriptions, const TSet<AccountId>& allowedPresenceSubscriptions, FOnBeginLoginCompletedDelegate) override;
    VivoxCoreError BeginLogin(
        const FString &server,
        const FString & accessToken,
        FOnBeginLoginCompletedDelegate theDelegate = FOnBeginLoginCompletedDelegate()) override;
    IChannelSession& GetChannelSession(const ChannelId &channelId) override;
    void DeleteChannelSession(const ChannelId &channelId) override;
    VivoxCoreError BeginSetCrossMutedCommunications(const AccountId &accountId, const bool &muted, FOnBeginSetCrossMutedCommunicationsCompletedDelegate callback) override;
    VivoxCoreError BeginSetCrossMutedCommunications(const TSet<AccountId> &accountId, const bool &muted, FOnBeginSetCrossMutedCommunicationsCompletedDelegate callback) override;
    VivoxCoreError BeginClearCrossMutedCommunications(FOnBeginClearCrossMutedCommunicationsCompletedDelegate callback) override;
    VivoxCoreError BeginAddBlockedSubscription(const AccountId& accountId, FOnBeginAddBlockedSubscriptionCompletedDelegate callback) override;
    VivoxCoreError BeginRemoveBlockedSubscription(const AccountId& accountId, FOnBeginRemoveBlockedSubscriptionCompletedDelegate callback) override;
    VivoxCoreError BeginAddAllowedSubscription(const AccountId& accountId, FOnBeginAddAllowedSubscriptionCompletedDelegate callback) override;
    VivoxCoreError BeginRemoveAllowedSubscription(const AccountId& accountId, FOnBeginRemoveAllowedSubscriptionCompletedDelegate callback) override;
    VivoxCoreError BeginAddPresenceSubscription(const AccountId& accountId, FOnBeginAddPresenceSubscriptionCompletedDelegate callback) override;
    VivoxCoreError BeginRemovePresenceSubscription(const AccountId& accountId, FOnBeginRemovePresenceSubscriptionCompletedDelegate callback) override;
    VivoxCoreError BeginSendSubscriptionReply(const AccountId& accountId, const SubscriptionReply& replyType, FOnBeginSendSubscriptionReplyCompletedDelegate callback) override;
    VivoxCoreError BeginSendDirectedMessage(const AccountId& accountId, const FString& language, const FString& message, const FString& applicationStanzaNamespace, const FString& applicationStanzaBody, FOnBeginSendDirectedMessageCompletedDelegate callback) override;
    VivoxCoreError BeginSendDirectedMessage(const AccountId& accountId, const FString& message, FOnBeginSendDirectedMessageCompletedDelegate callback) override;
    VivoxCoreError BeginStartAudioInjection(const FString &filePath, FOnBeginStartAudioInjectionCompletedDelegate theDelegate) override;
    VivoxCoreError BeginSetSafeVoiceConsentStatus(const bool& consentToSet, const FString& environmentId, const FString& projectId, const FString& UASToken, FOnBeginSetSafeVoiceConsentCompletedDelegate theDelegate) override;
    VivoxCoreError BeginGetSafeVoiceConsentStatus(const FString& environmentId, const FString& projectId, const FString& UASToken, FOnBeginGetSafeVoiceConsentCompletedDelegate theDelegate) override;
    VivoxCoreError StopAudioInjection() override;
    bool IsAudioInjecting() override;
    VivoxCoreError SetTransmissionMode(TransmissionMode mode, ChannelId singleChannel) override;
    TransmissionMode GetTransmissionMode() const override;
    TArray<ChannelId> GetTransmittingChannels() const override;
    VivoxCoreError SetParticipantSpeakingUpdateRate(ParticipantSpeakingUpdateRate rate) override;
    ParticipantSpeakingUpdateRate GetParticipantSpeakingUpdateRate() const override;
    ITextToSpeech &TTS() override;
    void Logout() override;
    FString GetLoginToken(const FString& tokenSigningKey, FTimespan tokenExpirationDuration) const override;

    // Internal
    void ClearTransmittingChannel(const ChannelId &channelId);
    VivoxCoreError SetTransmissionInCore();
    int GetParticipantUpdateRateForCore() const;
    void HandleChannelConnectionStateChanged(const IChannelConnectionState& connectionState);
};