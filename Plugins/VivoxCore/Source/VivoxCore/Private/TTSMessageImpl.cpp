/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "TTSMessageImpl.h"
#include "VivoxCore.h"

TTSMessageImpl::TTSMessageImpl()
{
    _isValid = false;
}

TTSMessageImpl::TTSMessageImpl(const TTSVoiceImpl &voice, const FString &text, const TTSDestination &destination, const TTSUtteranceId &utteranceId, const TTSMessageState &state)
{
    _voice = voice;
    _text = text;
    _destination = destination;
    _utteranceId = utteranceId;
    _state = state;
    _isValid = true;
}

TTSMessageImpl::~TTSMessageImpl()
{
}

const ITTSVoice &TTSMessageImpl::Voice() const
{
    return _voice;
}

const FString &TTSMessageImpl::Text() const
{
    return _text;
}

const TTSDestination &TTSMessageImpl::Destination() const
{
    return _destination;
}

const TTSUtteranceId &TTSMessageImpl::UtteranceId() const
{
    return _utteranceId;
}

const TTSMessageState &TTSMessageImpl::State() const
{
    return _state;
}

bool TTSMessageImpl::IsValid() const
{
    return _isValid && !_text.IsEmpty();
}

void TTSMessageImpl::SetPlaying()
{
    _state = TTSMessageState::Playing;
}
