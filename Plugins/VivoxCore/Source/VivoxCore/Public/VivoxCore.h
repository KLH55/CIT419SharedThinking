/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "Tickable.h"
#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "VivoxCoreCommon.h"

#include "IClient.h"
#include "IAudioDevice.h"
#include "IAudioDevices.h"
#include "IChannelSession.h"
#include "IChannelConnectionState.h"
#include "IChannelTextMessage.h"
#include "ITranscribedMessage.h"
#include "IDirectedTextMessage.h"
#include "ILoginSession.h"
#include "IParticipant.h"
#include "IPresenceLocation.h"
#include "IPresenceSubscription.h"
#include "ITTSAudioBuffer.h"
#include "Presence.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Features/IModularFeature.h"

class ClientImpl;

/**
 * \brief An Unreal Engine 4 plugin for Vivox voice.
 */
class VIVOXCORE_API FVivoxCoreModule : public FTickableGameObject, public FSelfRegisteringExec, public IModuleInterface, public IModularFeature
{
public:

    /**
     * \brief Constructor
     */
    FVivoxCoreModule();

    /** \brief IModuleInterface implementation  - called on Modules startup */
    void StartupModule() override;
    /** \brief IModuleInterface implementation  - called on Module shutdon */
    void ShutdownModule() override;

    /* @cond */
    /** FTickableGameObject implementation */
    bool IsTickable() const override { return true; }
    bool IsTickableInEditor() const override { return true; }
    bool IsTickableWhenPaused() const override { return true; }

    TStatId GetStatId() const override
    {
        return TStatId();
    }

    void Tick(float DeltaTime) override;

    /* FSelfRegisteringExec implementation */
    /**
    * Exec handler
    *
    * \param	Inworld	World context
    * \param	Cmd		Command to parse
    * \param	Ar		Output device to log to
    *
    * \return	True if the command was handled, false otherwise.
    */
    virtual bool Exec(UWorld* Inworld, const TCHAR* Cmd, FOutputDevice& Ar) override;

    void OnWorldCreated(UWorld* World, const UWorld::InitializationValues IVS);
    void OnWorldDestroyed(UWorld* World);
    /* @endcond */

    /**
     * \brief The voice client.
     * \return The voice client.
     */
    IClient &VoiceClient() const;

    /**
     * \brief Get a random account identifier. For internal testing purposes only.
     */
    static AccountId GetRandomAccountId(const FString &issuer, const FString &domain, const FString &prefix, const FString& display, const FString& environmentId);
    /**
    * \brief Get a random channel identifier. For internal testing purposes only.
    */
    static ChannelId GetRandomChannelId(const FString &issuer, const FString &domain, const FString &prefix, ChannelType type);
    /**
     ** \brief Get the UTF-8 string for a specific error code in US English.
     */
    static const char *ErrorToString(VivoxCoreError error);
private:
#if (defined(_GAMING_XBOX) || PLATFORM_WINDOWS || PLATFORM_MAC || defined(__ORBIS__) || defined(__PROSPERO__))
    void *LoadALibrary(const TCHAR *libraryName);
#endif // PLATFORM_WINDOWS || PLATFORM_MAC

    /** Handle to the test dll we will load */
    void* _vivoxSdkHandle;
    mutable ClientImpl *_voiceClient;
};
