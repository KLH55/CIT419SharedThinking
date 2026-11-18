/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#pragma once
#include "VivoxCoreCommon.h"
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(VivoxCore, Log, All);

template<class T>
typename T::ElementType::ValueType First(const T &items)
{
    TArray<typename T::ElementType::KeyType> keys;
    items.GenerateKeyArray(keys);
    return items[keys[0]];
}

class ClientImpl;
