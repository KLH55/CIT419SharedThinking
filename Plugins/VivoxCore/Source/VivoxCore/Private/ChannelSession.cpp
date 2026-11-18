/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "ChannelSession.h"
#include "VivoxCore.h"
#include "LoginSession.h"
#include "VxcErrors.h"
#include "VivoxNativeSdk.h"
#include "Participant.h"
#include "ChannelTextMessage.h"
#include "TranscribedMessage.h"
#include "ChannelConnectionState.h"

#define CUR_CLASS_FUNC (FString(__FUNCTION__))

ChannelSession::ChannelSession(LoginSession &parent, const FString &sessionGroupHandle, const ChannelId &id) :
    _loginSession(parent),
    _audioState(ConnectionState::Disconnected),
    _textState(ConnectionState::Disconnected),
    _channelState(ConnectionState::Disconnected),
    _typing(false),
    _isSessionBeingTranscribed(false),
    _channel(id),
    _groupHandle(sessionGroupHandle),
    _detached(true),
    _toBeDeleted(false),
    _firstConnect(true)
{
    UE_LOG(VivoxCore, Log, TEXT("ChannelSession created for %s"), *_channel.ToString());
    ensure(_channel.IsValid());
    _sessionHandle = _groupHandle + TEXT("_") + id.Name();
}

ChannelSession::~ChannelSession()
{
    Detach();
    UE_LOG(VivoxCore, Log, TEXT("ChannelSession destroyed for %s"), *_channel.ToString());
    CleanupEventHandler();
}

void ChannelSession::CleanupEventHandler()
{
    if (_eventSdkEventRaised.IsValid()) {
        VivoxNativeSdk::Get().EventSdkEventRaised.Remove(_eventSdkEventRaised);
        _eventSdkEventRaised = FDelegateHandle();
    }
}

FString ChannelSession::SessionHandle()
{
    return _sessionHandle;
}

ILoginSession & ChannelSession::Parent()
{
    return _loginSession;
}

ConnectionState ChannelSession::AudioState() const
{
    return _audioState;
}

ConnectionState ChannelSession::TextState() const
{
    return _textState;
}

ConnectionState ChannelSession::ChannelState() const
{
    return _channelState;
}

const TMap<FString, IParticipant*>& ChannelSession::Participants() const
{
    return _participants;
}

bool ChannelSession::Typing() const
{
    return _typing;
}

void ChannelSession::SetTyping(bool value)
{
    unimplemented();
}

bool ChannelSession::IsTransmitting() const
{
    return _loginSession.GetTransmittingChannels().Contains(_channel);
}

void ChannelSession::HandleEvent(const vx_evt_base_t& evt)
{
    switch (evt.type)
    {
    case evt_participant_added:
    {
        const vx_evt_participant_added_t &tevt(reinterpret_cast<const vx_evt_participant_added_t &>(evt));
        if (!IsMine(tevt)) return;
        if (!_participants.Contains(AccountId::AccountNameFromUri(tevt.participant_uri)))
        {
            Participant *participant = new Participant(*this, tevt);
            _participants.Add(participant->Account().Name(), participant);
            EventAfterParticipantAdded.Broadcast(*participant);
            if (participant->IsSelf())
                CheckSessionConnection();
        } else {
            UE_LOG(VivoxCore, Warning, TEXT("Dropping participant added event for participant already in session (%s)"), *FString(tevt.participant_uri));
        }
    }
    break;
    case evt_participant_removed:
    {
        const vx_evt_participant_removed_t &tevt(reinterpret_cast<const vx_evt_participant_removed_t &>(evt));
        if (!IsMine(tevt)) return;
        FString ParticipantKey = AccountId::AccountNameFromUri(tevt.participant_uri);
        if (_participants.Contains(ParticipantKey)) {
        Participant *participant = static_cast<Participant*>(_participants[ParticipantKey]);
            participant->HandleEvent(tevt);
            EventBeforeParticipantRemoved.Broadcast(*participant);
            _participants.Remove(ParticipantKey);
            delete participant;
        } else {
            UE_LOG(VivoxCore, Warning, TEXT("Dropping participant removed event for participant not in session (%s)"), *FString(tevt.participant_uri));
        }
    }
    break;
    case evt_participant_updated:
    {
        const vx_evt_participant_updated_t &tevt(reinterpret_cast<const vx_evt_participant_updated_t &>(evt));
        if (!IsMine(tevt)) return;
        FString ParticipantKey = AccountId::AccountNameFromUri(tevt.participant_uri);
        if (_participants.Contains(ParticipantKey)) {
            Participant *participant = static_cast<Participant*>(_participants[ParticipantKey]);
            if (participant->HandleEvent(tevt)) {
                EventAfterParticipantUpdated.Broadcast(*participant);
            }
        } else {
            UE_LOG(VivoxCore, Warning, TEXT("Dropping participant updated event for participant not in session (%s)"), *FString(tevt.participant_uri));
        }
    }
    break;
    case evt_media_stream_updated:
    {
        const vx_evt_media_stream_updated_t &tevt(reinterpret_cast<const vx_evt_media_stream_updated_t &>(evt));
        if (!IsMine(tevt)) return;
        if (tevt.state == session_media_connected && _audioState == ConnectionState::Connecting)
        {
            SetAudioState(ConnectionState::Connected);
        } else if (tevt.state == session_media_disconnected)
        {
            if (AudioState() == ConnectionState::Connecting && ChannelState() != ConnectionState::Connected)
            {
                Disconnect();
            }
            SetAudioState(ConnectionState::Disconnected);
        }
    }
    break;
    case evt_text_stream_updated:
    {
        const vx_evt_text_stream_updated_t &tevt(reinterpret_cast<const vx_evt_text_stream_updated_t &>(evt));
        if (!IsMine(tevt)) return;
        if (tevt.state == session_text_connected && _textState == ConnectionState::Connecting)
        {
            SetTextState(ConnectionState::Connected);
        } else if (tevt.state == session_text_disconnected)
        {
            if (TextState() == ConnectionState::Connecting && ChannelState() != ConnectionState::Connected)
            {
                Disconnect();
            }
            SetTextState(ConnectionState::Disconnected);
        }
    }
    break;
    case evt_session_removed:
    {
        const vx_evt_session_removed_t &tevt(reinterpret_cast<const vx_evt_session_removed_t &>(evt));
        if (!IsMine(tevt)) return;
        SetAudioState(ConnectionState::Disconnected);
        SetTextState(ConnectionState::Disconnected);
        auto keepChannelSessionAliveUntilEndOfScope = SharedThis(this); // Keeps ChannelSession object alive within the remainder of the scope.
        SetChannelState(ConnectionState::Disconnected); // If this call causes a logout, the SharedThis from above stops ChannelSession from being destroyed too early.
        _participants.Empty();
        _typing = false;
        Detach();
        if (_toBeDeleted)
        {
            Parent().DeleteChannelSession(_channel);
        }
    }
    break;
    case evt_message:
    {
        const vx_evt_message_t &tevt(reinterpret_cast<const vx_evt_message_t &>(evt));
        if (!IsMine(tevt)) return;

        if(_textState == ConnectionState::Connected) {
            ChannelTextMessage msg(*this, tevt);
            EventTextMessageReceived.Broadcast(msg);
        }
    }
    break;
    case evt_transcribed_message:
    {
        const vx_evt_transcribed_message_t &tevt(reinterpret_cast<const vx_evt_transcribed_message_t &>(evt));
        if (!IsMine(tevt)) return;

        // ignore transcribed messages if we've disconnected audio
        if (_audioState == ConnectionState::Connected) {
            TranscribedMessage msg(*this, tevt);
            EventTranscribedMessageReceived.Broadcast(msg);
        }
    }
    break;
    default:
        break;
    }
}

ChannelId ChannelSession::Channel() const
{
    return _channel;
}

VivoxCoreError ChannelSession::BeginConnect(bool connectAudio, bool connectText, bool switchTransmission, const FString& accessToken, FOnBeginConnectCompletedDelegate theDelegate)
{
    if (accessToken.IsEmpty()) return VxErrorInvalidArgument;
    if (!connectAudio && !connectText) return VxErrorInvalidArgument;
    if (!_channel.IsValid()) return VxErrorInvalidArgument;
    if (ChannelState() != ConnectionState::Disconnected)
    {
        return VxErrorInvalidState;
    }
    Attach();
    if (_firstConnect)
    {
        _firstConnect = false;
#if (ENGINE_MAJOR_VERSION == 4)
        TWeakPtr<ChannelSession> weakThis = TWeakPtr<ChannelSession>(this->AsShared());
#else
        TWeakPtr<ChannelSession> weakThis = this->AsWeak();
#endif
        _eventSdkEventRaised = VivoxNativeSdk::Get().EventSdkEventRaised.AddLambda([weakThis](const vx_evt_base_t& evt)
        {
            TSharedPtr<ChannelSession> sharedThis = weakThis.Pin();
            if (!sharedThis.IsValid()) return;
            if (sharedThis->_detached) return;
            sharedThis->HandleEvent(evt);
        });
    }
    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<ChannelSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        if(resp.return_code != 0) {
            SetAudioState(ConnectionState::Disconnected);
            SetTextState(ConnectionState::Disconnected);
        }
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().AddSession(
        _loginSession.LoginSessionId().ToString(),
        _groupHandle,
        _sessionHandle,
        accessToken,
        _channel, connectAudio, connectText, innerDelegate);
    if(error != VxErrorSuccess) {
        return error;
    }

    // set connecting state before switching transmission
    if (connectAudio)
        SetAudioState(ConnectionState::Connecting);
    if (connectText)
        SetTextState(ConnectionState::Connecting);
    SetChannelState(ConnectionState::Connecting);

    if (switchTransmission)
    {
        if (!connectAudio)
            UE_LOG(VivoxCore, Warning, TEXT("Switching audio transmission exclusively to text-only channel -- this is allowed but unusual."));
        error = _loginSession.SetTransmissionMode(TransmissionMode::Single, _channel);
    } else { // make tx requests based on stored state (this is the first opportunity to make ones set up ahead of time)
        error = _loginSession.SetTransmissionInCore();
    }
    return error;
}

void ChannelSession::Disconnect(bool deleteOnDisconnect)
{
    _toBeDeleted = deleteOnDisconnect;
    _loginSession.ClearTransmittingChannel(_channel);
    if (AudioState() == ConnectionState::Connecting || AudioState() == ConnectionState::Connected ||
        TextState() == ConnectionState::Connecting || TextState() == ConnectionState::Connected)
    {
        VivoxNativeSdk::Get().RemoveSession(_groupHandle, _sessionHandle, VivoxNativeSdk::FOnRequestCompletedDelegate());
    }

    if (AudioState() != ConnectionState::Disconnected)
        SetAudioState(ConnectionState::Disconnecting);
    if (TextState() != ConnectionState::Disconnected)
        SetTextState(ConnectionState::Disconnecting);
    if (ChannelState() != ConnectionState::Disconnected)
        SetChannelState(ConnectionState::Disconnecting);
}

static bool AlreadyDone(bool connect, ConnectionState state)
{
    if (connect)
    {
        return state == ConnectionState::Connected || state == ConnectionState::Connecting;
    } else
    {
        return state == ConnectionState::Disconnected || state == ConnectionState::Disconnecting;
    }
}

VivoxCoreError ChannelSession::BeginSetAudioConnected(bool value, bool switchTransmission, FOnBeginSetAudioConnectedCompletedDelegate theDelegate)
{
    if (value && switchTransmission)
    {
        _loginSession.SetTransmissionMode(TransmissionMode::Single, _channel);
    }
    if (AlreadyDone(value, _audioState))
    {
        theDelegate.ExecuteIfBound(VxErrorSuccess);
        return VxErrorSuccess;
    }
    if (value)
    {
        VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
        TSharedPtr<ChannelSession> protect = SharedThis(this);
        innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
        {
            theDelegate.ExecuteIfBound(resp.status_code);
        });
        VivoxCoreError error = VivoxNativeSdk::Get().ConnectMedia(_sessionHandle, innerDelegate);
        if (error != VxErrorSuccess)
            return error;
        SetAudioState(ConnectionState::Connecting);
        return error;
    } else {
        _loginSession.ClearTransmittingChannel(_channel);
        VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
        TSharedPtr<ChannelSession> protect = SharedThis(this);
        innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
        {
            theDelegate.ExecuteIfBound(resp.status_code);
        });
        VivoxCoreError error = VivoxNativeSdk::Get().DisconnectMedia(_sessionHandle, innerDelegate);
        if (error != VxErrorSuccess)
            return error;
        SetAudioState(ConnectionState::Disconnecting);
        return error;
    }
}

VivoxCoreError ChannelSession::BeginSetTextConnected(bool value, FOnBeginSetTextConnectedCompletedDelegate theDelegate)
{
    if (AlreadyDone(value, _textState))
    {
        theDelegate.ExecuteIfBound(VxErrorSuccess);
        return VxErrorSuccess;
    }
    if (value)
    {
        VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
        TSharedPtr<ChannelSession> protect = SharedThis(this);
        innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
        {
            theDelegate.ExecuteIfBound(resp.status_code);
        });
        VivoxCoreError error = VivoxNativeSdk::Get().ConnectText(_sessionHandle, innerDelegate);
        if (error != VxErrorSuccess)
            return error;
        SetTextState(ConnectionState::Connecting);
        return error;
    } else {
        VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
        TSharedPtr<ChannelSession> protect = SharedThis(this);
        innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
        {
            theDelegate.ExecuteIfBound(resp.status_code);
        });
        VivoxCoreError error = VivoxNativeSdk::Get().DisconnectText(_sessionHandle, innerDelegate);
        if (error != VxErrorSuccess)
            return error;
        SetTextState(ConnectionState::Disconnecting);
        return error;
    }
}

VivoxCoreError ChannelSession::BeginSendText(const FString& language,
    const FString& message,
    const FString& applicationStanzaNamespace,
    const FString& applicationStanzaBody,
    FOnBeginSendTextCompletedDelegate theDelegate)
{
    if (message.IsEmpty()) return VxErrorInvalidArgument;

    if (TextState() != ConnectionState::Connected) return VxErrorInvalidState;
    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<ChannelSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().SendMessage(_sessionHandle, language, message, applicationStanzaNamespace, applicationStanzaBody, innerDelegate);
    return error;
}

VivoxCoreError ChannelSession::BeginSendText(const FString& message, FOnBeginSendTextCompletedDelegate theDelegate)
{
    return BeginSendText(FString(), message, FString(), FString(), theDelegate);
}

VivoxCoreError ChannelSession::Set3DPosition(const FVector &speakerPosition, const FVector &listenerPosition, const FVector &listenerForwardVector, const FVector &listenerUpVector)
{
    // Session must have Audio Connected for 3D position data to be processed correctly
    if (AudioState() != ConnectionState::Connected)
    {
        UE_LOG(VivoxCore, Error, TEXT("%s: failed for InvalidState - Channel AudioState Disconnected"), *CUR_CLASS_FUNC);
        return VxErrorInvalidState;
    }
    else if (_channel.Type() != ChannelType::Positional)
    {
        UE_LOG(VivoxCore, Error, TEXT("%s: failed for InvalidState - Channel Type must be Positional"), *CUR_CLASS_FUNC);
        return VxErrorInvalidState;
    }

    return VivoxNativeSdk::Get().Set3DPosition(_sessionHandle, speakerPosition, listenerPosition, listenerForwardVector, listenerUpVector);
}

FString ChannelSession::GetConnectToken(const FString& tokenSigningKey, FTimespan tokenExpirationDuration) const
{
    return VivoxNativeSdk::GetJoinToken(this->_loginSession.LoginSessionId(), _channel, tokenSigningKey, tokenExpirationDuration);
}

void ChannelSession::Detach()
{
    if(!_detached)
    {
        _detached = true;
    }
}

void ChannelSession::Attach()
{
    if (_detached)
    {
        _detached = false;
    }
}

void ChannelSession::SetAudioState(ConnectionState value)
{
    if (value == _audioState)
        return;
    _audioState = value;
    ChannelConnectionState state(*this, _audioState);
    EventAudioStateChanged.Broadcast(state);
    if (value == ConnectionState::Connected)
        CheckSessionConnection();
}

void ChannelSession::SetTextState(ConnectionState value)
{
    if (value == _textState)
        return;
    _textState = value;
    ChannelConnectionState state(*this, _textState);
    EventTextStateChanged.Broadcast(state);
    if(value == ConnectionState::Connected)
        CheckSessionConnection();
}

void ChannelSession::SetChannelState(ConnectionState value)
{
    if (value == _channelState)
        return;
    _channelState = value;
    ChannelConnectionState state(*this, value);
    EventChannelStateChanged.Broadcast(state);
}

bool ChannelSession::IsSessionBeingTranscribed() const
{
    return _isSessionBeingTranscribed;
}

void ChannelSession::CheckSessionConnection()
{
    if (!_toBeDeleted && AudioState() != ConnectionState::Connecting && TextState() != ConnectionState::Connecting && Participants().Find(_loginSession.LoginSessionId().Name()))
    {
        SetChannelState(ConnectionState::Connected);
        return;
    }
}

VivoxCoreError ChannelSession::BeginSetChannelTranscription(bool value, const FString &accessToken, FOnBeginSetChannelTranscriptionCompletedDelegate theDelegate)
{
    if (value == _isSessionBeingTranscribed)
    {
        theDelegate.ExecuteIfBound(VxErrorSuccess);
        return VxErrorSuccess;
    }
    else if (value && (AudioState() == ConnectionState::Disconnecting || AudioState() == ConnectionState::Disconnected))
    {
        // print warning and continue
        UE_LOG(VivoxCore, Warning, TEXT("%s: Transcription events will not occur unless Channel AudioState is Connected (since there is no speech)."), *CUR_CLASS_FUNC);
    }

    VivoxNativeSdk::FOnRequestCompletedDelegate innerDelegate;
    TSharedPtr<ChannelSession> protect = SharedThis(this);
    innerDelegate.BindLambda([this, theDelegate, protect](const vx_resp_base_t &resp)
    {
        if (resp.return_code == 0) { // if successful, set current state to requested state, otherwise no change
            _isSessionBeingTranscribed = (bool)reinterpret_cast<vx_req_session_transcription_control_t *>(resp.request)->enable;
        }
        theDelegate.ExecuteIfBound(resp.status_code);
    });
    VivoxCoreError error = VivoxNativeSdk::Get().SetTranscription(value, _sessionHandle, accessToken, innerDelegate);
    return error;
}

FString ChannelSession::GetTranscriptionToken(const FString& tokenSigningKey, FTimespan tokenExpirationDuration) const
{
    return VivoxNativeSdk::GetTranscriptionToken(this->_loginSession.LoginSessionId(), _channel, tokenSigningKey, tokenExpirationDuration);
}