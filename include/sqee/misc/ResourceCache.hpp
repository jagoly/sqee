#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

#include <sqee/misc/ResourceHandle.hpp>

namespace sq {

//============================================================================//

/// Abstract base for caching a type of @ref Resource.
/// @tparam Type type of the resource

template <class Type>
class ResourceCache : sq::NonCopyable
{
public: //====================================================//

    /// Virtual Destructor.
    virtual ~ResourceCache() = default;

    //--------------------------------------------------------//

    /// Acquire a handle to a resource, loading it if needed.
    /// @param path the path of the resource to acquire

    Handle<Type> acquire(const string& path)
    {
        auto& resource = mResourceMap[path];

        if (resource.loaded() == false)
        {
            resource.uptr = create(path);
            resource.path = path;
        }

        return resource;
    }

protected: //=================================================//

    /// Implement to load a new resource.
    /// @param path the path of the resource to acquire

    virtual unique_ptr<Type> create(const string& path) = 0;

    //--------------------------------------------------------//

    /// Map holding already cached resources.
    std::unordered_map<string, Resource<Type>> mResourceMap;
};

//============================================================================//

} // namespace sq
