/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "VivoxCoreCommon.h"
#include "IParticipantProperties.h"

/**
 * \brief A participant in a channel.
 *
 * \remarks The key for this interface is an account name returned by AccountId::Name().
 */
class VIVOXCORE_API IParticipant : public IParticipantProperties
{
public:
    /**
     * \brief Identifies the participant's specific sign in location.
     * \remarks Rarely used. In most implementations, use IParticipant::Account() as a unique identifier for participants in a channel instead of ParticipantId().
     * ParticipantId() is not the same as Account, but is the same as IPresenceLocation::LocationId(). The Account() of each participant is unique.
     * Accounts cannot be connected to a ChannelSession from multiple locations (the location for the latest ChannelSession connection overrides locations from earlier ChannelSession connections).
     * This will match a particular presence location when a user is signed in from more than one location.
     */
    virtual const FString &ParticipantId() const = 0;

    /**
     * \brief The ChannelSession that owns this participant.
     */
    virtual IChannelSession &ParentChannelSession() const = 0;

    /**
     * \brief The AccountId associated with this participant.
     */
    virtual const AccountId &Account() const = 0;
};
