#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

#include <sqee/misc/ResourceHandle.hpp>

namespace sq {

//============================================================================//

/// Abstract base for caching a type of @ref Resource.
/// @tparam Key hashable key type
/// @tparam Type type of the resource

template <class Key, class Type>
class ResourceCache : sq::NonCopyable
{
public: //====================================================//

    using HandleT = Handle<Key, Type>;

    //--------------------------------------------------------//

    /// Virtual Destructor.
    virtual ~ResourceCache() = default;

    //--------------------------------------------------------//

    /// Acquire a handle to a resource, loading it if needed.
    /// @param path the path of the resource to acquire

    HandleT acquire(const Key& key)
    {
        auto& resource = mResourceMap[key];

        if (resource.loaded() == false)
        {
            resource.uptr = create(key);
            resource.key = key;
        }

        return resource;
    }

protected: //=================================================//

    /// Implement to load a new resource.
    /// @param key the key of the resource to acquire

    virtual unique_ptr<Type> create(const Key& key) = 0;

    //--------------------------------------------------------//

    /// Map holding already cached resources.
    std::unordered_map<Key, Resource<Key, Type>> mResourceMap;
};

//============================================================================//

} // namespace sq
