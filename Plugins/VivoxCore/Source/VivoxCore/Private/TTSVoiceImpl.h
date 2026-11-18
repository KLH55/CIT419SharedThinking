/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "Containers/UnrealString.h"
#include "ITTSVoice.h"

typedef vx_tts_voice_t TTSVoiceStruct;

/**
 *
 */
class TTSVoiceImpl : public ITTSVoice
{
    FString _name;
    TTSVoiceId _voiceId;
    bool _isValid;

public:
    TTSVoiceImpl();
    TTSVoiceImpl(const TTSVoiceStruct &voiceStruct);
    TTSVoiceImpl(const ITTSVoice &ttsVoice);
    ~TTSVoiceImpl();

    const FString &Name() const override;
    const TTSVoiceId &VoiceId() const override;
    bool IsValid() const override;

    TTSVoiceImpl &operator =(const ITTSVoice &ttsVoice);
};
