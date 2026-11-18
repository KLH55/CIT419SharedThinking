/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "VivoxCoreCommon.h"

/**
 * \brief Presence information for a user signed in at a particular location.
 */
class VIVOXCORE_API IPresenceLocation
{
public:
    /**
     * \brief Destructor
     */
    virtual ~IPresenceLocation() = default;
    /**
     * \brief The unique identifier for this account's specific login session.
     */
    virtual const FString &LocationId() const = 0;

    /**
     * \brief The presence for this account at this location.
     */
    virtual const Presence &CurrentPresence() const = 0;

    /**
     * \brief The subscription that owns this presence location. This does not change and does not raise a PropertyChangedEvent.
     */
    virtual IPresenceSubscription &Subscription() const = 0;
};
