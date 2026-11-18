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
 * \brief Transcribed text from a participant in a channel session.
 */
class VIVOXCORE_API ITranscribedMessage
{
public:
    /**
     * \brief Destructor
     */
    virtual ~ITranscribedMessage() = default;
    /**
     * \brief The time that the transcription was received in UTC.
     */
    virtual const FDateTime &ReceivedTime() const = 0;
    /**
     * \brief The transcribed text.
     */
    virtual const FString &Text() const = 0;
    /**
     * \brief The speaker.
     */
    virtual const AccountId &Speaker() const = 0;
    /**
     * \brief The language of the transcribed text. This conforms to BCP 47 (https://tools.ietf.org/html/bcp47).
     */
    virtual const FString &Language() const = 0;
    /**
     * \brief The ChannelSession in which this message was originally spoken.
     */
    virtual IChannelSession &ChannelSession() const = 0;
};
