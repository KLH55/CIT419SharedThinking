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
 * A subscription to another user for online status.
 */
class VIVOXCORE_API IPresenceSubscription
{
public:
    DECLARE_EVENT_OneParam(IPresenceSubscription, AfterLocationAdded, const IPresenceLocation &)
    DECLARE_EVENT_OneParam(IPresenceSubscription, AfterLocationUpdated, const IPresenceLocation &)
    DECLARE_EVENT_OneParam(IPresenceSubscription, BeforeLocationRemoved, const IPresenceLocation &)

public:
    virtual ~IPresenceSubscription() = default;
    /**
     * The account that this subscription pertains to.
     */
    virtual const AccountId &SubscribedAccount() const = 0;

    /**
     * The login session that owns this subscription.
     */
    virtual ILoginSession &LoginSession() = 0;
    /**
     * If the account associated with this subscription is signed in, then the Locations map has an entry for the location of each login session for that user.
     */
    virtual TMap<FString, IPresenceLocation *> Locations() const = 0;
    /**
    * This event is raised after a device has been added to the Locations() collection.
    */
    AfterLocationAdded EventAfterLocationAdded;

    /**
    * This event is raised after a device has been added to the Locations() collection.
    */
    AfterLocationUpdated EventAfterLocationUpdated;

    /**
    * This event is raised before a device is removed from the Locations() collection.
    */
    BeforeLocationRemoved EventBeforeLocationRemoved;
};
