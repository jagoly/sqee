// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/ResourceHandle.hpp>

namespace sq {

//============================================================================//

/// Class that provides caching of a type of resource.
template <class Key, class Type>
class ResourceCache final
{
public: //====================================================//

    using Factory = std::function<Type(const Key&)>;

    ResourceCache() = default;

    SQEE_COPY_DELETE(ResourceCache)
    SQEE_MOVE_DELETE(ResourceCache)

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
            try {
                iter->second.value = std::make_unique<Type>(mFactoryFunc(key));
            }
            catch (const std::exception& ex) {
                log_error_multiline("unhandled exception when loading resource\nkey:  {}\nwhat: {}", key, ex.what());
            }
            iter->second.count = 0u;
        }
        return Handle<Key, Type>(iter->second);
    }

    /// Try to acquire a resource. Return a handle to an error resource if creation fails.
    Handle<Key, Type> acquire_safe(const Key& key) noexcept
    {
        const auto [iter, created] = mResourceMap.try_emplace(key);
        if (created == true)
        {
            SQASSERT(mFactoryFunc != nullptr, "no factory assigned");
            iter->second.key = &iter->first;
            try {
                iter->second.value = std::make_unique<Type>(mFactoryFunc(key));
            }
            catch (const std::exception& ex) {
                iter->second.error = ex.what();
            }
            iter->second.count = 0u;
        }
        return Handle<Key, Type>(iter->second);
    }

    /// Reload cached resources. Handles and pointers will remain valid.
    void reload_resources()
    {
        for (auto& [key, resource] : mResourceMap)
        {
            // todo: how should we handle error resources?
            if (resource.value != nullptr)
                *resource.value = mFactoryFunc(key);
        }
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
