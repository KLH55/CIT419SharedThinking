/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "TranscribedMessage.h"
#include "VivoxCore.h"
#include "VxcEvents.h"

TranscribedMessage::TranscribedMessage(::ChannelSession &parent, const vx_evt_transcribed_message &evt) : _channelSession(parent)
{
    _receivedTime = FDateTime::UtcNow();
    _language = evt.language ? UTF8_TO_TCHAR(evt.language) : UTF8_TO_TCHAR("en");
    _text = UTF8_TO_TCHAR(evt.text);
    _speaker = AccountId::CreateFromUri(evt.participant_uri, TOptional<FString>(UTF8_TO_TCHAR(evt.participant_displayname)), parent.Channel().UnityEnvironmentId().IsEmpty() ? TOptional<FString>() : parent.Channel().UnityEnvironmentId());
}

TranscribedMessage::~TranscribedMessage()
{
}
