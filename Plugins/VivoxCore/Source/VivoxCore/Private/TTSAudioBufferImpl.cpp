/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "TTSAudioBufferImpl.h"
#include "VivoxCore.h"

TTSAudioBufferImpl::TTSAudioBufferImpl()
{
    _utteranceStruct = nullptr;
}

TTSAudioBufferImpl::TTSAudioBufferImpl(const TTSManagerId &managerId, TTSUtteranceStruct *utterance)
{
    _managerId = managerId;
    _utteranceStruct = utterance;
}

TTSAudioBufferImpl::~TTSAudioBufferImpl()
{
    vx_tts_destroy_utterance(_managerId, _utteranceStruct);
}

const short *TTSAudioBufferImpl::SpeechBuffer() const
{
    return _utteranceStruct->speech_buffer;
}

const int &TTSAudioBufferImpl::SampleRate() const
{
    return _utteranceStruct->sample_rate;
}

const int &TTSAudioBufferImpl::NumFrames() const
{
    return _utteranceStruct->num_frames;
}

const int &TTSAudioBufferImpl::NumChannels() const
{
    return _utteranceStruct->num_channels;
}

bool TTSAudioBufferImpl::IsEmpty() const
{
    return !(_utteranceStruct && _utteranceStruct->num_frames > 0);
}
