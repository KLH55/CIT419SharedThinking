/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "PresenceLocation.h"
#include "VivoxCore.h"
#include "VxcEvents.h"
#include "PresenceSubscription.h"

PresenceLocation::PresenceLocation(PresenceSubscription &subscription, const vx_evt_buddy_presence &evt) : _subscription(subscription)
{
    _currentPresence = Presence(static_cast<PresenceStatus>(evt.presence), UTF8_TO_TCHAR(evt.custom_message));
    _locationId = evt.encoded_uri_with_tag;
}

PresenceLocation::~PresenceLocation()
{
}

bool PresenceLocation::UpdateFromEvent(const vx_evt_buddy_presence& evt)
{
    Presence p(static_cast<PresenceStatus>(evt.presence), UTF8_TO_TCHAR(evt.custom_message));
    if (p != _currentPresence) {
        _currentPresence = p;
        return true;
    }
    return false;
}
