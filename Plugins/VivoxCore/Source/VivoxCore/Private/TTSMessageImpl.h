/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "ITTSMessage.h"
#include "TTSVoiceImpl.h"

/**
 *
 */
class TTSMessageImpl : public ITTSMessage
{
    TTSVoiceImpl _voice;
    FString _text;
    TTSDestination _destination;
    TTSUtteranceId _utteranceId;
    TTSMessageState _state;
    bool _isValid;

public:
    TTSMessageImpl();
    TTSMessageImpl(const TTSVoiceImpl &voice, const FString &text, const TTSDestination &destination, const TTSUtteranceId &utteranceId, const TTSMessageState &state);
    ~TTSMessageImpl();

    const ITTSVoice &Voice() const override;
    const FString &Text() const override;
    const TTSDestination &Destination() const override;
    const TTSUtteranceId &UtteranceId() const override;
    const TTSMessageState &State() const override;
    bool IsValid() const override;

    // internal
    void SetPlaying();
};
