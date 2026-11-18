/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "IPresenceSubscription.h"
#include "LoginSession.h"

struct vx_evt_buddy_presence;
class LoginSession;

/**
 *
 */
class PresenceSubscription : public IPresenceSubscription
{
    AccountId _subscribedAccount;
    TMap<FString, IPresenceLocation*> _locations;
    ::LoginSession &_loginSession;
public:
    PresenceSubscription(::LoginSession &session,const AccountId &subscribedAccount);
    ~PresenceSubscription();
    const AccountId& SubscribedAccount() const override { return _subscribedAccount; }
    TMap<FString, IPresenceLocation*> Locations() const override { return _locations; }
    ::ILoginSession &LoginSession() override {
        return _loginSession;
    }
    void HandleEvent(const vx_evt_buddy_presence &evt);
};

