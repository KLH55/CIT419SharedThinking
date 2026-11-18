/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "ChannelTextMessage.h"
#include "VivoxCore.h"
#include "VxcEvents.h"

ChannelTextMessage::ChannelTextMessage(::ChannelSession &parent, const vx_evt_message &evt) : _channelSession(parent)
{
    _receivedTime = FDateTime::UtcNow();
    _applicationStanzaBody = UTF8_TO_TCHAR(evt.application_stanza_body);
    _applicationStanzaNamespace = UTF8_TO_TCHAR(evt.application_stanza_namespace);
    _language = UTF8_TO_TCHAR(evt.language);
    _messageId = UTF8_TO_TCHAR(evt.message_id);
    _message = UTF8_TO_TCHAR(evt.message_body);
    _sender = AccountId::CreateFromUri(evt.participant_uri, TOptional<FString>(UTF8_TO_TCHAR(evt.participant_displayname)), parent.Channel().UnityEnvironmentId().IsEmpty() ? TOptional<FString>() : parent.Channel().UnityEnvironmentId());
}

ChannelTextMessage::~ChannelTextMessage()
{
}
