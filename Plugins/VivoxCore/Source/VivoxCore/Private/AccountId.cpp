/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "AccountId.h"
#include "VivoxCore.h"
#include "VivoxCoreCommonImpl.h"
#include "Internationalization/Regex.h"

AccountId AccountId::CreateFromUri(const FString& uri, const TOptional<FString>& displayName, const TOptional<FString>& unityEnvironmentId)
{
    if (uri.IsEmpty())
        return AccountId();

    const FRegexPattern regex("sip:\\.([^.]+)\\.(.*?)(?:\\.([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}))?\\.@([a-zA-Z0-9.]+)");
    FRegexMatcher matcher(regex, uri);
    AccountId id;
    bool found = matcher.FindNext();
    ensure(found);
    if (found) {
        id._issuer = matcher.GetCaptureGroup(1);
        id._name = matcher.GetCaptureGroup(2);
        if (!matcher.GetCaptureGroup(3).IsEmpty())
            id._unityEnvironmentId = matcher.GetCaptureGroup(3);
        else if (unityEnvironmentId.IsSet())
            id._unityEnvironmentId = unityEnvironmentId.GetValue();
        id._domain = matcher.GetCaptureGroup(4);
        if (displayName.IsSet())
            id._displayName = displayName.GetValue();
        ensure(id.IsValid());
        return id; // may be invalid
    }
    return AccountId();
}

FString AccountId::AccountNameFromUri(const FString& uri, const TOptional<FString>& unityEnvironmentId)
{
    return CreateFromUri(uri, TOptional<FString>(), unityEnvironmentId.IsSet() ? unityEnvironmentId.GetValue() : TOptional<FString>()).Name();
}

AccountId::AccountId()
{
}

AccountId::AccountId(const FString& issuer, const FString& name, const FString& domain, const TOptional<FString>& displayName, const TOptional<TArray<FString>>& spokenLanguages, const TOptional<FString>& unityEnvironmentId)
{
    ensure(!issuer.IsEmpty());
    ensure(!name.IsEmpty());
    ensure(!domain.IsEmpty());

    _issuer = issuer;
    _name = name;
    _domain = domain;
    if (unityEnvironmentId.IsSet())
        _unityEnvironmentId = unityEnvironmentId.GetValue();
    if (displayName.IsSet())
        _displayName = displayName.GetValue();
    if (spokenLanguages.IsSet()) {
        _spokenLanguages = spokenLanguages.GetValue();
        for (int32 Index = 0; Index != _spokenLanguages.Num(); ++Index)
            _spokenLanguages[Index].RemoveSpacesInline();
    }
}

AccountId::~AccountId()
{
}

const FString& AccountId::Issuer() const
{
    return _issuer;
}

const FString& AccountId::Name() const
{
    return _name;
}

const FString& AccountId::Domain() const
{
    return _domain;
}

const FString& AccountId::DisplayName() const
{
    return _displayName;
}

const TArray<FString>& AccountId::SpokenLanguages() const
{
    return _spokenLanguages;
}

const FString& AccountId::UnityEnvironmentId() const
{
    return _unityEnvironmentId;
}

bool AccountId::IsEmpty() const
{
    return _issuer.IsEmpty() && _name.IsEmpty() && _domain.IsEmpty();
}

FString AccountId::ToString() const
{
    if (IsEmpty()) return FString();
    return "sip:." + _issuer + "." + _name  + (_unityEnvironmentId.IsEmpty() ? "" : "." + _unityEnvironmentId) + ".@" + _domain;
}

bool AccountId::operator==(const AccountId& RHS) const
{
    return _issuer == RHS._issuer && _name == RHS._name && _domain == RHS._domain;
}

bool AccountId::IsNullOrEmpty(AccountId* id)
{
    return id == nullptr || id->IsEmpty();
}

uint32 GetTypeHash(const AccountId& id)
{
    uint32 hc = GetTypeHash(id.Name());
    hc *= 397;
    hc ^= GetTypeHash(id.Issuer());
    hc *= 397;
    hc ^= GetTypeHash(id.Domain());
    return hc;
}

bool AccountId::IsValid() const
{
    // FTCHARToUTF8::Length() returns the number of bytes for the encoded string,
    // excluding the null terminator. This accounts for mixed multi-byte characters.
    int32 displayNameByteCount = ((FTCHARToUTF8)*_displayName).Length();

    if (_issuer.IsEmpty()
     || _domain.IsEmpty()
     || _name.Len() + _issuer.Len() > 124
     || displayNameByteCount > 63)
    {
        return false;
    }
    FString ValidCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890=+-_.!~()%";
    int32 loc;
    auto ConstItrN = _name.CreateConstIterator();
    while (_name.IsValidIndex(ConstItrN.GetIndex()))
        if (!ValidCharacters.FindChar(_name[ConstItrN++.GetIndex()], loc))
            return false;

    // no spokenLanguages validation.
    return true;
}
