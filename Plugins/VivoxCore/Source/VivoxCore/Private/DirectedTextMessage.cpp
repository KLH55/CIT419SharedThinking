/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "DirectedTextMessage.h"
#include "VivoxCore.h"

DirectedTextMessage::DirectedTextMessage(::LoginSession &loginSession, const vx_evt_user_to_user_message &evt) : _loginSession(loginSession)
{
    _receivedTime = FDateTime::UtcNow();
    _messageId = UTF8_TO_TCHAR(evt.message_id);
    _message = UTF8_TO_TCHAR(evt.message_body);
    _applicationStanzaBody = UTF8_TO_TCHAR(evt.application_stanza_body);
    _applicationStanzaNamespace = UTF8_TO_TCHAR(evt.application_stanza_namespace);
    _language = UTF8_TO_TCHAR(evt.language);
    _sender = AccountId::CreateFromUri(evt.from_uri, TOptional<FString>(UTF8_TO_TCHAR(evt.from_displayname)), loginSession.LoginSessionId().UnityEnvironmentId().IsEmpty() ? TOptional<FString>() : loginSession.LoginSessionId().UnityEnvironmentId());
}

DirectedTextMessage::~DirectedTextMessage()
{
}
