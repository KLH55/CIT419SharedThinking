/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once

#include "ITextToSpeech.h"
#include "VivoxCoreCommonImpl.h"
#include "TTSMessageImpl.h"

/**
 *
 */
class TextToSpeech : public ITextToSpeech
{
    ClientImpl &_client;
    TMap<FString, ITTSVoice*> _ttsAvailableVoices;
    FString _ttsSDKDefaultVoiceName;
    TTSVoiceImpl _ttsCurrentVoice;
    TMap<TTSDestination, TArray<ITTSMessage *>> _ttsMessages;
public:
    TextToSpeech(ClientImpl &client);
    ~TextToSpeech();

    const TMap<FString, ITTSVoice*> &GetAvailableVoices() override;
    const ITTSVoice &GetCurrentVoice() override;
    bool SetCurrentVoice(const ITTSVoice &newVoice) override;
    VivoxCoreError Speak(const FString &text, const TTSDestination &destination, ITTSMessage **outMessage) override;
    VivoxCoreError SpeakToBuffer(const FString &text, ITTSAudioBuffer **outBuffer) override;
    VivoxCoreError CancelMessage(const ITTSMessage &message) override;
    VivoxCoreError CancelDestination(const TTSDestination &destination) override;
    VivoxCoreError CancelAll() override;
    const TArray<ITTSMessage *> &GetMessages(const TTSDestination &destination) override;

    // Internal
    virtual void HandleEvent(const vx_evt_base_t &evt);
    ITTSMessage *GetITTSMessageFromEvt(const TTSDestination &destination, const TTSUtteranceId &utteranceId);
    void CleanupTTS();
};
