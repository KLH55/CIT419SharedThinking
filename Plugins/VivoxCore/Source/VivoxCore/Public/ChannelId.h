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
#include "Channel3DProperties.h"

/**
 * \brief The type of channel.
 */
UENUM(BlueprintType)
enum class ChannelType : uint8
{
    /**
     * \brief A typical conferencing channel.
     */
    NonPositional,
    /**
     * \brief A conferencing channel where user voices are rendered with 3D positional effects.
     */
    Positional,
    /**
     * \brief A conferencing channel where the user's text and audio is echoed back to that user.
     */
    Echo
};

/**
 * \brief The unique identifier for a channel. Channels are created and destroyed automatically on demand.
 */
class VIVOXCORE_API ChannelId
{
    FString _domain;
    FString _name;
    FString _issuer;
    FString _unityEnvironmentId;
    ChannelType _type;
    bool _largeTextChannel = false;
    Channel3DProperties _properties;
public:
    /**
     * \brief Internal use only.
     */
    static ChannelId CreateFromUri(const FString &uri, const TOptional<FString>& unityEnvironmentId = TOptional<FString>());

    /**
     * \brief Constructor
     */
    ChannelId();
    /**
     * \brief Constructor
     * \param issuer The issuer that is responsible for authenticating this channel.
     * \param name The name you assigned to the channel.
     * \param domain The Vivox domain that provides service for this channel, for example: mt1s.vivox.com.
     * \param type The type of channel, each with built-in characteristics (for example, 3D positional effects).
     * \param properties The 3D properties of the channel. Note: This is optional, and is only used for positional channels.
     * \param unityEnvironmentId The Unity Environment Id, from the Unity Dashboard, to be embedded in the Accounts URI, primarily for Moderation requests
     * \param largeTextChannel When true, the SDK will mark the channel as a "large text" channel when joining. 
              Large text channels allow a significantly higher number of text participants (for example, up to ~2000 users
              instead of the default ~200) and require server-side support and entitlement. This is an enterprise-only
              feature; to enable it for your project, contact Vivox customer support or your Vivox representative.

              If enabled, the SDK will ensure the channel name used for the join includes the special "(t-largetext)"
              suffix (appending it if it is not already present). The SDK surface will continue to expose the
              human-friendly channel name (the suffix is removed for display/lookup); the large text designation is
              handled internally by the SDK and the Vivox service.
     * \remarks Name must not exceed 200 characters, and can only use the letters A-Z and a-z, the numbers 0-9, and the special characters =+-_.!~()%
     */
    ChannelId(const FString& issuer, const FString& name, const FString& domain, ChannelType type = ChannelType::NonPositional, Channel3DProperties properties = Channel3DProperties(), const TOptional<FString>& unityEnvironmentId = TOptional<FString>(), bool largeTextChannel = false);
    
    /**
     * \brief Destructor
     */
    ~ChannelId();

    /**
     * \brief The issuer that is responsible for authenticating this channel.
     */
    const FString &Issuer() const;
    /**
     * \brief The name you assigned to the channel.
     */
    const FString &Name() const;
    /**
     * \brief The Vivox domain that provides service for this channel.
     */
    const FString &Domain() const;
    /**
     * \brief The type of channel.
     */
    ChannelType Type() const;
    /**
     * \brief The 3D properties of the channel.
     */
    Channel3DProperties Properties() const;
    /**
     * \brief whether or not the channel is designated to handle large amounts of users in text
     */
    const bool &LargeTextChannel() const;
    /**
     * \brief The Unity Environment of the UGS Project
     */
    const FString& UnityEnvironmentId() const;
    /**
     * \brief True if Issuer, Name, and Domain are all empty, and Type is NonPositional.
     */
    bool IsEmpty() const;

    /**
     * \brief True if Issuer and Domain are non-empty, and Name and Properties meet restrictions.
     */
    bool IsValid() const;

    /**
     * \brief Returns a complete channel SIP URI formatted for Vivox Access Token generation, including the channel type, issuer, name, and other components.
     * \remarks Use this string as a token claim value representing a channel identifier instead of hand-crafting one.

     */
    FString ToString() const;

    /** \brief Equality operator */
    bool operator ==(const ChannelId &RHS) const;
    /** \brief InEquality operator */
    bool operator !=(const ChannelId &RHS) const { return !operator ==(RHS); }

    /**
     * \brief True if the ID is null or empty.
     */
    static bool IsNullOrEmpty(ChannelId *id);
};

/**
 * \brief A standard Unreal Engine hash function that allows ChannelId to be used as a key in UE collections.
 * \param id The channel ID.
 * \return The hash.
 */
uint32 VIVOXCORE_API GetTypeHash(const ChannelId &id);
