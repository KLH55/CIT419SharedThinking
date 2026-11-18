/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "TTSVoiceImpl.h"
#include "VivoxCore.h"

TTSVoiceImpl::TTSVoiceImpl()
{
    _isValid = false;
}

TTSVoiceImpl::TTSVoiceImpl(const TTSVoiceStruct &voiceStruct)
{
    _name = voiceStruct.name;
    _voiceId = voiceStruct.voice_id;
    _isValid = true;
}

TTSVoiceImpl::TTSVoiceImpl(const ITTSVoice &ttsVoice)
{
    _name = ttsVoice.Name();
    _voiceId = ttsVoice.VoiceId();
    _isValid = !_name.IsEmpty();
}

TTSVoiceImpl::~TTSVoiceImpl()
{
}

const FString &TTSVoiceImpl::Name() const
{
    return _name;
}

const TTSVoiceId &TTSVoiceImpl::VoiceId() const
{
    return _voiceId;
}

bool TTSVoiceImpl::IsValid() const
{
    return _isValid;
}

TTSVoiceImpl &TTSVoiceImpl::operator =(const ITTSVoice &ttsVoice)
{
    _name = ttsVoice.Name();
    _voiceId = ttsVoice.VoiceId();
    _isValid = !_name.IsEmpty();
    return *this;
}
