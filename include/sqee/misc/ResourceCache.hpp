// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/misc/ResourceHandle.hpp>

namespace sq {

//============================================================================//

/// Class that provides caching of a type of resource.
template <class Key, class Type>
class ResourceCache final : private NonCopyable
{
public: //====================================================//

    using Factory = std::function<std::unique_ptr<Type>(const Key&)>;

    /// Set the callable to use to create new resources.
    void assign_factory(Factory factory)
    {
        mFactoryFunc = std::move(factory);
    }

    /// Acquire a handle to a resource. Create it if needed.
    Handle<Key, Type> acquire(const Key& key)
    {
        const auto [iter, created] = mResourceMap.try_emplace(key);
        if (created == true)
        {
            SQASSERT(mFactoryFunc != nullptr, "no factory assigned");
            iter->second.key = &iter->first;
            iter->second.data = mFactoryFunc(key);
            iter->second.count = 0u;
        }
        return Handle<Key, Type>(iter->second);
    }

    /// Reload cached resources. Handles will remain valid.
    void reload_resources()
    {
        for (auto& [key, resource] : mResourceMap)
            resource.data = mFactoryFunc(key);
    }

    /// Free resources that no longer have any handles.
    void free_unreachable()
    {
        for (auto iter = mResourceMap.begin(); iter != mResourceMap.end();)
        {
            if (iter->second.count == 0u)
                iter = mResourceMap.erase(iter);
            else ++iter;
        }
    }

private: //===================================================//

    // todo: experiment with using unordered_map instead
    std::map<Key, Resource<Key, Type>> mResourceMap;

    // we use a std::function to make things easy
    Factory mFactoryFunc;
};

//============================================================================//

} // namespace sq
