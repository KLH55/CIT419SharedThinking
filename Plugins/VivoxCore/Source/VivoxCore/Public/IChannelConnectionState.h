/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "VivoxCoreCommon.h"
#include "ConnectionState.h"

/**
 * \brief A connection state change in a channel.
 */
class VIVOXCORE_API IChannelConnectionState
{
public:
    /**
     * \brief Destructor
     */
    virtual ~IChannelConnectionState() = default;
    /**
     * \brief The connection state of the resource.
     */
    virtual const ConnectionState &State() const = 0;
    /**
     * \brief The ChannelSession in which this connection state change took place.
     */
    virtual IChannelSession &ChannelSession() const = 0;
};
