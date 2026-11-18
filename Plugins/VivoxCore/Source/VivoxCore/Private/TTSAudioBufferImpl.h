/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "ITTSAudioBuffer.h"
#include "ClientImpl.h"

typedef vx_tts_utterance_t TTSUtteranceStruct;
typedef vx_tts_utterance_impl_t TTSUtteranceImpl;

/**
 *
 */
class TTSAudioBufferImpl : public ITTSAudioBuffer
{
    TTSManagerId _managerId;
    TTSUtteranceStruct *_utteranceStruct;

public:
    TTSAudioBufferImpl();
    TTSAudioBufferImpl(const TTSManagerId &managerId, TTSUtteranceStruct *utterance);
    ~TTSAudioBufferImpl();

    const short *SpeechBuffer() const override;
    const int &SampleRate() const override;
    const int &NumFrames() const override;
    const int &NumChannels() const override;
    bool IsEmpty() const override;
};
