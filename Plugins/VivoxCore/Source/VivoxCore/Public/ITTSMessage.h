/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "Engine/Blueprint.h"
#include "Containers/UnrealString.h"
#include "ITTSVoice.h"

/**
 * \brief A unified selection of output stream and mechanism for text-to-speech (TTS) injection.
 */
UENUM(BlueprintType)
enum class TTSDestination : uint8
{
    /**
     * \brief Do not use. This is only defined to work around a bug in UnrealBuildTool.
     */
    Default = 0,
    /**
     * \brief Immediately send to participants in connected sessions. Mixes new messages with any other ongoing messages.
     */
    RemoteTransmission = tts_dest_remote_transmission,
    /**
     * \brief Immediately play back locally on a render device (for example, speaker hardware). Mixes new messages with any other ongoing messages.
     */
    LocalPlayback = tts_dest_local_playback,
    /**
     * \brief Immediately play back locally on a render device and send to participants in connected sessions. Mixes new messages with any other ongoing messages.
     */
    RemoteTransmissionWithLocalPlayback = tts_dest_remote_transmission_with_local_playback,
    /**
     * \brief Send to participants in connected sessions, or enqueue if there is already an ongoing message playing in this destination.
     */
    QueuedRemoteTransmission = tts_dest_queued_remote_transmission,
    /**
     * \brief Play back locally on a render device (for example, speaker hardware), or enqueue if there is already an ongoing message playing in this destination.
     */
    QueuedLocalPlayback = tts_dest_queued_local_playback,
    /**
     * \brief Play back locally on a render device and send to participants in connected sessions. Enqueue if there is already an ongoing message playing in this destination.
     */
    QueuedRemoteTransmissionWithLocalPlayback = tts_dest_queued_remote_transmission_with_local_playback,
    /**
     * \brief Immediately play back locally on a render device (for example, speaker hardware). Replaces the currently playing message in this destination.
     */
    ScreenReader = tts_dest_screen_reader
};

/**
 * \brief The state of the TTSMessage.
 */
UENUM(BlueprintType)
enum class TTSMessageState : uint8
{
    /**
     * \brief The message is currently being played in its destination.
     */
    Playing,
    /**
     * \brief The message is waiting to be played in its destination.
     */
    Enqueued
};

typedef vx_tts_utterance_id TTSUtteranceId;

/**
  * \brief A spoken message in the text-to-speech subsystem.
  */
class VIVOXCORE_API ITTSMessage
{
public:
    virtual ~ITTSMessage() = default;

    /**
     * \brief The voice used to synthesize this message.
     */
    virtual const ITTSVoice &Voice() const = 0;

    /**
     * \brief The text to be synthesized into speech.
     */
    virtual const FString &Text() const = 0;

    /**
     * \brief The destination of this message.
     */
    virtual const TTSDestination &Destination() const = 0;

    /**
     * \brief An internal utterance identifier.
     */
    virtual const TTSUtteranceId &UtteranceId() const = 0;

    /**
     * \brief The Playing/Enqueued state of the message.
     */
    virtual const TTSMessageState &State() const = 0;

    /**
     * \brief True if created by using a non-default constructor and Text is not empty.
     */
    virtual bool IsValid() const = 0;
};
