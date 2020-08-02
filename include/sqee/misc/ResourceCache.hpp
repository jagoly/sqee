// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/misc/ResourceHandle.hpp>

#include <unordered_map>

namespace sq {

//============================================================================//

/// Abstract base for caching a type of @ref Resource.
/// @tparam Key hashable key type
/// @tparam Type type of the resource

template <class Key, class Type>
class ResourceCache : private NonCopyable
{
public: //====================================================//

    /// Virtual Destructor.
    virtual ~ResourceCache() = default;

    //--------------------------------------------------------//

    /// Acquire a handle to a resource, loading it if needed.
    /// @param path the path of the resource to acquire

    Handle<Key, Type> acquire(const Key& key)
    {
        auto& resource = mResourceMap[key];

        if (resource.loaded() == false)
        {
            resource.uptr = create(key);
            resource.key = key;
        }

        return resource;
    }

    //--------------------------------------------------------//

    /// Reload all cached resources.
    void reload_resources()
    {
        for (auto& [key, resource] : mResourceMap)
            resource.uptr = create(key);
    }

protected: //=================================================//

    /// Implement to load a new resource.
    /// @param key the key of the resource to load

    virtual std::unique_ptr<Type> create(const Key& key) = 0;

    //--------------------------------------------------------//

    /// Map holding already cached resources.
    std::unordered_map<Key, Resource<Key, Type>> mResourceMap;
};

//============================================================================//

} // namespace sq
