/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "VivoxCoreCommon.h"

/**
 * \brief A text message.
 */
class VIVOXCORE_API ITextMessage
{
public:
    /**
     * \brief Destructor
     */
    virtual ~ITextMessage() = default;
    /**
     * \brief The time that the message was received in UTC.
     */
    virtual const FDateTime &ReceivedTime() const = 0;
    /**
     * \brief The message ID. The value of message ID will be empty unless chat history has been enabled.
     */
    virtual const FString &MessageId() const = 0;
    /**
     * \brief The message.
     */
    virtual const FString &Message() const = 0;
    /**
     * \brief The sender.
     */
    virtual const AccountId &Sender() const = 0;
    /**
     * \brief The language of the message. This conforms to BCP 47 (https://tools.ietf.org/html/bcp47).
     */
    virtual const FString &Language() const = 0;
    /**
     * \brief An optional name space for application-specific data.
     */
    virtual const FString &ApplicationStanzaNamespace() const = 0;
    /**
     * \brief Optional application-specific data.
     */
    virtual const FString &ApplicationStanzaBody() const = 0;
};
