/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once

#include "CoreMinimal.h"

/**
  * \brief Text-to-speech that is synthesized into an audio buffer for direct use.
  */
class VIVOXCORE_API ITTSAudioBuffer
{
public:
    virtual ~ITTSAudioBuffer() = default;

    /**
     * \brief The PCM buffer of the synthesized speech.
     */
    virtual const short *SpeechBuffer() const = 0;

    /**
     * \brief The sample rate of audio in the buffer.
     */
    virtual const int &SampleRate() const = 0;

    /**
     * \brief The number of frames of audio in the buffer.
     */
    virtual const int &NumFrames() const = 0;

    /**
     * \brief The number of channels that the audio in the buffer consists of.
     */
    virtual const int &NumChannels() const = 0;

    /**
     * \brief True if the speech buffer is not empty.
     */
    virtual bool IsEmpty() const = 0;
};
