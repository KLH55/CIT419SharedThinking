/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "LoginSession.h"
#include "VivoxCore.h"
#include "ChannelSession.h"
#include "VxcErrors.h"
#include "VivoxNativeSdk.h"
#include "ClientImpl.h"
#include "Presence.h"
#include "DirectedTextMessage.h"
#include "PresenceSubscription.h"
#include "VxcEvents.h"

#define EnsureLoggedIn() ensure(_state == LoginState::LoggedIn)
#define CUR_CLASS_FUNC (FString(__FUNCTION__))

LoginSession::LoginSession(ClientImpl &client, const AccountId &loginSessionId) : _client(client), _ttsSubSystem(_client)
{
    UE_LOG(VivoxCore, Log, TEXT("LoginSession created for %s"), *loginSessionId.ToString());
    ensure(loginSessionId.IsValid());
    _loginSessionId = loginSessionId;
    _groupHandle = TEXT("sg_") + _loginSessionId.ToString();
    _state = LoginState::LoggedOut;
    _transmissionMode = TransmissionMode::None;
    _participantUpdateRate = ParticipantSpeakingUpdateRate::StateChange;
    _isAudioInjecting = false;
    InitEventHandler();
}

LoginSession::~LoginSession()
{
    UE_LOG(VivoxCore, Log, TEXT("LoginSession destroyed for %s"), *_loginSessionId.ToString());
    CleanupEventHandler();
}

void LoginSession::CleanupEventHandler()
{
    if (_eventSdkEventRaised.IsValid()) {
        VivoxNativeSdk::Get().EventSdkEventRaised.Remove(_eventSdkEventRaised);
        _eventSdkEventRaised = FDelegateHandle();
    }
}

void LoginSession::CleanupLoginSessionState()
{
        _channelSessions.Empty();
        _presenceSubscriptions.Empty();
        _blockedSubscriptions.Empty();
        _allowedSubscriptions.Empty();
        _currentPresence = Presence();
}

void LoginSession::InitEventHandler()
{
    CleanupEventHandler();
    _eventSdkEventRaised = VivoxNativeSdk::Get().EventSdkEventRaised.AddLambda([this](const vx_evt_base_t &evt)
    {
        TSharedPtr<LoginSession> protect = SharedThis(this);
        HandleEvent(evt);
    });
}

void LoginSession::SetState(LoginState state)
{
    if(state != _state)
    {
        _state = state;
        EventStateChanged.Broadcast(state);
    }
}

void LoginSession::HandleEvent(const vx_evt_base_t& evt)
{
    if (evt.type == evt_account_login_state_change)
    {
        const vx_evt_account_login_state_change_t &tevt = reinterpret_cast<const vx_evt_account_login_state_change_t &>(evt);
        if (tevt.state == login_state_logged_out && _loginSessionId.ToString() == FString(tevt.account_handle))
        {
            SetState(LoginState::LoggedOut);
            CleanupLoginSessionState();
            CleanupEventHandler();
        }
    } else if (evt.type == evt_user_to_user_message) {
        const vx_evt_user_to_user_message &tevt = reinterpret_cast<const vx_evt_user_to_user_message &>(evt);
        if (_state == LoginState::LoggedIn && _loginSessionId.ToString() == FString(tevt.account_handle)) {
            DirectedTextMessage directMessage(*this, tevt);
            EventDirectedTextMessageReceived.Broadcast(directMessage);
        }
    } else if (evt.type == evt_buddy_presence) {
        const vx_evt_buddy_presence_t &tevt = reinterpret_cast<const vx_evt_buddy_presence_t &>(evt);
        if (_state == LoginState::LoggedIn && _loginSessionId.ToString() == FString(tevt.account_handle)) {
            AccountId buddyAccount = AccountId::CreateFromUri(tevt.buddy_uri, FString(UTF8_TO_TCHAR(tevt.displayname)), _loginSessionId.UnityEnvironmentId().IsEmpty() ? TOptional<FString>() : _loginSessionId.UnityEnvironmentId());
            if (_presenceSubscriptions.Contains(buddyAccount)) {
                PresenceSubscription *sub = static_cast<PresenceSubscription*>(_presenceSubscriptions[buddyAccount].Get());
                sub->HandleEvent(tevt);
            }
        }
    } else if (evt.type == evt_subscription) {
        const vx_evt_subscription_t &tevt = reinterpret_cast<const vx_evt_subscription_t &>(evt);
        if (_state == LoginState::LoggedIn && _loginSessionId.ToString() == FString(tevt.account_handle)) {
            AccountId buddyAccount = AccountId::CreateFromUri(tevt.buddy_uri, TOptional<FString>(UTF8_TO_TCHAR(tevt.displayname)), _loginSessionId.UnityEnvironmentId().IsEmpty() ? TOptional<FString>() : _loginSessionId.UnityEnvironmentId());
            EventSubscriptionRequestReceived.Broadcast(buddyAccount);
        }
    } else if (evt.type == evt_media_completion) {
        const vx_evt_media_completion_t &tevt = reinterpret_cast<const vx_evt_media_completion_t &>(evt);
        if (_state == LoginState::LoggedIn && _groupHandle == FString(tevt.sessiongroup_handle)) {
            switch (tevt.completion_type)
            {
            case vx_media_completion_type::sessiongroup_audio_injection:
                _isAudioInjecting = false;
                EventAudioInjectionCompleted.Broadcast();
            default:
                return;
            }
        }
    } else if (evt.type == evt_account_send_message_failed) {
        const vx_evt_account_send_message_failed_t &tevt = reinterpret_cast<const vx_evt_account_send_message_failed_t &>(evt);
        if (_state == LoginState::LoggedIn && _loginSessionId.ToString() == FString(tevt.account_handle)) {
            EventSendDirectedTextMessageFailed.Broadcast(tevt.status_code, tevt.request_id);
        }
    } else {
        // Handle Text-To-Speech cases
        _ttsSubSystem.HandleEvent(evt);
    }
}

const TMap<ChannelId, TSharedPtr<IChannelSession>>& LoginSession::ChannelSessions() const
{
    return _channelSessions;
}

const TMap<AccountId, TSharedPtr<IPresenceSubscription>>& LoginSession::PresenceSubscriptions() const
{
    return _presenceSubscriptions;
}

const TSet<AccountId>& LoginSession::BlockedSubscriptions() const
{
    return _blockedSubscriptions;
}

const TSet<AccountId>& LoginSession::AllowedSubscriptions() const
{
    return _allowedSubscriptions;
}

const TSet<AccountId>& LoginSession::CrossMutedCommunications() const
{
    return _crossMutedCommunications;
}

LoginState LoginSession::State() const
{
    return _state;
}

const Presence& LoginSession::CurrentPresence()
{
    return _currentPresence;
}

VivoxCoreError LoginSession::SetCurrentPresence(const Presence& value)
{
    EnsureLoggedIn();
    if (_currentPresence != value) {
        VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
        TSharedPtr<LoginSession> protect = SharedThis(this);
        innerDelegate.BindLambda([this, protect](const vx_resp_base_t &resp)
        {
            ;
        }
        );
        VivoxCoreError error = VivoxNativeSdk::Get().SetPresence(_loginSessionId.ToString(), value.Message(), static_cast<vx_buddy_presence_state>(value.CurrentStatus()));
        if (error == VxErrorSuccess) {
            _currentPresence = value;
        }
        return error;
    }
    return VxErrorSuccess;
}

const AccountId& LoginSession::LoginSessionId() const
{
    return _loginSessionId;
}

VivoxCoreError LoginSession::BeginLogin(const FString& server,
    const FString& accessToken,
    SubscriptionMode subscriptionMode,
    const TSet<AccountId>& presenceSubscriptions,
    const TSet<AccountId>& blockedPresenceSubscriptions,
    const TSet<AccountId>& allowedPresenceSubscriptions,
    FOnBeginLoginCompletedDelegate theDelegate)
{
    ensure(!server.IsEmpty());
    ensure(!accessToken.IsEmpty());
    if (_state != LoginState::LoggedOut) return VxErrorInvalidState;

    InitEventHandler();

    ClientImpl::FOnBeginGetConnectorHandleCompletedDelegate connectorDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    connectorDelegate.BindLambda([this, accessToken, subscriptionMode, presenceSubscriptions, blockedPresenceSubscriptions, allowedPresenceSubscriptions, theDelegate, protect](VivoxCoreError error, const FString &connectorHandle)
    {
        if (error == VxErrorSuccess)
        {
            VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
            innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
            {
                if (resp.return_code == 0)
                {
                    if (_state == LoginState::LoggingIn)
                    {
                        SetState(LoginState::LoggedIn);
                    }
                }
                else {
                    SetState(LoginState::LoggedOut);
                }
                theDelegate.ExecuteIfBound(resp.status_code);
            });
            _presenceEnabled = true;
            VivoxCoreError error2 = VivoxNativeSdk::Get().Login(connectorHandle, _loginSessionId, accessToken, GetParticipantUpdateRateForCore(), _presenceEnabled, subscriptionMode, presenceSubscriptions, blockedPresenceSubscriptions, allowedPresenceSubscriptions, innerDelegate);
            if (error2 != VxErrorSuccess)
            {
                SetState(LoginState::LoggedOut);
                theDelegate.ExecuteIfBound(error);
            }
        }
        else
        {
            SetState(LoginState::LoggedOut);
            theDelegate.ExecuteIfBound(error);
        }
    });
    VivoxCoreError error = _client.BeginGetConnectorHandle(server, connectorDelegate);
    if (error != VxErrorSuccess)
        return error;
    SetState(LoginState::LoggingIn);
    return error;
}

VivoxCoreError LoginSession::BeginLogin(const FString& server, const FString& accessToken, FOnBeginLoginCompletedDelegate theDelegate)
{
    ensure(!server.IsEmpty());
    ensure(!accessToken.IsEmpty());
    if (_state != LoginState::LoggedOut) return VxErrorInvalidState;

    InitEventHandler();

    ClientImpl::FOnBeginGetConnectorHandleCompletedDelegate connectorDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);

    connectorDelegate.BindLambda([this, accessToken, theDelegate, protect](VivoxCoreError error, const FString &connectorHandle)
    {
        if (error == VxErrorSuccess)
        {
            VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
            innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
            {
                if (resp.return_code == 0)
                {
                    if (_state == LoginState::LoggingIn)
                    {
                        SetState(LoginState::LoggedIn);
                    }
                }
                else {
                    SetState(LoginState::LoggedOut);
                }
                theDelegate.ExecuteIfBound(resp.status_code);
            });
            _presenceEnabled = false;
            VivoxCoreError error2 = VivoxNativeSdk::Get().Login(connectorHandle, _loginSessionId, accessToken, GetParticipantUpdateRateForCore(), _presenceEnabled, SubscriptionMode::Accept, TSet<AccountId>(), TSet<AccountId>(), TSet<AccountId>(), innerDelegate);
            if (error2 != VxErrorSuccess)
            {
                SetState(LoginState::LoggedOut);
                theDelegate.ExecuteIfBound(error);
            }
        }
        else
        {
            SetState(LoginState::LoggedOut);
            theDelegate.ExecuteIfBound(error);
        }
    });
    VivoxCoreError error = _client.BeginGetConnectorHandle(server, connectorDelegate);
    if (error != VxErrorSuccess)
        return error;
    SetState(LoginState::LoggingIn);
    return error;
}

IChannelSession& LoginSession::GetChannelSession(const ChannelId &channelId)
{
    ensure(!channelId.IsEmpty());
    TSharedPtr<IChannelSession> returnValue;
    if (!_channelSessions.Contains(channelId))
    {
        returnValue = TSharedPtr<IChannelSession>(new ChannelSession(*this, _groupHandle, channelId));
        returnValue->EventChannelStateChanged.AddLambda([this](const IChannelConnectionState& channelConnectionState) {
            HandleChannelConnectionStateChanged(channelConnectionState);
            });
        _channelSessions.Add(channelId, returnValue);
    } else
    {
        returnValue = *_channelSessions.Find(channelId);
    }
    return *returnValue;
}

void LoginSession::DeleteChannelSession(const ChannelId &channelId)
{
    if (!_channelSessions.Contains(channelId))
    {
        UE_LOG(VivoxCore, Warning, TEXT("%s: deleting non-existent channel %s"), *CUR_CLASS_FUNC, *channelId.ToString());
    }
    else if (_channelSessions[channelId].Get()->ChannelState() == ConnectionState::Disconnected)
    {
        UE_LOG(VivoxCore, Log, TEXT("%s: deleting existing, disconnected channel %s"), *CUR_CLASS_FUNC, *channelId.ToString());
        _channelSessions[channelId]->EventChannelStateChanged.RemoveAll(this);
        _channelSessions.Remove(channelId);
    }
    else
    {
        UE_LOG(VivoxCore, Log, TEXT("%s: deleting existing, connected, connecting or disconnecting channel %s"), *CUR_CLASS_FUNC, *channelId.ToString());
        _channelSessions[channelId]->Disconnect(true);
    }
    return;
}

VivoxCoreError LoginSession::BeginSetCrossMutedCommunications(const AccountId &accountId, const bool &muted, FOnBeginSetCrossMutedCommunicationsCompletedDelegate theDelegate)
{
    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect, accountId, muted](const vx_resp_base_t &resp)
    {
        if (resp.return_code == 0)
        {
            const vx_resp_account_control_communications_t &response = reinterpret_cast<const vx_resp_account_control_communications_t &>(resp);
            FString blockedAccount = response.blocked_uris;
            AccountId currentlyBlocking = AccountId::CreateFromUri(blockedAccount, TOptional<FString>(), _loginSessionId.UnityEnvironmentId().IsEmpty() ? TOptional<FString>() : _loginSessionId.UnityEnvironmentId());
            if (currentlyBlocking == accountId && !_crossMutedCommunications.Contains(accountId) && muted)
            {
                _crossMutedCommunications.Add(accountId);
            }
            else if (currentlyBlocking == accountId && _crossMutedCommunications.Contains(accountId) && !muted)
            {
                _crossMutedCommunications.Remove(accountId);
            }
        }
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().ControlCommunications(_loginSessionId.ToString(), muted ? vx_control_communications_operation_block : vx_control_communications_operation_unblock, accountId.ToString(), mute_scope_all, innerDelegate);
    return error;
}

VivoxCoreError LoginSession::BeginSetCrossMutedCommunications(const TSet<AccountId> &accountIdSet, const bool &muted, FOnBeginSetCrossMutedCommunicationsCompletedDelegate theDelegate)
{
    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect, accountIdSet, muted](const vx_resp_base_t &resp)
    {
        if (resp.return_code == 0)
        {
            const vx_resp_account_control_communications_t &response = reinterpret_cast<const vx_resp_account_control_communications_t &>(resp);
            FString changedAccounts = response.blocked_uris;
            TSet<AccountId> changedAccountIds;
            AccountId currentlyChanging;
            while (changedAccounts.Contains("\n"))
            {
                FString left, right;
                changedAccounts.Split("\n", &left, &right);
                currentlyChanging = AccountId::CreateFromUri(left, TOptional<FString>(), _loginSessionId.UnityEnvironmentId().IsEmpty() ? TOptional<FString>() :_loginSessionId.UnityEnvironmentId());
                changedAccountIds.Add(currentlyChanging);
                changedAccounts = right;
                if (!changedAccounts.Contains("\n"))
                {
                    changedAccountIds.Add(AccountId::CreateFromUri(right, TOptional<FString>(), _loginSessionId.UnityEnvironmentId().IsEmpty() ? TOptional<FString>() : _loginSessionId.UnityEnvironmentId()));
                }
            }
            for (auto& Elem : accountIdSet)
            {
                if (changedAccountIds.Contains(Elem) && !_crossMutedCommunications.Contains(Elem) && muted)
                {
                    _crossMutedCommunications.Add(Elem);
                }
                else if (changedAccountIds.Contains(Elem) && _crossMutedCommunications.Contains(Elem) && !muted)
                {
                    _crossMutedCommunications.Remove(Elem);
                }
            }
        }
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    FString formattedList = "";
    for (auto& accountId : accountIdSet)
    {
        if (_crossMutedCommunications.Contains(accountId) && muted)
        {
            continue;
        }
        else if (!_crossMutedCommunications.Contains(accountId) && !muted)
        {
            continue;
        }
        formattedList = formattedList + accountId.ToString() + "\n";
    }
    VivoxCoreError error = VivoxNativeSdk::Get().ControlCommunications(_loginSessionId.ToString(), muted ? vx_control_communications_operation_block : vx_control_communications_operation_unblock, formattedList, mute_scope_all, innerDelegate);
    return error;
}

VivoxCoreError LoginSession::BeginClearCrossMutedCommunications(FOnBeginClearCrossMutedCommunicationsCompletedDelegate theDelegate)
{
    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        if (resp.status_code == 0)
        {
            _crossMutedCommunications.Empty();
        }
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().ControlCommunications(_loginSessionId.ToString(), vx_control_communications_operation_clear, "", mute_scope_all, innerDelegate);
    return error;
}

VivoxCoreError LoginSession::BeginAddBlockedSubscription(const AccountId& accountId, FOnBeginAddBlockedSubscriptionCompletedDelegate theDelegate)
{
    EnsureLoggedIn();

    if (_blockedSubscriptions.Contains(accountId)) {
        theDelegate.ExecuteIfBound(VxErrorSuccess);
        return VxErrorSuccess;
    }

    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {

        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().AddBlockRule(_loginSessionId.ToString(), accountId, innerDelegate);
    if (!error) {
        _blockedSubscriptions.Add(accountId);
    }
    return error;
}

VivoxCoreError LoginSession::BeginRemoveBlockedSubscription(const AccountId& accountId, FOnBeginRemoveBlockedSubscriptionCompletedDelegate theDelegate)
{
    EnsureLoggedIn();

    if (!_blockedSubscriptions.Contains(accountId)) {
        theDelegate.ExecuteIfBound(VxErrorSuccess);
        return VxErrorSuccess;
    }

    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().DeleteBlockRule(_loginSessionId.ToString(), accountId, innerDelegate);
    if (!error) {
        _blockedSubscriptions.Remove(accountId);
    }
    return error;
}

VivoxCoreError LoginSession::BeginAddAllowedSubscription(const AccountId& accountId, FOnBeginAddAllowedSubscriptionCompletedDelegate theDelegate)
{
    EnsureLoggedIn();

    if (_allowedSubscriptions.Contains(accountId)) {
        theDelegate.ExecuteIfBound(VxErrorSuccess);
        return VxErrorSuccess;
    }

    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().AddAcceptRule(_loginSessionId.ToString(), accountId, innerDelegate);
    if (!error) {
        _allowedSubscriptions.Add(accountId);
    }
    return error;
}

VivoxCoreError LoginSession::BeginRemoveAllowedSubscription(const AccountId& accountId, FOnBeginRemoveAllowedSubscriptionCompletedDelegate theDelegate)
{
    EnsureLoggedIn();

    if (!_allowedSubscriptions.Contains(accountId)) {
        theDelegate.ExecuteIfBound(VxErrorSuccess);
        return VxErrorSuccess;
    }

    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().DeleteAcceptRule(_loginSessionId.ToString(), accountId, innerDelegate);
    if (!error) {
        _allowedSubscriptions.Remove(accountId);
    }
    return error;
}

VivoxCoreError LoginSession::BeginAddPresenceSubscription(const AccountId& accountId, FOnBeginAddPresenceSubscriptionCompletedDelegate theDelegate)
{
    EnsureLoggedIn();

    if (_presenceSubscriptions.Contains(accountId)) {
        theDelegate.ExecuteIfBound(VxErrorSuccess);
        return VxErrorSuccess;
    }

    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().AddPresenceSubscription(_loginSessionId.ToString(), accountId, innerDelegate);
    if (!error) {
        _presenceSubscriptions.Add(accountId, TSharedPtr<PresenceSubscription>(new PresenceSubscription(*this, accountId)));
    }
    return error;
}

VivoxCoreError LoginSession::BeginRemovePresenceSubscription(const AccountId& accountId, FOnBeginRemovePresenceSubscriptionCompletedDelegate theDelegate)
{
    EnsureLoggedIn();

    if (!_presenceSubscriptions.Contains(accountId)) {
        theDelegate.ExecuteIfBound(VxErrorSuccess);
        return VxErrorSuccess;
    }

    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().DeletePresenceSubscription(_loginSessionId.ToString(), accountId, innerDelegate);
    if (!error) {
        _presenceSubscriptions.Remove(accountId);
    }
    return error;
}

VivoxCoreError LoginSession::BeginSendDirectedMessage(const AccountId& accountId, const FString& language, const FString& message, const FString& applicationStanzaNamespace, const FString& applicationStanzaBody, FOnBeginSendDirectedMessageCompletedDelegate theDelegate)
{
    if (!accountId.IsValid()) return VxErrorInvalidArgument;
    if (message.IsEmpty() && applicationStanzaBody.IsEmpty()) return VxErrorInvalidArgument;
    if (!_presenceEnabled) return VxErrorInvalidOperation;
    EnsureLoggedIn();
    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        const vx_resp_account_send_message_t &response = reinterpret_cast<const vx_resp_account_send_message_t &>(resp);
        theDelegate.ExecuteIfBound(resp.status_code, response.request_id);
    });
    return VivoxNativeSdk::Get().SendUserToUserMessage(_loginSessionId.ToString(), accountId, language, message, applicationStanzaNamespace, applicationStanzaBody, innerDelegate);
}

VivoxCoreError LoginSession::BeginSendSubscriptionReply(const AccountId& accountId, const SubscriptionReply& replyType ,FOnBeginSendSubscriptionReplyCompletedDelegate theDelegate)
{
    if (!accountId.IsValid()) return VxErrorInvalidArgument;
    if (replyType != SubscriptionReply::Block && replyType != SubscriptionReply::Allow) return VxErrorInvalidArgument;
    EnsureLoggedIn();
    vx_rule_type subRule = replyType == SubscriptionReply::Allow ? vx_rule_type::rule_allow : vx_rule_type::rule_block;
    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    innerDelegate.BindLambda([this, theDelegate](const vx_resp_base_t &resp)
    {
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    return VivoxNativeSdk::Get().SendSubscriptionReply(_loginSessionId.ToString(), accountId, subRule, innerDelegate);
}

VivoxCoreError LoginSession::BeginSendDirectedMessage(const AccountId& accountId, const FString& message, FOnBeginSendDirectedMessageCompletedDelegate theDelegate)
{
    return BeginSendDirectedMessage(accountId, FString(), message, FString(), FString(), theDelegate);
}

VivoxCoreError LoginSession::BeginStartAudioInjection(const FString &filePath, FOnBeginStartAudioInjectionCompletedDelegate theDelegate)
{
    EnsureLoggedIn();

    // Must be in at least one channel for valid sessiongroup handle
    bool inChannel = false;
    for (auto session : _channelSessions)
    {
        if (session.Value->AudioState() == ConnectionState::Connected)
        {
            inChannel = true;
            break;
        }
    }
    if (!inChannel)
    {
        UE_LOG(VivoxCore, Error, TEXT("%s: failed for InvalidState - Must have an Audio Connected ChannelSession to Start Audio Injection"), *CUR_CLASS_FUNC);
        return VxErrorInvalidState;
    }

    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        if (resp.status_code == VxErrorSuccess)
        {
            _isAudioInjecting = true;
        }
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    return VivoxNativeSdk::Get().StartAudioInjection(_groupHandle, filePath, innerDelegate);
}

VivoxCoreError LoginSession::StopAudioInjection()
{
    EnsureLoggedIn();

    // NB: _isAudioInjecting is set to false when we get evt_media_completion
    return VivoxNativeSdk::Get().StopAudioInjection(_groupHandle);
}

bool LoginSession::IsAudioInjecting()
{
    return _isAudioInjecting;
}

VivoxCoreError LoginSession::SetTransmissionMode(TransmissionMode mode, ChannelId singleChannel)
{
    if (mode == TransmissionMode::Single && singleChannel.IsEmpty()) return VxErrorInvalidArgument;
    _transmissionMode = mode;
    _transmittingChannel = (mode == TransmissionMode::Single) ? singleChannel : ChannelId();

    // check for sessiongroup
    bool sessiongroupExists = false;
    TArray<TSharedPtr<IChannelSession>> sessions;
    _channelSessions.GenerateValueArray(sessions);
    for (TSharedPtr<IChannelSession> session : sessions)
    {
        if (session->AudioState() != ConnectionState::Disconnected || session->TextState() != ConnectionState::Disconnected)
        {
            sessiongroupExists = true;
            break;
        }
    }
    // if a sessiongroup exists and either we're not single or we're in the single channel, set transmission immediately
    if (sessiongroupExists && (_transmissionMode !=TransmissionMode::Single || _channelSessions.Contains(_transmittingChannel)))
        return SetTransmissionInCore();
    return VxErrorSuccess;
}

VivoxCoreError LoginSession::BeginSetSafeVoiceConsentStatus(const bool &consentToSet, const FString &environmentId, const FString &projectId, const FString &UASToken, FOnBeginSetSafeVoiceConsentCompletedDelegate theDelegate)
{
    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t& resp)
        {
            const vx_resp_account_safe_voice_update_consent_t& response = reinterpret_cast<const vx_resp_account_safe_voice_update_consent_t&>(resp);
            theDelegate.ExecuteIfBound(resp.status_code, response.consent_status);
        });
    return VivoxNativeSdk::Get().SetSafeVoiceConsentStatus(_loginSessionId.ToString(), consentToSet, _loginSessionId, environmentId, projectId, UASToken, innerDelegate);
}



VivoxCoreError LoginSession::BeginGetSafeVoiceConsentStatus(const FString& environmentId, const FString& projectId, const FString& UASToken, FOnBeginGetSafeVoiceConsentCompletedDelegate theDelegate)
{
    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<LoginSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t& resp)
        {
            const vx_resp_account_safe_voice_get_consent_t& response = reinterpret_cast<const vx_resp_account_safe_voice_get_consent_t&>(resp);
            theDelegate.ExecuteIfBound(resp.status_code, response.consent_status);
        });
    return VivoxNativeSdk::Get().GetSafeVoiceConsentStatus(_loginSessionId.ToString(), _loginSessionId, environmentId, projectId, UASToken, innerDelegate);
}

TransmissionMode LoginSession::GetTransmissionMode() const
{
    return _transmissionMode;
}

TArray<ChannelId> LoginSession::GetTransmittingChannels() const
{
    TArray<ChannelId> channels;
    switch (_transmissionMode)
    {
    case TransmissionMode::Single:
        channels.Add(_transmittingChannel);
        break;
    case TransmissionMode::All:
        for (auto session : _channelSessions)
            channels.Add(session.Key);
        break;
    case TransmissionMode::None:
    default:
        break; // leave channels empty
    }
    return channels;
}

VivoxCoreError LoginSession::SetParticipantSpeakingUpdateRate(ParticipantSpeakingUpdateRate rate)
{
    if (rate == _participantUpdateRate) return VxErrorSuccess;
    _participantUpdateRate = rate;

    // If we're not logged in from the Core SDK perspective, we can just set the value and wait for login
    if (_state == LoginState::LoggedIn || (_state == LoginState::LoggingIn && _client.IsConnected()))
    {
        // If we're already logged in from the Core SDK perspective, set login properties immediately
        return VivoxNativeSdk::Get().SetLoginProperties(_loginSessionId.ToString(), GetParticipantUpdateRateForCore());
    }
    return VxErrorSuccess;
}

ParticipantSpeakingUpdateRate LoginSession::GetParticipantSpeakingUpdateRate() const
{
    return _participantUpdateRate;
}

ITextToSpeech &LoginSession::TTS()
{
    return _ttsSubSystem;
}

void LoginSession::Logout()
{
    if (_state == LoginState::LoggedIn || _state == LoginState::LoggingIn)
    {
        VivoxNativeSdk::Get().Logout(_loginSessionId.ToString());
        SetState(LoginState::LoggingOut);
    }
}

FString LoginSession::GetLoginToken(const FString& tokenSigningKey, FTimespan tokenExpirationDuration) const
{
    return VivoxNativeSdk::GetLoginToken(this->_loginSessionId, tokenSigningKey, tokenExpirationDuration);
}

void LoginSession::ClearTransmittingChannel(const ChannelId &channelId)
{
    if (_transmittingChannel == channelId)
    {
        _transmittingChannel = ChannelId();
        _transmissionMode = TransmissionMode::None;
    }
    // TODO: raise a property changed event here?
}

VivoxCoreError LoginSession::SetTransmissionInCore()
{
    // TODO: raise a property changed event here?
    switch (_transmissionMode)
    {
    case TransmissionMode::None:
        return VivoxNativeSdk::Get().SetNoSessionTransmitting(_groupHandle);
    case TransmissionMode::Single:
        // In very rare corner cases or race conditions, Find() can return a nullptr, so guard against it
        TSharedPtr<IChannelSession> *transmittingChannelSessionPtr;
        transmittingChannelSessionPtr = _channelSessions.Find(_transmittingChannel);
        if (transmittingChannelSessionPtr == nullptr)
        {
            // return a suitable error, but do not otherwise try to correct state
            UE_LOG(VivoxCore, Warning, TEXT("An internal guard prevented audio transmission from being set to a single specific channel for which no channel session exists (\"%s\")."), *_transmittingChannel.ToString());
            return VxErrorTargetObjectDoesNotExist;
        }
        return VivoxNativeSdk::Get().SetTransmittingSession(static_cast<ChannelSession*>(transmittingChannelSessionPtr->Get())->GetSessionHandle());
    case TransmissionMode::All:
        return VivoxNativeSdk::Get().SetAllSessionsTransmitting(_groupHandle);
    }
    return VxErrorInternalError;
}

int LoginSession::GetParticipantUpdateRateForCore() const
{
    switch (_participantUpdateRate)
    {
    case ParticipantSpeakingUpdateRate::StateChange:
        return 100;
    case ParticipantSpeakingUpdateRate::Update1Hz:
        return 50; // 50*20ms = 1000ms
    case ParticipantSpeakingUpdateRate::Update5Hz:
        return 10; // 10*20ms = 200ms
    case ParticipantSpeakingUpdateRate::Update10Hz:
        return 5;  //  5*20ms = 100ms
    case ParticipantSpeakingUpdateRate::Never:
        return 0;
    default:
        return 100; // should never happen, but default to StateChange
    }
}

void LoginSession::HandleChannelConnectionStateChanged(const IChannelConnectionState& connectionState)
{
    if (connectionState.State() == ConnectionState::Connected)
    {
        EventChannelJoined.Broadcast(connectionState.ChannelSession());
        return;
    }
    if (connectionState.State() == ConnectionState::Disconnected)
    {
        EventChannelLeft.Broadcast(connectionState.ChannelSession());
        return;
    }
}
