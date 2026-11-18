/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once

#include "Runtime/Launch/Resources/Version.h"

#if PLATFORM_IOS
#include <AVFoundation/AVFoundation.h>
#ifdef __cplusplus
extern "C" {
#endif

void PrepareForVivox ();

#ifdef __cplusplus
}

void PrepareForVivox() {
    // Important: must set PlayAndRecord category for simultaneous input/output
    // Default to speaker will play from speakers instead of the receiver (ear speaker) when headphones are not used.
    [AVAudioSession.sharedInstance
     setCategory:AVAudioSessionCategoryPlayback
     mode:AVAudioSessionModeDefault
     options:0
     error:nil];
    // 44,100 sample rate must be used for iOS
    [AVAudioSession.sharedInstance setPreferredSampleRate:44100 error:nil];
}
#endif
#endif
