/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "Presence.h"
#include "VivoxCore.h"

Presence::Presence(PresenceStatus status, const FString& message)
{
    _currentStatus = status;
    _message = message;
}

PresenceStatus Presence::CurrentStatus() const
{
    return _currentStatus;
}

const FString& Presence::Message() const
{
    return _message;
}

bool Presence::operator==(const Presence& RHS) const
{
    return _currentStatus == RHS._currentStatus && _message == RHS._message;
}

bool Presence::operator!=(const Presence& RHS) const
{
    return !operator ==(RHS);
}
