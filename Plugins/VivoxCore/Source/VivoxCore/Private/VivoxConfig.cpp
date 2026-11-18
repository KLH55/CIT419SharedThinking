/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "VivoxConfig.h"
#include "VivoxCoreCommonImpl.h"
#include "VxcErrors.h"
#include "Runtime/Launch/Resources/Version.h"

VivoxConfig::VivoxConfig()
{
    int status = vx_get_default_config3(&config, sizeof(config));
    ensure(status == 0);
    if (status != 0) return;
    config.pf_logging_callback = LoggingCallback;
    config.pf_malloc_func = &VivoxMalloc;
    config.pf_realloc_func = &VivoxRealloc;
    config.pf_calloc_func = &VivoxCalloc;
    config.pf_malloc_aligned_func = &VivoxMallocAligned;
    config.pf_free_func = &VivoxFree;
    config.pf_free_aligned_func = &VivoxFreeAligned;
    config.initial_log_level = vx_log_level::log_error;
    // Core 5.29.0 echoes directed messages back to their sender by default. IDirectedTextMessage can't tell an
    // echo from a received message, so it would raise the player's own messages as received from themselves.
    config.enable_dm_send_confirmation = 0;
#ifdef APP_UNIQUE_3_LETTERS_USER_AGENT_ID_STRING
    memcpy(config.app_id, APP_UNIQUE_3_LETTERS_USER_AGENT_ID_STRING, sizeof(config.app_id));
#endif
}

void VivoxConfig::SetLogLevel(vx_log_level logLevel)
{
    config.initial_log_level = logLevel;
}

media_codec_type VivoxConfig::DefaultCodecsMask() const {
    vx_codec vxCodec = vx_codec(config.default_codecs_mask);
    switch (vxCodec)
    {
VX_DEPRECATED_IGNORE_BEGIN()
    case vx_codec_pcmu:
        return media_codec_type_pcmu;
VX_DEPRECATED_IGNORE_END()
    case vx_codec_siren7:
        return media_codec_type_siren7;
    case vx_codec_siren14:
        return media_codec_type_siren14;
    case vx_codec_opus40:
    case vx_codec_opus57:
    case vx_codec_opus72:
    default:
        return media_codec_type_opus;
    }
}

void VivoxConfig::SetDefaultCodecsMask(media_codec_type codecType)
{
    switch (codecType)
    {
VX_DEPRECATED_IGNORE_BEGIN()
    case media_codec_type_pcmu:
        config.default_codecs_mask = vx_codec_pcmu;
        break;
VX_DEPRECATED_IGNORE_END()
    case media_codec_type_siren7:
        config.default_codecs_mask = vx_codec_siren7;
        break;
    case media_codec_type_siren14:
        config.default_codecs_mask = vx_codec_siren14;
        break;
    case media_codec_type_opus:
    default:
        config.default_codecs_mask = vx_codec_opus40;
        break;
    }
}

void VivoxConfig::SetRenderSourceQueueDepthMax(int queueDepthMax)
{
    config.render_source_queue_depth_max = queueDepthMax;
}

void VivoxConfig::SetRenderSourceInitialBufferCount(int initialBufferCount)
{
    config.render_source_initial_buffer_count = initialBufferCount;
}

void VivoxConfig::SetUpstreamJitterFrameCount(int upstreamJitterFrameCount)
{
    config.upstream_jitter_frame_count = upstreamJitterFrameCount;
}

void VivoxConfig::SetMaxLoginsPerUser(int maxLoginsPerUser)
{
    config.max_logins_per_user = maxLoginsPerUser;
}

void VivoxConfig::SetCaptureDeviceBufferSizeIntervals(int captureDeviceBufferSizeIntervals)
{
    config.capture_device_buffer_size_intervals = captureDeviceBufferSizeIntervals;
}

void VivoxConfig::SetRenderDeviceBufferSizeIntervals(int renderDeviceBufferSizeIntervals)
{
    config.render_device_buffer_size_intervals = renderDeviceBufferSizeIntervals;
}

void VivoxConfig::SetNeverRtpTimeoutMs(int neverRtpTimeoutMs)
{
    config.never_rtp_timeout_ms = neverRtpTimeoutMs;
}

void VivoxConfig::SetLostRtpTimeoutMs(int lostRtpTimeoutMs)
{
    config.lost_rtp_timeout_ms = lostRtpTimeoutMs;
}

void VivoxConfig::SetProcessorAffinityMask(long long processorAffinityMask)
{
    config.processor_affinity_mask = processorAffinityMask;
}

void VivoxConfig::SetDisableDevicePolling(bool disableDevicePolling)
{
    config.disable_device_polling = disableDevicePolling;
}

void VivoxConfig::SetForceCaptureSilence(bool forceCaptureSilence)
{
    config.force_capture_silence = forceCaptureSilence;
}

void VivoxConfig::SetEnableAdvancedAutoLevels(bool enableAdvancedAutoLevels)
{
    config.enable_advanced_auto_levels = enableAdvancedAutoLevels;
}

void VivoxConfig::SetEnablePersistentHTTP(bool enablePersistentHTTP)
{
    config.enable_persistent_http = enablePersistentHTTP;
}

void VivoxConfig::SetDisableAudioDucking(bool disableAudioDucking)
{
    config.disable_audio_ducking = disableAudioDucking;
}

void VivoxConfig::SetEnableFastNetworkChangeDetection(bool enableFastNetworkChangeDetection)
{
    config.enable_fast_network_change_detection = enableFastNetworkChangeDetection;
}

void VivoxConfig::SetUseOSProxySettings(bool useOSProxySettings)
{
    config.use_os_proxy_settings = useOSProxySettings;
}

void VivoxConfig::SetAllowSharedCaptureDevices(bool allowSharedCaptureDevices)
{
    config.allow_shared_capture_devices = allowSharedCaptureDevices;
}

void VivoxConfig::SetEnableDTX(bool enableDTX)
{
    config.enable_dtx = enableDTX;
}

void VivoxConfig::LoggingCallback(void *callback_handle, vx_log_level logLevel, const char *source, const char *message)
{
    switch (logLevel)
    {
    case vx_log_level::log_error:
        UE_LOG(VivoxCore, Error, TEXT("%s: %s, %s"), *FString(vx_get_log_level_string(logLevel)), *FString(source), *FString(message));
        break;
    case vx_log_level::log_warning:
        UE_LOG(VivoxCore, Warning, TEXT("%s: %s, %s"), *FString(vx_get_log_level_string(logLevel)), *FString(source), *FString(message));
        break;
    case vx_log_level::log_info:
        UE_LOG(VivoxCore, Log, TEXT("%s: %s, %s"), *FString(vx_get_log_level_string(logLevel)), *FString(source), *FString(message));
        break;
    case vx_log_level::log_debug:
        UE_LOG(VivoxCore, Verbose, TEXT("%s: %s, %s"), *FString(vx_get_log_level_string(logLevel)), *FString(source), *FString(message));
        break;
    case vx_log_level::log_trace:
        UE_LOG(VivoxCore, VeryVerbose, TEXT("%s: %s, %s"), *FString(vx_get_log_level_string(logLevel)), *FString(source), *FString(message));
        break;
    case vx_log_level::log_all:
    default:
        UE_LOG(VivoxCore, All, TEXT("%s: %s, %s"), *FString(vx_get_log_level_string(logLevel)), *FString(source), *FString(message));
        break;
    }
}

void* VivoxConfig::VivoxMalloc(size_t bytes)
{
    return FMemory::Malloc(bytes);
}

void VivoxConfig::VivoxFree(void* ptr)
{
    FMemory::Free(ptr);
}

void* VivoxConfig::VivoxRealloc(void* ptr, size_t bytes)
{
    return FMemory::Realloc(ptr, bytes);
}

void* VivoxConfig::VivoxCalloc(size_t num, size_t bytes)
{
    void *ptr = FMemory::Malloc(bytes*num);
    if (ptr)
    {
        FMemory::Memset(ptr, 0, bytes*num);
    }
    return ptr;
}

void* VivoxConfig::VivoxMallocAligned(size_t alignment, size_t bytes)
{
    return FMemory::Malloc(bytes, alignment);
}

void VivoxConfig::VivoxFreeAligned(void* ptr)
{
    FMemory::Free(ptr);
}

