/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */
#pragma once

# define _VX_STR(x) #x
# define VX_STR(x) _VX_STR(x)

# if defined(VIVOXDOC) || defined(SWIG)
#   define VIVOXSDK_DLLEXPORT
# else
#   if defined(_MSC_VER) || defined(__ORBIS__) || defined(__PROSPERO__)
#       ifdef BUILD_SHARED
#           ifdef BUILDING_VIVOXSDK
#               define VIVOXSDK_DLLEXPORT __declspec(dllexport)
#           else
#               define VIVOXSDK_DLLEXPORT __declspec(dllimport)
#           endif
#       else
#           define VIVOXSDK_DLLEXPORT
#       endif
#   else
#       define VIVOXSDK_DLLEXPORT __attribute__ ((visibility("default")))
#   endif
# endif

# if defined(__GNUC__) || defined(__clang__)
#   define _VX_DO_PRAGMA(x) _Pragma(#x)
#   define VX_DEPRECATED_IGNORE_BEGIN()                                 \
    _VX_DO_PRAGMA(clang diagnostic push)                                \
    _VX_DO_PRAGMA(clang diagnostic ignored "-Wdeprecated-declarations") \
    _VX_DO_PRAGMA(clang diagnostic ignored "-W#pragma-messages")
#   define VX_DEPRECATED_IGNORE_END() \
    _VX_DO_PRAGMA(clang diagnostic pop)

# elif defined(_MSC_VER)
#   define _VX_DO_PRAGMA(x) __pragma(x)
#   define VX_DEPRECATED_IGNORE_BEGIN() \
    _VX_DO_PRAGMA(warning(push))        \
    _VX_DO_PRAGMA(warning(disable: 4995 4996))
#   define VX_DEPRECATED_IGNORE_END() \
    _VX_DO_PRAGMA(warning(pop))

# else
#   define _VX_DO_PRAGMA(x)
#   define VX_DEPRECATED_IGNORE_BEGIN()
#   define VX_DEPRECATED_IGNORE_END()
# endif

# if defined(__GNUC__) || defined(__clang__)
#   define VX_DEPRECATED(since) __attribute__ ((deprecated("Since "#since)))
#   define VX_DEPRECATED_FOR(since, replacement) __attribute__ ((deprecated("Since "#since "; use "#replacement)))
#   define VX_DEPRECATED_ENUM_GCC(symbol, since) symbol VX_DEPRECATED(since)
#   define VX_DEPRECATED_ENUM_MSC(symbol)

# elif defined(_MSC_VER)
#   define VX_DEPRECATED(since) __declspec(deprecated("Since "#since))
#   define VX_DEPRECATED_FOR(since, replacement) __declspec(deprecated("Since "#since "; use "#replacement))
#   define VX_DEPRECATED_ENUM_GCC(symbol, since) symbol
#   define VX_DEPRECATED_ENUM_MSC(symbol) __pragma(deprecated(symbol))

# else
#   pragma message("WARNING: VX_DEPRECATED is not implemented for this compiler")
#   define VX_DEPRECATED(since)
#   define VX_DEPRECATED_FOR(since, replacement)
#   define VX_DEPRECATED_ENUM_GCC(symbol, since) symbol
#   define VX_DEPRECATED_ENUM_MSC(symbol)
# endif

# if defined(__GNUC__) || defined(__clang__)
#   define VX_DEPRECATED_MACRO(name, since) _VX_DO_PRAGMA(message("Warning: '" _VX_STR(name) "' is deprecated: Since " VX_STR(since)))

# elif defined(_MSC_VER)
#   define VX_DEPRECATED_MACRO(name, since) _VX_DO_PRAGMA(message(__FILE__ "(" VX_STR(__LINE__) "): warning: '" _VX_STR(name) "' is deprecated: Since " VX_STR(since)))

# else
#   define VX_DEPRECATED_MACRO(name, since) name
# endif
