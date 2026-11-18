# Changelog
All notable changes to this package will be documented in this file starting from version 5.21.0.unr.0.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

The Vivox Core SDK is the foundation for the Vivox Unreal SDK. Review changes to the Vivox Core SDK on the [Vivox Core release notes page](https://docs.unity.com/en-us/vivox-core/core-release-notes).

## [5.29.0.unr.0] - 2026-11-06
### Changed
- Upgraded Vivox Core SDK to version `5.29.0`.
- Vivox Core SDK 5.29.0 echoes directed messages back to the account that sent them by default (`enable_dm_send_confirmation`). The plugin turns this off, so `EventDirectedTextMessageReceived` still raises only directed messages received from other users, as in previous releases.

### Fixed
- The plugin's documentation, support, and homepage links (`DocsURL`, `SupportURL`, `CreatedByURL` in `VivoxCore.uplugin`) pointed to the retired vivox.com sites. They now point to the [Vivox Unreal documentation](https://docs.unity.com/en-us/vivox-unreal) and [Vivox support](https://support.unity.com/hc/en-us/sections/4407501155732-Vivox).
- Fixed a build error in Editor (modular) builds without precompiled headers, where `ITTSAudioBuffer.h` failed with `uses undefined class 'DLLEXPORT'` because it did not include any engine header.

## [5.28.0.unr.0] - 2026-09-08
### Changed
- Upgraded Vivox Core SDK to version `5.28.0`.

## [5.27.1.unr.0] - 2026-02-19
### Added
- Added support for Nintendo Switch™ 2.
- MessageID is now exposed to ChannelTextMessage and DirectedTextMessage.
### Changed
- Upgraded Vivox Core SDK to version `5.27.1`.

## [5.27.0.unr.1] - 2025-12-11
### Fixed
- Fixed a race condition where ChannelSession events would complete as the ChannelSession was destroyed.
- Fixed a bug where the Unity Environment ID was not being properly passed through to various structures.

## [5.27.0.unr.0] - 2025-11-17
### Added
- Added a LargeTextChannel option to the ChannelId constructor. With this feature enabled, text channels can support up to 2000 users.

### Fixed
- Fixed a bug where DisplayNames were not being properly passed with channels and directed messages.

## [5.26.1.unr.0] - 2025-05-02
### Fixed
- Fixed a bug where it would not be possible to join channels on Nintendo Switch™ when running in compatibility mode.

## [5.26.0.unr.0] - 2025-03-31
### Added
- Noise suppression: Now applied to capture audio by default.
- Vivox Acoustic Echo Cancellation (AEC) and Automatic Gain Control (AGC) are now available on all platforms. AEC and AGC were previously unavailable on Nintendo Switch™, PlayStation®, Xbox, visionOS, and UWP platforms.
### Changed
- The licenses and third party notices have been updated.
- Vivox Acoustic Echo Cancellation (AEC) and Automatic Gain Control (AGC) have had their underlying algorithms replaced.
    - AEC on mobile platforms no longer mutes the microphone signal when echo is detected. Instead, echo is removed and speech continues to transmit.
    - AGC is on by default. The new AGC increases microphone capture loudness on all platforms. Consider adjusting application volume presets if they were changed from the default volumes; perhaps going so far as to reset user volume settings entirely or to adjust them by some formula.
    - For platforms that already had AEC and AGC, the new implementation including the addition of Noise Suppression has:
        - The same or less CPU cost than the previous implementation when using the Release configuration.
        - A reduction in memory usage of about 11 MB while a voice session is active.
    - For platforms that are getting AEC and AGC for the first time:
        - CPU cost of the Vivox audio thread increases by about 44% when using the Release configuration.
        - An extra 6 MB of memory will be used while a voice session is active.
    - Vivox library sizes have increased for all platforms due to the change in audio processing implementations:
        - Release configuration libraries add between 53 KB and 8 MB more to an application than the previous Vivox release.
        - Debug configuration libraries can add upwards of 12 MB more to an application than the previous Vivox release.
- Network optimization: A new default voice codec bitrate was chosen to reduce packet loss and jitter. The new setting was found to be indistinguishable from the previous setting in structured listening tests.
- `iOS`:
    - The `ios_voice_processing_io_mode` value now defaults to 1 rather than 2, meaning that the VoiceProcessingIO (VPIO) unit will only be used when the speakerphone is used for render and capture. This change favors the use of the new AEC underlying algorithms rather than the use of the VPIO unit to avoid the decrease in quality for most audio device configurations that come from using the VPIO unit.
### Fixed
- Fixed a crash with iOS Shipping builds in Unreal 5.3+ caused when disconnecting from a channel.
- Fixed a crash caused by the LoginSession object being deleted while HandleEvent is running.
- Vivox log levels were off-by-one. There may be more Vivox log statements now if `VivoxConfig.SetLogLevel` was used for your application.

### Removed
- `iOS`:
    - Removed bitcode in the iOS libraries.
    - Removed support for ARMV7 architecture on iOS platforms.

## [5.25.4.unr.0] - 2024-12-18
### Changed
- The licenses and third party notices have been updated.
- `Xbox`:
    - Improved handling of hotswapping capture and render devices.
### Fixed
- A bug where trying to join multiple channels quickly would lead to an exception.
- `Xbox`:
    - Issues preventing Remote Control functioning with Vivox through Xbox Manager. See documentation for full details.
    - A bug where USB audio devices were not being detected when plugged in.

## [5.25.0.unr.0] - 2024-08-23
- `iOS`, `Android`: - Enabled Unreal Engine custom allocators for all Unreal versions except for UE4.26 and UE4.27 on mobile platforms. Mobile platforms with those versions of UE are still excluded to avoid a crash.

### Added
- `macOS`, `iOS`:
  - Added a copy step to iOS and macOS builds to copy the PrivacyInfo.xcprivacy file to the Xcode project.
- Moderation Module:
  - Added a new Module allowing for user sanctions for behaviours, powered by the Unity Moderation Service, available on the Unity Dashboard
- Vivox Safe Voice Consent:
  - Added two new API end points to LoginSession (BeginSetSafeVoiceConsentStatus and BeginGetSafeVoiceConsentStatus), allowing for Consent to be given for Vivox Safe Voice recordings if the feature is turned on on the Unity Dashboard.

### Removed
- Removed ensure() statements which could trigger when calling IAudioDevices accessor methods in some cases.

### Fixed
- Fixed issue related to LoginState not returned to LoggedOut after a failed connector_create. Leads to InvalidState error on subsequent login
    retries.
- Fixed `Error LNK1104 : cannot open file 'SceSha1.lib'` which could occur when building non-PlayStation platforms in environments where PLATFORM_PS4 is defined.

## [5.23.1.unr.0] - 2024-04-18
### Added
- `macOS`, `iOS`, `visionOS`:
    - Added the new PrivacyInfo.xcprivacy file. It is required in your Xcode project when generating your application's privacy report before submitting it to the App Store. For more details, refer to this page of the Unity documentation: <https://docs.unity.com/ugs/en-us/manual/vivox-unity/manual/Unity/privacy/apple-privacy-manifest>

### Fixed
- An issue where it would not be possible to connect to a session again after an unexpected logout.

## [5.23.0.unr.0] - 2024-03-06
### Changed
- Reduced the amount of memory used by the Automatic Connection Recovery.
- `Android`:
    - Support for the new Bluetooth Low Energy API on Android, making for smoother Bluetooth connections on newer Android versions.
    - On Meta/Oculus devices, audio is now always rendered in stereo when using the speakerphone.
    - A new way to disable mono downmixing on speakerphone by adding an Android meta-data name-value pair to your application's AndroidManifest.xml.
- `iOS`:
    - Restored bitcode in the iOS libraries.
- `Xbox`:
    - Removed delay load designation for GDK Vivox DLL.

### Fixed
- A bug where a participant would have a low volume when leaving and rejoining a channel.
- A bug where a participant transmitting to both a positional (3D) and a non-positional (2D) could only be heard in the non-positional channel.
- An issue where it would be impossible to rejoin channels after an unexpected logout occurred.
- An issue where signing in again through `ILoginSession` after a manual `Logout()` would raise no events.
- `Android`:
    - A bug on where all audio would route to the speakerphone when the Bluetooth permissions were granted after joining a channel.
- `macOS`:
    - An issue where capture sounds would be robotic on some macOS devices.
    - A problem where the dylib codesigning identity was invalid.
- `PlayStation`:
    - A deadlock that occurs when Automatic Connection Recovery is triggered.

### Known Issues
-   Vivox crashes with Live Code in Unreal 5.2.
-   Vivox Unreal Plugin might have symbol conflicts when used with other plugins.
- `Windows`:
    - Starting multiple clients rapidly may cause Windows 11 to lose its ability to resolve DNS.
- `Xbox`:
    - If the SDK is initialized before an Xbox user is logged in on the console, controller events do not come through.
    - Mismatch of GDK versions with the Vivox SDK version leads to voice chat issues.

## [5.22.0.unr.0] - 2023-12-11
### Added
- Support for Apple Silicon devices

### Changed
- `Android`:
    - Improved SPAM/DVPS behavior by using A2DP devices when failing to use the Hands-Free Profile of devices rather than routing to speakerphone.

### Fixed
- World bindings so they are properly being added on Module startup and removed on Module shutdown.
- `Android`:
    - An issue where all positional audio for the right channel could not be heard when using speakerphone.
    - An issue where Bluetooth devices reporting as Bluetooth Low Energy devices would be ignored, using the speakerphone instead.

## [5.21.2.unr.0] - 2023-11-16
### Changed
- Hidden all symbols related to C++ STL which were conflicting with some third parties, such as AWS Cognito, to avoid any conflict with other third party libraries used by our customers.
- `Android`:
    - Upgraded NDK to R25B to be aligned with Unreal Engine 5 NDK version and with other third-party plug-ins that use NDK R25B (i.e. AWS Cognito)
    - Reduced the size of the Android libraries.
- `Switch`:
    - Improved thread memory management.

## [5.21.1.unr.0] - 2023-09-13
### Changed
 - Messages from blocked participants from history_session queries are now omitted.

### Fixed
- An issue where users could hear repeat audio blips in the Core SDK.
- An issue that triggered when joining a 3D channel after a 2D channel where session groups ignored clamping_distance.
- A crash that triggered when a user attempted to send a message while Automatic Connect Recovery was in progress.
- `Android`:
    - Fixed Android Bluetooth permission issues.
    - Added the Android symbols in Android SDK package
- `iOS`:
    - Fixed a duplicate symbol Issue.

### Known Issues
- Positional audio over speaker phone plays no audio for all voice chat spoken to the right of the user on Android.

## [5.21.0.unr.0] - 2023-07-25
### Added
 -   Text Chat (Vivox) features include a new Adaptive Chat Filter that provides real-time filtering of unwanted words and phrases and the following features (now in beta). To get started, see the [Text Chat Guide](https://docs.vivox.com/v5/general/core/5_21_0/en-us/Default.htm#text-chat-guide/text-chat-guide-overview.htm), and contact Unity Support for feature enablement.
    -   **Anti-flooding** (beta) - Achieve a balanced and enjoyable chat environment by customizing message length and frequency, preventing users from sending excessively long or frequent messages.
    -   **Text evidence management** (beta) - Retrieve all versions of flagged messages through an easy-to-use Server-to-Server API that allows integration into your own moderation dashboard.
    -   **Large channel support** (beta) - Large channels allow up to 2000 users for vibrant world chats and massive multiplayer experiences.

### Changed
- `iOS`:
    - Bitcode has been removed from the iOS SDK because it has been deprecated by Apple. Users should now disable bitcode in Xcode build settings for new projects.
- `Xbox`:
    - Xbox GDK SDKs will only be distributed as DLLs from this version onwards. This will reduce the size of the library, transforming it into an import library with accompanying symbols PDB. These changes were to align with the Windows release. Users should continue to link with vivoxsdk.lib to automatically load the DLL.