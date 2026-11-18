/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "IClient.h"
#include "AudioDevicesImpl.h"

struct vx_evt_base;
typedef vx_tts_manager_id TTSManagerId;

class ClientImpl : public IClient
{
public:
    DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateBeginGetConnectorHandleCompleted, VivoxCoreError, const FString &)
    typedef FDelegateBeginGetConnectorHandleCompleted::FDelegate FOnBeginGetConnectorHandleCompletedDelegate;
private:
    bool _initialized;
    TMap<AccountId, TSharedPtr<ILoginSession> > _loginSessions;
    AudioDevicesImpl _audioInputDevices;
    AudioDevicesImpl _audioOutputDevices;
    FString _connectorHandle;
    TArray<FOnBeginGetConnectorHandleCompletedDelegate> _pendingConnects;
    bool _ttsIsInitialized;
    TTSManagerId _ttsManagerId;
    void Cleanup();
public:
    ClientImpl();
    ~ClientImpl();

    void Tick(float DeltaTime);

    // IClient
    VivoxCoreError Initialize(VivoxConfig config) override;
    void Uninitialize() override;
    ILoginSession &GetLoginSession(const AccountId &loginSessionId) override;
    const TMap<AccountId, TSharedPtr<ILoginSession>> & LoginSessions() override;
    IAudioDevices& AudioInputDevices() override;
    IAudioDevices& AudioOutputDevices() override;

    // internal
    VivoxCoreError BeginGetConnectorHandle(const FString &server, FOnBeginGetConnectorHandleCompletedDelegate theDelegate);
    bool IsConnected() { return !_connectorHandle.IsEmpty(); }
    const TTSManagerId &GetTTSManagerId() { return _ttsManagerId; }
    bool TTSInitialize();
    void TTSShutdown();
};
