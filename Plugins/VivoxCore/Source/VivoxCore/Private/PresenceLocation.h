/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "IPresenceLocation.h"

class PresenceSubscription;
struct vx_evt_buddy_presence;

/**
 *
 */
class PresenceLocation : public IPresenceLocation
{
    FString _locationId;
    Presence _currentPresence;
    IPresenceSubscription &_subscription;
public:
    PresenceLocation(PresenceSubscription &subscription, const vx_evt_buddy_presence &evt);
    ~PresenceLocation();

    const FString& LocationId() const override { return _locationId; }
    const Presence& CurrentPresence() const override { return _currentPresence; }
    IPresenceSubscription &Subscription() const override { return _subscription; }

    bool UpdateFromEvent(const vx_evt_buddy_presence &evt);
};
