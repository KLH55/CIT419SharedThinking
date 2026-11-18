/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once

#include "CoreMinimal.h"

typedef int32 VivoxCoreError;
#define APP_UNIQUE_3_LETTERS_USER_AGENT_ID_STRING "UNR"

#ifndef VxErrorSuccess
// We need these for use in Unreal applications, but we cannot include VxcErrors.h
// without pulling in extra code that we do not want, so we must capture specific content
// without causing macro redefinitions.

// Error code definitions (V4) for V5 macro compatibility only
#define VX_E_NO_MESSAGE_AVAILABLE                           -1
#define VX_E_SUCCESS                                        0
#define VX_E_INVALID_XML                                    1000
#define VX_E_NO_EXIST                                       1001
#define VX_E_MAX_CONNECTOR_LIMIT_EXCEEDED                   1002
#define VX_E_MAX_SESSION_LIMIT_EXCEEDED                     1003
#define VX_E_FAILED                                         1004
#define VX_E_ALREADY_LOGGED_IN                              1005
#define VX_E_ALREADY_LOGGED_OUT                             1006  // This is not returned.
#define VX_E_NOT_LOGGED_IN                                  1007  // This is not returned.
#define VX_E_INVALID_ARGUMENT                               1008
#define VX_E_INVALID_USERNAME_OR_PASSWORD                   1009
#define VX_E_INSUFFICIENT_PRIVILEGE                         1010
#define VX_E_NO_SUCH_SESSION                                1011
#define VX_E_NOT_INITIALIZED                                1012
#define VX_E_REQUESTCONTEXT_NOT_FOUND                       1013
#define VX_E_LOGIN_FAILED                                   1014
#define VX_E_SESSION_MAX                                    1015  // Used if already on a call.
#define VX_E_WRONG_CONNECTOR                                1016
#define VX_E_NOT_IMPL                                       1017
#define VX_E_REQUEST_CANCELLED                              1018
#define VX_E_INVALID_SESSION_STATE                          1019
#define VX_E_SESSION_CREATE_PENDING                         1020
#define VX_E_SESSION_TERMINATE_PENDING                      1021
#define VX_E_SESSION_CHANNEL_TEXT_DENIED                    1022  // Multi-party text chat is not supported.
#define VX_E_SESSION_TEXT_DENIED                            1023  // This session does not support text messaging.
#define VX_E_SESSION_MESSAGE_BUILD_FAILED                   1024  // The call to am_message_build failed for an unknown reason.
#define VX_E_SESSION_MSG_CONTENT_TYPE_FAILED                1025  // The call to osip_message_set_content_type failed for an unknown reason.
#define VX_E_SESSION_MEDIA_CONNECT_FAILED                   1026  // The media connect call failed.
#define VX_E_SESSION_MEDIA_DISCONNECT_FAILED                1026  // The media disconnect call failed.
#define VX_E_SESSION_DOES_NOT_HAVE_TEXT                     1027  // The session does not have text.
#define VX_E_SESSION_DOES_NOT_HAVE_AUDIO                    1028  // The session does not have audio.
#define VX_E_SESSION_MUST_HAVE_MEDIA                        1029  // The session must have media specified (audio or text).
#define VX_E_SESSION_IS_NOT_3D                              1030  // The session is not a SIREN14-3D codec call, so it cannot be a 3D call.
#define VX_E_SESSIONGROUP_NOT_FOUND                         1031  // The session group cannot be found.
#define VX_E_REQUEST_TYPE_NOT_SUPPORTED                     1032
#define VX_E_REQUEST_NOT_SUPPORTED                          1033
#define VX_E_MULTI_CHANNEL_DENIED                           1034
#define VX_E_MEDIA_DISCONNECT_NOT_ALLOWED                   1035
#define VX_E_PRELOGIN_INFO_NOT_RETURNED                     1036
#define VX_E_SUBSCRIPTION_NOT_FOUND                         1037  // The subscription cannot be found.
#define VX_E_INVALID_SUBSCRIPTION_RULE_TYPE                 1038
#define VX_E_INVALID_MASK                                   1040
#define VX_E_INVALID_CONNECTOR_STATE                        1041  // The operation cannot be performed because the connector is in an invalid state.
#define VX_E_BUFSIZE                                        1042  // The request operation could not return data because the supplied buffer was too small.
#define VX_E_FILE_OPEN_FAILED                               1043  // The file could not be opened because it does not exist, is locked, or due to insufficient privileges.
#define VX_E_FILE_CORRUPT                                   1044  // The file was corrupt.
#define VX_E_FILE_WRITE_FAILED                              1045  // Unable to write to the file.
#define VX_E_INVALID_FILE_OPERATION                         1046  // An invalid operation was performed on the file.
#define VX_E_NO_MORE_FRAMES                                 1047  // There are no more frames to read.
#define VX_E_UNEXPECTED_END_OF_FILE                         1048  // The file appears truncated or corrupt.
#define VX_E_FILE_WRITE_FAILED_REACHED_MAX_FILESIZE         1049  // The maximum file size has been reached. The app can close the file and continue.
#define VX_E_TERMINATESESSION_NOT_VALID                     1050
#define VX_E_MAX_PLAYBACK_SESSIONGROUPS_EXCEEDED            1051  // The maximum number of playback groups (1) has been exceeded.
#define VX_E_TEXT_DISCONNECT_NOT_ALLOWED                    1052
#define VX_E_TEXT_CONNECT_NOT_ALLOWED                       1053
#define VX_E_SESSION_TEXT_DISABLED                          1055
#define VX_E_SESSIONGROUP_TRANSMIT_NOT_ALLOWED              1056
#define VX_E_CALL_CREATION_FAILED                           1057  // The call creation failed. Check to ensure that the URI is valid.
#define VX_E_RTP_TIMEOUT                                    1058
#define VX_E_ACCOUNT_MISCONFIGURED                          1059
#define VX_E_MAXIMUM_NUMBER_OF_CALLS_EXCEEEDED              1060
#define VX_E_NO_SESSION_PORTS_AVAILABLE                     1061
#define VX_E_INVALID_MEDIA_FORMAT                           1062
#define VX_E_INVALID_CODEC_TYPE                             1063
#define VX_E_RENDER_DEVICE_DOES_NOT_EXIST                   1064
#define VX_E_RENDER_CONTEXT_DOES_NOT_EXIST                  1065
#define VX_E_RENDER_SOURCE_DOES_NOT_EXIST                   1067
#define VX_E_RECORDING_ALREADY_ACTIVE                       1068
#define VX_E_RECORDING_LOOP_BUFFER_EMPTY                    1069
#define VX_E_STREAM_READ_FAILED                             1070
#define VX_E_INVALID_SDK_HANDLE                             1071
#define VX_E_FAILED_TO_CONNECT_TO_VOICE_SERVICE             1072
#define VX_E_FAILED_TO_SEND_REQUEST_TO_VOICE_SERVICE        1073
#define VX_E_MAX_LOGINS_PER_USER_EXCEEDED                   1074
#define VX_E_MAX_HTTP_DATA_RESPONSE_SIZE_EXCEEDED           1075
#define VX_E_CHANNEL_URI_REQUIRED                           1076
#define VX_E_INVALID_CAPTURE_DEVICE_FOR_REQUESTED_OPERATION 1077
#define VX_E_LOOP_MODE_RECORDING_NOT_ENABLED                1078
#define VX_E_TEXT_DISABLED                                  1079
#define VX_E_VOICE_FONT_NOT_FOUND                           1080
#define VX_E_CROSS_DOMAIN_LOGINS_DISABLED                   1081
#define VX_E_INVALID_AUTH_TOKEN                             1082
#define VX_E_INVALID_APP_TOKEN                              1083
#define VX_E_CAPACITY_EXCEEDED                              1084
#define VX_E_ALREADY_INITIALIZED                            1085
#define VX_E_NOT_UNINITIALIZED_YET                          1086
#define VX_E_NETWORK_ADDRESS_CHANGE                         1087
#define VX_E_NETWORK_DOWN                                   1088
#define VX_E_POWER_STATE_CHANGE                             1089
#define VX_E_HANDLE_ALREADY_TAKEN                           1090
#define VX_E_HANDLE_IS_RESERVED                             1091
#define VX_E_XNETCONNECT_FAILED                             1093
#define VX_E_REQUEST_CANCELED                               1094
#define VX_E_CALL_TERMINATED_NO_RTP_RXED                    1095
#define VX_E_CALL_TERMINATED_NO_ANSWER_LOCAL                1096
#define VX_E_CHANNEL_URI_TOO_LONG                           1097
#define VX_E_CALL_TERMINATED_BAN                            1098
#define VX_E_CALL_TERMINATED_KICK                           1099
#define VX_E_CALL_TERMINATED_BY_SERVER                      1100
#define VX_E_ALREADY_EXIST                                  1101
#define VX_E_FEATURE_DISABLED                               1102
#define VX_E_SIZE_LIMIT_EXCEEDED                            1103
#define VX_E_RTP_SESSION_SOCKET_ERROR                       1104
#define VX_E_SIP_BACKEND_REQUIRED                           1105
#define VX_E_DEPRECATED                                     1106

#define VX_E_NO_RENDER_DEVICES_FOUND            7001
#define VX_E_NO_CAPTURE_DEVICES_FOUND           7002
#define VX_E_INVALID_RENDER_DEVICE_SPECIFIER    7003
#define VX_E_RENDER_DEVICE_IN_USE               7004
#define VX_E_INVALID_CAPTURE_DEVICE_SPECIFIER   7005
#define VX_E_CAPTURE_DEVICE_IN_USE              7006
#define VX_E_UNABLE_TO_OPEN_CAPTURE_DEVICE      7009

#define VX_E_FAILED_TO_CONNECT_TO_SERVER    10007

#define VX_E_ACCESSTOKEN_ALREADY_USED           20120
#define VX_E_ACCESSTOKEN_EXPIRED                20121
#define VX_E_ACCESSTOKEN_INVALID_SIGNATURE      20122
#define VX_E_ACCESSTOKEN_CLAIMS_MISMATCH        20123
#define VX_E_ACCESSTOKEN_MALFORMED              20124
#define VX_E_ACCESSTOKEN_INTERNAL_ERROR         20125
#define VX_E_ACCESSTOKEN_SERVICE_UNAVAILABLE    20127
#define VX_E_ACCESSTOKEN_ISSUER_MISMATCH        20128

// Error code definitions (V5)
#define VxErrorNoMessageAvailable VX_E_NO_MESSAGE_AVAILABLE  // Old value is -1 (not changed)
#define VxErrorSuccess VX_E_SUCCESS  // Old value is 0 (not changed)
#define VxErrorAsyncOperationCanceled 5001
#define VxErrorCaptureDeviceInUse 5002
#define VxErrorConnectionTerminated 5003
#define VxErrorFileOpenFailed VX_E_FILE_OPEN_FAILED  // Old value is 5004
#define VxErrorHandleReserved 5005
#define VxErrorHandleTaken 5006
#define VxErrorInternalError VX_E_FAILED  // Old value is 5007
#define VxErrorInvalidArgument VX_E_INVALID_ARGUMENT  // Old value is 5008
#define VxErrorInvalidFormat 5009
#define VxErrorInvalidOperation 5010
#define VxErrorInvalidState VX_E_INVALID_SESSION_STATE  // Old value is 5011
#define VxErrorInvalidValueTypeXmlQuery 5012
#define VxErrorNoMatchingXmlAttributeFound 5013
#define VxErrorNoMatchingXmlNodeFound 5014
#define VxErrorNoMemory 5015
#define VxErrorNoMoreData 5016
#define VxErrorNoXLSPConfigured 5017
#define VxErrorNotSupported 5018
#define VxErrorPortNotAvailable 5019
#define VxErrorRtpTimeout VX_E_RTP_TIMEOUT  // Old value is 5020
#define VxErrorUnableToOpenCaptureDevice 5021
#define VxErrorXLSPConnectFailed 5022
#define VxErrorXmppBackEndRequired 5023
#define VxErrorPreloginDownloadFailed 5024
#define VxErrorNotLoggedIn 5025
#define VxErrorPresenceMustBeEnabled 5026
#define VxErrorConnectorLimitExceeded 5027
#define VxErrorTargetObjectNotRelated 5028
#define VxErrorTargetObjectDoesNotExist VX_E_NO_EXIST  // Old value is 5029
#define VxErrorMaxLoginsPerUserExceeded 5030
#define VxErrorRequestCanceled 5031
#define VxErrorBuddyDoesNotExist 5032
#define VxErrorChannelUriRequired 5033
#define VxErrorTargetObjectAlreadyExists 5034
#define VxErrorInvalidCaptureDeviceForRequestedOperation 5035
#define VxErrorInvalidCaptureDeviceSpecifier 5036
#define VxErrorInvalidRenderDeviceSpecifier 5037
#define VxErrorDeviceLimitReached 5038
#define VxErrorInvalidEventType 5039
#define VxErrorNotInitialized VX_E_NOT_INITIALIZED  // Old value is 5040
#define VxErrorAlreadyInitialized VX_E_ALREADY_INITIALIZED  // Old value is 5041
#define VxErrorNotImplemented VX_E_NOT_IMPL  // Old value is 5042
#define VxErrorTimeout 5043
#define VxNoAuthenticationStanzaReceived 5044
#define VxFailedToConnectToXmppServer 5045
#define VxSSLNegotiationToXmppServerFailed 5046
#define VxErrorUserOffLineOrDoesNotExist 5047
#define VxErrorCaptureDeviceInvalidated 5048
#define VxErrorMaxEtherChannelLimitReached 5049
#define VxErrorHostUnknown 5050
#define VxErrorChannelUriTooLong 5051
#define VxErrorUserUriTooLong 5052
#define VxErrorInvalidChannelUri 5053
#define VxErrorCrossDomainLoginDisabled 5054
#define VxErrorSipRegistrationAuthorizationFailure 5055
#define VxErrorUserAlreadyLoggingOut 5056
#define VxErrorBuddyGroupDoesNotExist 5057
#define VxErrorPowerEvent 5058
#define VxErrorNetworkAddressChanged 5059
#define VxErrorNetworkDown 5060
#define VxErrorNotUninitializedYet VX_E_NOT_UNINITIALIZED_YET  // Old value is 5075
#define VxErrorCallTerminatedBanned 5098
#define VxErrorCallTerminatedKick 5099
#define VxErrorCallTerminatedByServer 5100
#define VxErrorServerRtpTimeout VX_E_CALL_TERMINATED_NO_RTP_RXED  // Old value is 5101
#define VxErrorDeprecated VX_E_DEPRECATED
#define VxErrorUserBlocked 5102
#define VxErrorMessageTextTooLong 5103
#define VxErrorNoMicrophonePermission 5104
#define VxErrorMessageTextRateExceeded 5105
#define VxErrorSessionDoesNotHaveAudio VX_E_SESSION_DOES_NOT_HAVE_AUDIO
#define VxSafeVoiceConsentUnauthorized 5106
#define VxSafeVoiceConsentInvalidProjectOrEnvironment 5107
#define VxSafeVoiceInternalError 5108
#define VxSafeVoiceConsentNotSet 5109
#define VxSafeVoiceUnknownError 5110

// 20xxx-20xxx - Reserved for 3-digit XMPP error codes returned from server
#define VxXmppErrorCodesRangeStart 20000
#define VxUnknownXmppError 20000
#define VxAccessTokenAlreadyUsed 20120
#define VxAccessTokenExpired 20121
#define VxAccessTokenInvalidSignature 20122
#define VxAccessTokenClaimsMismatch 20123
#define VxAccessTokenMalformed 20124
#define VxAccessTokenInternalError 20125
#define VxAccessTokenServiceUnavailable 20127
#define VxAccessTokenIssuerMismatch 20128
#define VxErrorTitleDisabled 20129
#define VxXmppErrorBadRequest 20400
#define VxXmppErrorNotAuthorized 20401
#define VxXmppErrorItemNotFound 20404
#define VxXmppErrorNotAllowed 20405
#define VxXmppInternalServerError 20500
#define VxXmppErrorFeatureNotImplemented 20501
#define VxXmppServerErrorServiceUnavailable 20502
#define VxXmppErrorServiceUnavailable 20503
#define VxXmppErrorChannelAtCapacity 20507
#define VxXmppErrorCodesRangeEnd 20999

// 21000+ - Internal XMPP error codes
#define VxErrorXmppServerResponseMalformed 21000
#define VxErrorXmppServerResponseBadSdp 21001
#define VxErrorXmppServerResponseInviteMissing 21002
#define VxErrorXmppServerResponseChanAddMissing 21003

#define VxNetworkSocketErrorUnknown 10000
#define VxNetworkNameResolutionFailed 10006
#define VxNetworkUnableToConnectToServer 10007
#define VxNetworkHttpTimeout 10028
#define VxNetworkHttpInvalidUrl 10003
#define VxNetworkHttpInvalidCertificate 10077
#define VxNetworkHttpConnectionReset 10056
#define VxNetworkHttpInvalidServerResponse 10052
#define VxNetworkHttpGeneralConnectionFailure 10100
#define VxNetworkReconnectFailure 10101

// Compatibility
#define VX_E_XMPP_BACKEND_REQUIRED VxErrorXmppBackEndRequired
#define VxErrorSipBackEndRequired VX_E_SIP_BACKEND_REQUIRED

// Text-to-speech (TTS)
#define VxErrorTTSInvalidEngineType tts_error_invalid_engine_type
#define VxErrorTTSEngineAllocationFailed tts_error_engine_allocation_failed
#define VxErrorTTSNotSupported tts_error_not_supported
#define VxErrorTTSMaxCharactersExceeded tts_error_max_characters_exceeded
#define VxErrorTTSUtteranceBelowMinDuration tts_error_utterance_below_min_duration
#define VxStatusTTSInputTextWasEnqueued tts_status_input_text_was_enqueued
#define VxErrorTTSSdkNotInitialized tts_error_sdk_not_initialized
#define VxErrorTTSDestinationQueueIsFull tts_error_destination_queue_is_full
#define VxStatusTTSEnqueueNotNecessary tts_status_enqueue_not_necessary
#define VxErrorTTSUtteranceNotFound tts_error_utterance_not_found
#define VxErrorTTSManagerNotFound tts_error_manager_not_found
#define VxErrorTTSInvalidArgument tts_error_invalid_argument
#define VxErrorTTSInternal tts_error_internal

#endif

#include "VivoxConfig.h"
#include "AccountId.h"
#include "ChannelId.h"
#include "Presence.h"
#include "ConnectionState.h"
#include "Channel3DProperties.h"
class AccountId;
class ChannelId;
class IAudioDevice;
class IAudioDevices;
class IChannelSession;
class IChannelConnectionState;
class IChannelTextMessage;
class ITranscribedMessage;
class Channel3DProperties;
class IDirectedTextMessage;
class IClient;
class ILoginSession;
class IParticipant;
class IParticipantProperties;
class IPresenceLocation;
class IPresenceSubscription;
class ITextMessage;
class ITTSAudioBuffer;
class ITTSMessage;
class ITTSVoice;
class Presence;
