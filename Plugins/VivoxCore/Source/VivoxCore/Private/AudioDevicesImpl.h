/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "IAudioDevices.h"
#include "AudioDeviceImpl.h"
#include "VivoxCoreCommonImpl.h"
#include "VxcEvents.h"
#include "VxcResponses.h"

enum class AudioDeviceType : uint8
{
    Input,
    Output
};
/**
 *
 */
class AudioDevicesImpl : public IAudioDevices
{
    AudioDeviceImpl _noDevice;
    AudioDeviceImpl _systemDevice;
    AudioDeviceImpl _communicationDevice;
    TMap<FString, IAudioDevice*> _availableDevices;
    FString _activeDeviceId;
    FString _effectiveDeviceId;
    int _volumeAdjustment;
    bool _muted;
    AudioDeviceType _type;

    void CheckAndSetNewEffectiveDevice(FString newEffectiveId);
    void Handle(const vx_evt_audio_device_hot_swap_t &r);
    void Handle(const vx_resp_aux_get_capture_devices_t &r);
    void Handle(const vx_resp_aux_get_render_devices_t &r);
    void Handle(const vx_evt_base_t &message);

public:
	AudioDevicesImpl(AudioDeviceType type);
	~AudioDevicesImpl();

    void Initialize();

    const IAudioDevice& SystemDevice() const override;
    const IAudioDevice& CommunicationDevice() const override;
    const IAudioDevice& NullDevice() const override;
    const TMap<FString, IAudioDevice*>& AvailableDevices() const override;
    VivoxCoreError SetActiveDevice(const IAudioDevice &deviceId, const FOnSetActiveDeviceCompletedDelegate& Delegate) override;
    const IAudioDevice& ActiveDevice() override;
    const IAudioDevice& EffectiveDevice() override;
    int VolumeAdjustment() override;
    VivoxCoreError SetVolumeAdjustment(int value) override;
    bool Muted() const override;
    void SetMuted(bool value) override;

    void Refresh();
};
