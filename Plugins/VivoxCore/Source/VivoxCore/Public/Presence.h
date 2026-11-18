/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "Engine/Blueprint.h"
#include "VivoxCoreCommon.h"

/**
* \brief The online status of the user.
*/
UENUM(BlueprintType)
enum class PresenceStatus : uint8
{
    /**
    * \brief Not available (offline).
    */
    Unavailable,
    /**
    * \brief Generally available.
    */
    Available,
    /**
    * \brief Available to chat.
    */
    Chat,
    /**
    * \brief Do Not Disturb.
    */
    DoNotDisturb,
    /**
    * \brief Away.
    */
    Away,
    /**
    * \brief Away for an extended period of time.
    */
    ExtendedAway
};

/**
 * \brief The presence information for a user at a location.
 */
class VIVOXCORE_API Presence
{
    PresenceStatus _currentStatus;
    FString _message;
public:
    /**
     * \brief Constructor
     */
    explicit Presence(PresenceStatus status = PresenceStatus::Unavailable, const FString &message = FString());

    /**
     * \brief The online status of the user.
     */
    PresenceStatus CurrentStatus() const;
    /**
     * \brief An optional message published by that user.
     */
    const FString &Message() const;
    /** \brief Equality operator */
    bool operator ==(const Presence &RHS) const;
    /** \brief Inequality operator */
    bool operator !=(const Presence &RHS) const;
};
