/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "Containers/UnrealString.h"

/**
  * \brief Either an audio input device (microphone) or audio output device (speaker hardware or headphones).
  */
class VIVOXCORE_API IAudioDevice
{
public:
    virtual ~IAudioDevice() = default;
    /**
     * \brief A user-displayable device name.
     */
    virtual const FString &Name() const = 0;
    /**
     * \brief An internal device identifier.
     */
    virtual const FString &Id() const = 0;
    /**
     * \brief Returns true if both the Name and ID properties are empty.
     */
    virtual bool IsEmpty() const = 0;
};
