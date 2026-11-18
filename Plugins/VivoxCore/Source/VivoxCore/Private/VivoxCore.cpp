/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "VivoxCore.h"
#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Features/IModularFeatures.h"
#include "Interfaces/IPluginManager.h"
#include "ClientImpl.h"
#include "VivoxCoreCommonImpl.h"
#include "VivoxNativeSdk.h"
#include "VxcErrors.h"

#if (((ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 1) || ENGINE_MAJOR_VERSION == 4) && PLATFORM_PS4)
#pragma comment (lib, "SceSha1")
#pragma comment (lib, "SceHmac")
#pragma comment (lib, "SceAudioIn_stub_weak")
#elif PLATFORM_ANDROID
#include "VxcJNI.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#elif PLATFORM_IOS
#import "PrepareForVivox.h"
#endif // PLATFORM_IOS
#define LOCTEXT_NAMESPACE "FVivoxCoreModule"

DEFINE_LOG_CATEGORY(VivoxCore)

#define CUR_CLASS_FUNC (FString(__FUNCTION__))

FVivoxCoreModule::FVivoxCoreModule() :
    _vivoxSdkHandle(nullptr),
    _voiceClient(nullptr)
{
}

void FVivoxCoreModule::StartupModule()
{
    UE_LOG(VivoxCore, Log, TEXT("%s"), *CUR_CLASS_FUNC);

#if PLATFORM_WINDOWS
    _vivoxSdkHandle = LoadALibrary(TEXT("vivoxsdk.dll"));
    if (_vivoxSdkHandle == nullptr) {
        return;
    }
#elif PLATFORM_MAC
    _vivoxSdkHandle = LoadALibrary(TEXT("libvivoxsdk.dylib"));
    if (_vivoxSdkHandle == nullptr) {
        return;
    }
#elif defined(__ORBIS__) || defined(__PROSPERO__)
    _vivoxSdkHandle = LoadALibrary(TEXT("libvivoxsdk.prx"));
    if (_vivoxSdkHandle == nullptr) {
        return;
    }
#elif PLATFORM_ANDROID

    // hopefully this is early enough; we don't have a way add into JNI_OnLoad in AndroidJNI.cpp
    vx_jni_set_java_vm(GJavaVM);

    // do not call any Vivox SDK functions before this
    if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
    {
        static jmethodID InitVivoxMethod = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_Vivox_Init", "()V", false);
        if (InitVivoxMethod == 0)
        {
            UE_LOG(VivoxCore, Warning, TEXT("Failed to find AndroidThunkJava_Vivox_Init. Vivox voice chat will not work."));
        }

        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, InitVivoxMethod);
        if (Env->ExceptionCheck())
        {
            Env->ExceptionDescribe();
            Env->ExceptionClear();
            UE_LOG(VivoxCore, Warning, TEXT("Exception encountered calling AndroidThunkJava_Vivox_Init. Vivox voice chat will not work."));
        }
    }
    else
    {
        UE_LOG(VivoxCore, Warning, TEXT("Unable to get Java environment. Vivox voice chat will not work."));
    }

#elif PLATFORM_IOS
    PrepareForVivox();
#endif //PLATFORM_WINDOWS || PLATFORM_MAC elif PLATFORM_ANDROID elif PLATFORM_IOS

    FWorldDelegates::FWorldInitializationEvent::FDelegate OnWorldCreatedDelegate = FWorldDelegates::FWorldInitializationEvent::FDelegate::CreateRaw(this, &FVivoxCoreModule::OnWorldCreated);
    FDelegateHandle OnWorldCreatedDelegateHandle = FWorldDelegates::OnPostWorldInitialization.Add(OnWorldCreatedDelegate);

    FWorldDelegates::FWorldEvent::FDelegate OnWorldDestroyedDelegate = FWorldDelegates::FWorldEvent::FDelegate::CreateRaw(this, &FVivoxCoreModule::OnWorldDestroyed);
    FDelegateHandle OnWorldDestroyedDelegateHandle = FWorldDelegates::OnPreWorldFinishDestroy.Add(OnWorldDestroyedDelegate);

    IModularFeatures::Get().RegisterModularFeature(TEXT("VivoxVoice"), this);
}

void FVivoxCoreModule::ShutdownModule()
{
    UE_LOG(VivoxCore, Log, TEXT("%s"), *CUR_CLASS_FUNC);

    IModularFeatures::Get().UnregisterModularFeature(TEXT("VivoxVoice"), this);

    FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
    FWorldDelegates::OnPreWorldFinishDestroy.RemoveAll(this);

    if (_voiceClient != nullptr) {
        _voiceClient->Uninitialize();
        delete _voiceClient;
        _voiceClient = nullptr;
    }

    // Free the dynamic library handles
#if (PLATFORM_WINDOWS || PLATFORM_MAC)
    if (_vivoxSdkHandle != nullptr) {
        FPlatformProcess::FreeDllHandle(_vivoxSdkHandle);
        _vivoxSdkHandle = nullptr;
    }
#endif // PLATFORM_WINDOWS || PLATFORM_MAC
}

#if (defined(_GAMING_XBOX) || PLATFORM_WINDOWS || PLATFORM_MAC || defined(__ORBIS__)  || defined(__PROSPERO__))
void *FVivoxCoreModule::LoadALibrary(const TCHAR *libraryName)
{
    void *LibraryHandle;
    // Get the base directory of this plugin
    FString BaseDir = IPluginManager::Get().FindPlugin("VivoxCore")->GetBaseDir();

    // Add on the relative location of the third party dll and load it
    FString LibraryPath;
#if PLATFORM_WINDOWS
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/VivoxCoreLibrary/Windows/Release/x64"));
#elif defined(__ORBIS__)
    LibraryPath = TEXT("");
#elif defined(__PROSPERO__)
    LibraryPath = TEXT("");    
#elif defined(_GAMING_XBOX)
#if defined(_GAMING_XBOX_SCARLETT)
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/VivoxCoreLibrary/Scarlett/Release"));
#else
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/VivoxCoreLibrary/XB1GDK/Release"));
#endif
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/VivoxCoreLibrary/Mac/Release"));
#endif // PLATFORM_WINDOWS elif PLATFORM_MAC

    if (!LibraryPath.IsEmpty()) {
        FPlatformProcess::PushDllDirectory(*LibraryPath);
    }
    FString LibraryFilePath = LibraryPath / libraryName;
    LibraryHandle = !LibraryFilePath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryFilePath) : nullptr;
    if (!LibraryPath.IsEmpty()) {
        FPlatformProcess::PopDllDirectory(*LibraryPath);
    }
    
    if (LibraryHandle == nullptr) {
        UE_LOG(VivoxCore, Error, TEXT("Failed to load library (%s)"), *LibraryFilePath);
    }
    else
    {
        UE_LOG(VivoxCore, Log, TEXT("Loaded Library (%s)"), *LibraryFilePath);
    }
    return LibraryHandle;
}
#endif // PLATFORM_WINDOWS || PLATFORM_MAC

bool FVivoxCoreModule::Exec(UWorld* Inworld, const TCHAR* Cmd, FOutputDevice& Ar)
{
    return false;
}

void  FVivoxCoreModule::OnWorldCreated(UWorld* World, const UWorld::InitializationValues IVS)
{

}

void  FVivoxCoreModule::OnWorldDestroyed(UWorld* World)
{
}

void FVivoxCoreModule::Tick(float DeltaTime)
{
    if (_voiceClient) {
        _voiceClient->Tick(DeltaTime);
    }
}

IClient &FVivoxCoreModule::VoiceClient() const
{
    if (_voiceClient == nullptr) {
        UE_LOG(VivoxCore, Log, TEXT("Created VoiceClient"));
        _voiceClient = new ClientImpl();
    }
    return *_voiceClient;
}

AccountId FVivoxCoreModule::GetRandomAccountId(const FString &issuer, const FString &domain, const FString &prefix, const FString& display, const FString& environmentId)
{
    return VivoxNativeSdk::GetRandomAccountId(issuer, domain, prefix, display, environmentId);
}

ChannelId FVivoxCoreModule::GetRandomChannelId(const FString &issuer, const FString &domain, const FString &prefix, ChannelType type)
{
    return VivoxNativeSdk::GetRandomChannelId(issuer, domain, prefix, type);
}

const char *FVivoxCoreModule::ErrorToString(VivoxCoreError error)
{
    if (error <= tts_error_invalid_engine_type)
        return vx_get_tts_status_string((vx_tts_status)error);
    else
        return vx_get_error_string(error);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVivoxCoreModule, VivoxCore)
