/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "Containers/UnrealString.h"
#include "Misc/Optional.h"
/**
 * The unique identifier for a player that accesses a Vivox instance.
 */
class VIVOXCORE_API AccountId
{
    FString _domain;
    FString _name;
    FString _issuer;
    FString _displayName;
    FString _unityEnvironmentId;
    TArray<FString> _spokenLanguages;

public:
    /**
     * \brief Create an AccountId from a URI. Note: Internal use only.
     * \param uri The URI of the account.
     * \param displayName An optional display name that is seen by others. The default is an empty string.
     * \return The AccountId.
     */
    static AccountId CreateFromUri(const FString &uri, const TOptional<FString> &displayName = TOptional<FString>(), const TOptional<FString> &unityEnvironmentId = TOptional<FString>());
    /**
     * \brief A shortcut for AccountId::CreateFromUri(uri).Name(). Note: Internal use only.
     * \param uri The URI of the account.
     * \return The name returned from an AccountId created with a URI.
     */
    static FString AccountNameFromUri(const FString &uri, const TOptional<FString>& unityEnvironmentId = TOptional<FString>());
    /**
     * \brief Default constructor
     */
    AccountId();
    /**
     * \brief Constructor
     * \param issuer The issuer that is responsible for authenticating this account.
     * \param name The unique account name that you assigned to the player.
     * \param domain The Vivox domain that provides service for this account, for example: mt1s.vivox.com.
     * \param displayName An optional display name that is seen by others. The default is an empty string.
     * \param spokenLanguages An optional array of languages used as hints for audio transcription. The default is an empty array, which implies "en".
     * \param unityEnvironmentId The Unity Environment Id, from the Unity Dashboard, to be embedded in the Accounts URI, primarily for Moderation requests
     * \remarks The combined length of 'Issuer' + 'Name' must not exceed 124 characters. The DisplayName must not exceed 63 characters.
     * Name and DisplayName can only use the letters A-Z and a-z, the numbers 0-9, and the special characters =+-_.!~()%
     * You can specify up to three spoken languages in order of preference to inform transcription of all users in transcribed channels.
     * Note: IETF language tag strings are not validated, but are expected to conform to BCP 47 (https://tools.ietf.org/html/bcp47).
     */
    AccountId(const FString &issuer, const FString &name, const FString &domain, const TOptional<FString> &displayName = TOptional<FString>(), const TOptional<TArray<FString>>& spokenLanguages = TOptional<TArray<FString>>(), const TOptional<FString> &unityEnvironmentId = TOptional<FString>());
    /**
     * \brief Destructor
     */
    ~AccountId();

    /**
     * \brief The issuer that is responsible for authenticating this account.
     */
    const FString &Issuer() const;
    /**
     * \brief The unique account name that you assigned to the player.
     * \remarks This is the value the SDK uses internally to identify the user. To ensure character requirements, this typically matches the game or online ID of the player, or a hash of it.
     */
    const FString &Name() const;
    /**
     * \brief The Vivox domain that provides service for this account.
     */
    const FString &Domain() const;
    /**
    * \brief An optional display name that is seen by others. The default is an empty string.
    * \remarks The Vivox SDK makes no use of this value internally. You can use it in your game however you want, such as for example, in rosters or buddy lists. It is not required to be unique.
    */
    const FString &DisplayName() const;
    /**
    * \brief An optional array of languages used as hints for audio transcription. The default is an empty array, which implies "en".
    * \remarks You can specify up to three spoken languages in order of preference to inform transcription of all users in transcribed channels.
    * Note: IETF language tag strings are not validated, but are expected to conform to BCP 47 (https://tools.ietf.org/html/bcp47).
    */
    const TArray<FString> &SpokenLanguages() const;
    /**
     * \brief The Unity Environment of the UGS Project
     */
    const FString& UnityEnvironmentId() const;

    /**
     * \brief True if Issuer, Name, and Domain are all empty.
     */
    bool IsEmpty() const;

    /**
     * \brief True if Issuer and Domain are non-empty, and Name meets restrictions.
     */
    bool IsValid() const;

    /**
     * \brief Returns a complete user SIP URI formatted for Vivox Access Token generation, including the issuer, name, and other components.
     * \remarks Use this string as a token claim value representing a user identifier instead of hand-crafting one.

     */
    FString ToString() const;

    /** \brief Equality operator */
    bool operator ==(const AccountId &RHS) const;
    /** \brief InEquality operator */
    bool operator !=(const AccountId &RHS) const { return !operator ==(RHS); }

    /**
     * \brief True if ID is null or empty.
     */
    static bool IsNullOrEmpty(AccountId *id);
};

/**
 * \brief A standard Unreal Engine hash function that allows AccountId to be used as a key in UE collections.
 * \param id The account ID.
 * \return The hash.
 */
uint32 VIVOXCORE_API GetTypeHash(const AccountId &id);
