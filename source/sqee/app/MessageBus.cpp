// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#ifndef SQEE_MSVC
#include <sqee/app/MessageBus.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Algorithms.hpp>

using namespace sq;

//============================================================================//

void MessageBus::impl_register_type(std::type_index type)
{
    auto [iter, success] = mSubscriberMap.insert({ type, {} });
    SQASSERT(success, "message type already registered");
}

//============================================================================//

void MessageBus::impl_unregister_type(std::type_index type)
{
    auto iter = mSubscriberMap.find(type);
    SQASSERT(iter != mSubscriberMap.end(), "message type not registered");
    SQASSERT(iter->second.empty(), "message type has subscribers");
    mSubscriberMap.erase(iter);
}

//============================================================================//

void MessageBus::impl_subscribe(ReceiverBase* receiver, std::type_index type)
{
    auto iter = mSubscriberMap.find(type);
    SQASSERT(iter != mSubscriberMap.end(), "message type not registered");
    SQASSERT(!algo::exists(iter->second, receiver), "receiver already subscribed");
    iter->second.push_back(receiver);
}

//============================================================================//

void MessageBus::impl_unsubscribe(ReceiverBase* receiver, std::type_index type)
{
    auto mapIter = mSubscriberMap.find(type);
    SQASSERT(mapIter != mSubscriberMap.end(), "message type not registered");
    auto vecIter = algo::find(mapIter->second, receiver);
    SQASSERT(vecIter != mapIter->second.end(), "receiver not subscribed");
    mapIter->second.erase(vecIter);
}

//============================================================================//

Vector<ReceiverBase*>& MessageBus::impl_get_subscribers(std::type_index type)
{
    auto iter = mSubscriberMap.find(type);
    SQASSERT(iter != mSubscriberMap.end(), "message type not registered");
    return iter->second;
}

#endif
