/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "ITranscribedMessage.h"
#include "ChannelSession.h"

struct vx_evt_transcribed_message;
/**
 * Transcribed text from a participant in a channel session
 */
class TranscribedMessage : public ITranscribedMessage
{
    FDateTime _receivedTime;
    FString _text;
    AccountId _speaker;
    FString _language;
    ::ChannelSession &_channelSession;
public:
    TranscribedMessage(::ChannelSession &parent, const vx_evt_transcribed_message &evt);
    ~TranscribedMessage();
    const FDateTime& ReceivedTime() const override { return _receivedTime; }
    const FString& Text() const override { return _text; }
    const AccountId& Speaker() const override { return _speaker; }
    const FString& Language() const override { return _language; }
    IChannelSession& ChannelSession() const override { return _channelSession; }
};
