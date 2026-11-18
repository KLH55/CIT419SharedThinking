/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "Engine/Blueprint.h"

/**
 * \brief The state of any resource with connection semantics.
 */
UENUM(BlueprintType)
enum class ConnectionState : uint8
{
    /**
     *\brief The resource is disconnected.
     */
    Disconnected,
    /**
     * \brief The resource is in the process of connecting.
     */
    Connecting,
    /**
     * \brief The resource is connected.
     */
    Connected,
    /**
     * \brief The resource is in the process of disconnecting.
     */
    Disconnecting
};
