/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "IDirectedTextMessage.h"
#include "LoginSession.h"

struct vx_evt_user_to_user_message;
/**
 * A text message from one user to another user
 */
class DirectedTextMessage : public IDirectedTextMessage
{
    FDateTime _receivedTime;
    FString _messageId;
    FString _message;
    AccountId _sender;
    FString _language;
    FString _applicationStanzaNamespace;
    FString _applicationStanzaBody;
    ::LoginSession &_loginSession;
public:
    DirectedTextMessage(::LoginSession &loginSession, const vx_evt_user_to_user_message &evt);
    ~DirectedTextMessage();
    const FDateTime& ReceivedTime() const override { return _receivedTime; }
    const FString& MessageId() const override { return _messageId; }
    const FString& Message() const override { return _message; }
    const AccountId& Sender() const override { return _sender; }
    const FString& Language() const override { return _language; }
    const FString& ApplicationStanzaNamespace() const override { return _applicationStanzaNamespace; }
    const FString& ApplicationStanzaBody() const override { return _applicationStanzaBody; }
    ILoginSession& LoginSession() override { return _loginSession; }
};
