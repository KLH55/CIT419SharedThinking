/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "IChannelSession.h"
#include "VxcEvents.h"
#include "Templates/SharedPointer.h"

class LoginSession;

/**
 *
 */
class ChannelSession : public IChannelSession, public TSharedFromThis<ChannelSession>
{
    LoginSession &_loginSession;
    ConnectionState _audioState;
    ConnectionState _textState;
    ConnectionState _channelState;
    TMap<FString, IParticipant*> _participants;
    bool _typing;
    bool _isSessionBeingTranscribed;
    ChannelId _channel;
    FString _groupHandle;
    FString _sessionHandle;
    FDelegateHandle _eventSdkEventRaised;
    bool _detached;
    bool _toBeDeleted;
    bool _firstConnect;

    void SetAudioState(ConnectionState value);
    void SetTextState(ConnectionState value);
    void SetChannelState(ConnectionState value);
    void CheckSessionConnection();
    void HandleEvent(const vx_evt_base_t &evt);
    template<class T>
    bool IsMine(const T &evt)
    {
        return _sessionHandle == evt.session_handle;
    }

    void CleanupEventHandler();
public:
    ChannelSession(LoginSession &parent, const FString &sessionGroupHandle, const ChannelId &id);
    ~ChannelSession();
    FString SessionHandle();
    ILoginSession &Parent() override;
    ConnectionState AudioState() const override;
    ConnectionState TextState() const override;
    ConnectionState ChannelState() const override;
    const TMap<FString, IParticipant*>& Participants() const override;
    bool Typing() const override;
    void SetTyping(bool value) override;
    bool IsTransmitting() const override;
    ChannelId Channel() const override;
    VivoxCoreError BeginConnect(bool connectAudio, bool connectText, bool switchTransmission, const FString& accessToken, FOnBeginConnectCompletedDelegate theDelegate) override;
    void Disconnect(bool deleteOnDisconnect = false) override;
    VivoxCoreError BeginSetAudioConnected(bool value, bool switchTransmission, FOnBeginSetAudioConnectedCompletedDelegate theDelegate) override;
    VivoxCoreError BeginSetTextConnected(bool value, FOnBeginSetTextConnectedCompletedDelegate theDelegate) override;
    VivoxCoreError BeginSendText(const FString& language, const FString& message, const FString& applicationStanzaNamespace, const FString& applicationStanzaBody, FOnBeginSendTextCompletedDelegate theDelegate) override;
    VivoxCoreError BeginSendText(const FString& message, FOnBeginSendTextCompletedDelegate theDelegate) override;
    VivoxCoreError Set3DPosition(const FVector &speakerPosition, const FVector &listenerPosition, const FVector &listenerForwardVector, const FVector &listenerUpVector) override;
    FString GetConnectToken(const FString& tokenSigningKey, FTimespan tokenExpirationDuration) const override;
    bool IsSessionBeingTranscribed() const override;
    VivoxCoreError BeginSetChannelTranscription(bool value, const FString &accessToken, FOnBeginSetChannelTranscriptionCompletedDelegate theDelegate) override;
    FString GetTranscriptionToken(const FString & tokenSigningKey, FTimespan tokenExpirationDuration) const override;
    void Detach();
    void Attach();

    // Internal
    FString GetSessionHandle() { return _sessionHandle; }
};
