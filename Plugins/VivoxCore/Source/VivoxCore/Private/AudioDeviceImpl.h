/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "Engine/Blueprint.h"
#include "IAudioDevices.h"

/**
 * \brief The type of device
 */
UENUM(BlueprintType)
enum class DeviceType : uint8
{
    /**
     * This is the null device, which means that either input or output from/to that device will not occur.
     */
    NullDevice = 0,
    /**
     * This type is a specific device.
     */
    SpecificDevice,
    /**
     * This type means to use what ever the system has configured as a default, at the time of the call.
     * Don't switch devices mid-call if the default system device changes.
     */
    DefaultSystemDevice,
    /**
     * This type means to use what ever the system has configured as a default communication device, at the time of the call.
     * Don't switch devices mid-call if the default communication device changes.
     */
    DefaultCommunicationDevice
};

/**
 *
 */
class AudioDeviceImpl : public IAudioDevice
{
    FString _name;
    FString _id;
    DeviceType _type;

public:
	AudioDeviceImpl();
    AudioDeviceImpl(const FString &name, const FString &id, DeviceType type);
    ~AudioDeviceImpl();
    const FString& Name() const override { return _name; }
    const FString& Id() const override { return _id; }
    DeviceType Type() const { return _type; } // no override: not in interface
    bool IsEmpty() const override { return _name.IsEmpty() && _id.IsEmpty(); } // don't consider _type
};
