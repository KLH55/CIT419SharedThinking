/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

#include "ChannelId.h"
#include "VivoxCore.h"
#include "Internationalization/Regex.h"

FString GetUriDesignator(ChannelType value)
{
    switch (value)
    {
    case ChannelType::Echo:
        return "e";
    case ChannelType::NonPositional:
        return "g";
    case ChannelType::Positional:
        return "d";
    }
    ensure("Invalid ChannelType" && false);
    return FString();
}

ChannelId ChannelId::CreateFromUri(const FString& uri, const TOptional<FString>& unityEnvironmentId)
{
    if (uri.IsEmpty())
        return ChannelId();
    FRegexPattern regex("sip:confctl-(e|g|d)-([^.]+)\\.(.*?)(?:\\.([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}))?(?:!p-([^@]+))?@([a-zA-Z0-9.]+)");
    FRegexMatcher matcher(regex, uri);
    ChannelId id;
    bool found = matcher.FindNext();
    ensure(found);
    if (found) {
        FString type = matcher.GetCaptureGroup(1);
        id._issuer = matcher.GetCaptureGroup(2);
        id._name = matcher.GetCaptureGroup(3);
        if (id._name.Contains(TEXT("(t-largetext)"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
        {
            id._largeTextChannel = true;
            //Pop off the t-largetext from the name after, to not concatenate it into the name or limit names away from ()
            id._name = id._name.Replace(TEXT("(t-largetext)"), TEXT(""));
        }
        if (matcher.GetCaptureGroup(4) != "")
        {
            id._unityEnvironmentId = matcher.GetCaptureGroup(4);
        }
        id._domain = matcher.GetCaptureGroup(6);
        if (type == "g")
            id._type = ChannelType::NonPositional;
        else if (type == "e")
            id._type = ChannelType::Echo;
        else if (type == "d")
        {
            id._type = ChannelType::Positional;
            id._properties = Channel3DProperties::CreateFromString(matcher.GetCaptureGroup(5));
        }
        else
        {
            ensure("Invalid Channel Type String" && false);
        }
        ensure(id.IsValid());
        return id; // may be invalid
    }
    return ChannelId();
}

ChannelId::ChannelId()
{
    _type = ChannelType::NonPositional;
}

ChannelId::ChannelId(const FString& issuer, const FString& name, const FString& domain, ChannelType type, Channel3DProperties properties, const TOptional<FString>& unityEnvironmentId, bool largeTextChannel)
{
    ensure(!issuer.IsEmpty());
    ensure(!name.IsEmpty());
    ensure(!domain.IsEmpty());
    _issuer = issuer;
    _name = name;
    _domain = domain;
    _type = type;
    _properties = properties;
    _largeTextChannel = largeTextChannel;
    if (unityEnvironmentId.IsSet())
        _unityEnvironmentId = unityEnvironmentId.GetValue();
}

ChannelId::~ChannelId()
{
}

const FString& ChannelId::Issuer() const
{
    return _issuer;
}

const FString& ChannelId::Name() const
{
    return _name;
}

const FString& ChannelId::Domain() const
{
    return _domain;
}

const bool& ChannelId::LargeTextChannel() const
{
    return _largeTextChannel;
}

ChannelType ChannelId::Type() const
{
    return _type;
}

Channel3DProperties ChannelId::Properties() const
{
    return _properties;
}

const FString& ChannelId::UnityEnvironmentId() const
{
    return _unityEnvironmentId;
}

bool ChannelId::IsEmpty() const
{
    return _name.IsEmpty() && _domain.IsEmpty() && _issuer.IsEmpty() && _type == ChannelType::NonPositional;
}

FString ChannelId::ToString() const
{
    if (IsEmpty())
        return FString();
    FString prefix;
    FString props;
    switch (_type) {
    case ChannelType::Echo:
        prefix = "sip:confctl-e-"; break;
    case ChannelType::NonPositional:
        prefix = "sip:confctl-g-"; break;
    case ChannelType::Positional:
        prefix = "sip:confctl-d-";
        props = _properties.ToString();
        break;
    }
    ensure(!prefix.IsEmpty());
    return prefix + _issuer + "." + _name + (_largeTextChannel ? "(t-largetext)" : "") + (_unityEnvironmentId.IsEmpty() ? "" : "." + _unityEnvironmentId) + (props.IsEmpty() ? "" : "!" + props) + "@" + _domain;
}

bool ChannelId::IsNullOrEmpty(ChannelId* id)
{
    return id == nullptr || id->IsEmpty();
}

bool ChannelId::operator==(const ChannelId& RHS) const
{
    return _type == RHS._type &&
        _name == RHS._name &&
        _issuer == RHS._issuer &&
        _domain == RHS._domain;
}

uint32 GetTypeHash(const ChannelId& id)
{
    uint32 hc = GetTypeHash(id.Name());
    hc *= 397;
    hc ^= GetTypeHash(id.Issuer());
    hc *= 397;
    hc ^= GetTypeHash(id.Domain());
    hc *= 397;
    hc ^= GetTypeHash(static_cast<uint32>(id.Type()));
    return hc;
}

bool ChannelId::IsValid() const
{
    if (_issuer.IsEmpty()
     || _domain.IsEmpty()
     || _name.Len() > 200
     || (_type == ChannelType::Positional && !_properties.IsValid()))
    {
        return false;
    }
    FString ValidCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890=+-_.!~()%";
    int32 loc;
    auto ConstItr = _name.CreateConstIterator();
    while (_name.IsValidIndex(ConstItr.GetIndex()))
        if (!ValidCharacters.FindChar(_name[ConstItr++.GetIndex()], loc))
            return false;

    return true;
}
