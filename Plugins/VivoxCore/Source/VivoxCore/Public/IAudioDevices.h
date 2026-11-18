/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "IAudioDevice.h"
#include "VivoxCoreCommon.h"

/**
 * \brief An interface to enumerate and manage audio devices.
 */
class VIVOXCORE_API IAudioDevices
{
public:
    /* @cond */
    DECLARE_EVENT_OneParam(IAudioDevices, AfterAvailableDeviceAdded, const IAudioDevice &)
    DECLARE_EVENT_OneParam(IAudioDevices, BeforeAvailableDeviceRemoved, const IAudioDevice &)
    DECLARE_EVENT_OneParam(IAudioDevices, EffectiveDeviceChanged, const IAudioDevice &)
    DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateSetActiveDeviceCompleted, VivoxCoreError, const FString &)
    /* @endcond */

public:
    virtual ~IAudioDevices() = default;

    /**
     * \brief Call SetActiveDevice() with this device to use the operating system's default device.
     * \remarks Name and ID show the current values of the underlying device this virtual device is tracking.
     */
    virtual const IAudioDevice &SystemDevice() const = 0;

    /**
     * \brief Call SetActiveDevice() with this device to use the operating system's default communication device.
     * \remarks Name and ID show the current values of the underlying device this virtual device is tracking.
     * Note: This always points to the same physical device as SystemDevice, except on Windows, Android, and Xbox platforms, where it can differ.
     */
    virtual const IAudioDevice &CommunicationDevice() const = 0;

    /**
     * \brief Pass this device to AudioInputDevices().SetActiveDevice() or AudioOutputDevices().SetActiveDevice()
     * to prevent Vivox from either capturing audio or rendering audio, respectively.
     * \remarks Name and ID show "No Device".
     */
    virtual const IAudioDevice &NullDevice() const = 0;

    /**
     * \brief The available devices on this system. Lists both "virtual" and physical devices.
     * \remarks Key equals the IAudioDevice Value's Id(). Virtual devices appear on the available device list
     * with their virtual Name and ID, for example: "Default System Device". The IAudioDevice reference
     * returned by SystemDevice() and similar methods show what the effective device would be upon setting
     * it active. To set a virtual device as active, you can use either IAudioDevice with SetActiveDevice().
     */
    virtual const TMap<FString, IAudioDevice *> &AvailableDevices() const = 0;

    /**
     * \brief This event is raised after a device has been added to the AvailableDevices() collection.
     */
    AfterAvailableDeviceAdded EventAfterDeviceAvailableAdded;

    /**
     * \brief This event is raised before a device is removed from the AvailableDevices() collection.
     */
    BeforeAvailableDeviceRemoved EventBeforeAvailableDeviceRemoved;

    /**
     * \brief The delegate that is called when SetActiveDevice completes. FString equals new ActiveDevice().Id().
     */
    typedef FDelegateSetActiveDeviceCompleted::FDelegate FOnSetActiveDeviceCompletedDelegate;

    /**
     * \brief Call this to set the active audio device. Note that this takes effect immediately for all open sessions.
     * \param device The device to set as active.
     * \param theDelegate A delegate to call when this operation completes.
     * \return VxErrorInvalidArgument if device is empty.
     */
    virtual VivoxCoreError SetActiveDevice(const IAudioDevice &device, const FOnSetActiveDeviceCompletedDelegate& theDelegate = FOnSetActiveDeviceCompletedDelegate()) = 0;

    /**
     * \brief The virtual or physical audio device selected for active use if available. See EffectiveDevice() for the actual device currently in use.
     */
    virtual const IAudioDevice &ActiveDevice() = 0;

    /**
     * \brief The effective audio device. If the active device is set to SystemDevice or CommunicationDevice, then the effective device shows the actual device used.
     * If the active device is set to NullDevice or a physical device no longer connected to the system, then no device is effectively in use, and NullDevice is returned.
     * If the active device is set to a specific physical device which is still connected to the system, then that device will be returned.
     */
    virtual const IAudioDevice &EffectiveDevice() = 0;

    /**
    * \brief This event fires when the effective device changes.
    * Use this to inform users that the device in use has changed.
    */
    EffectiveDeviceChanged EventEffectiveDeviceChanged;

    /**
     * \brief The audio gain for the device.
     * This is a value between -50 and 50. Positive values make the audio louder, and negative values make the audio quieter.
     * 0 leaves the value unchanged (this is the default value).
     * Note: This applies to all active audio sessions.
     */
    virtual int VolumeAdjustment() = 0;

    /**
    * \brief Set the audio gain for the device.
    * This is a value between -50 and 50. Positive values make the audio louder, and negative values make the audio quieter.
    * 0 leaves the value unchanged (this is the default value).
    * Note: This applies to all active audio sessions.
    * \param value The audio gain value to set.
    * \return VxErrorInvalidArgument if the value is outside of -50 -> 50
    */
    virtual VivoxCoreError SetVolumeAdjustment(int value) = 0;

    /**
     * \brief Indicates if audio is muted for this device.
     */
    virtual bool Muted() const = 0;
    /**
     * \brief Set whether audio is muted for this device.
     *
     * Set to true to stop the audio device from capturing or rendering audio.
     * The default is false.
     */
    virtual void SetMuted(bool value) = 0;
    /**
    * \brief Refresh the list of available devices.
    *
    * \remarks Manual use of this method should be rare, because the device lists automatically refresh
    * when Vivox initializes and when the system raises an automatic device hotswap event.
    * An automatic or manual call must complete before IAudioDevices methods such as
    * ActiveDevice, EffectiveDevice, or AvailableDevices return valid values.
    * Note: It can take up to 200 milliseconds before the list of devices refreshes.
    */
    virtual void Refresh() = 0;
};
