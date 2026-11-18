/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "PresenceSubscription.h"
#include "VivoxCore.h"
#include "VxcErrors.h"
#include "PresenceLocation.h"

PresenceSubscription::PresenceSubscription(::LoginSession &loginSession, const AccountId &subscribedAccount) : _loginSession(loginSession)
{
    _subscribedAccount = subscribedAccount;
}

PresenceSubscription::~PresenceSubscription()
{
    for(auto item : _locations) {
        delete item.Value;
    }
}

void PresenceSubscription::HandleEvent(const vx_evt_buddy_presence& evt)
{
    FString locationId = evt.encoded_uri_with_tag;
    if(_locations.Contains(locationId)) {
        PresenceLocation *location = static_cast<PresenceLocation *>(_locations[locationId]);
        bool updated = location->UpdateFromEvent(evt);
        if (updated)
            EventAfterLocationUpdated.Broadcast(*location);
    }  else {
        PresenceLocation *location = new PresenceLocation(*this, evt);
        _locations.Add(locationId, location);
        EventAfterLocationAdded.Broadcast(*location);
    }
}
