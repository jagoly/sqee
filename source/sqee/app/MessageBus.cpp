#include <sqee/app/MessageBus.hpp>

#include <sqee/core/Algorithms.hpp>
#include <sqee/debug/Assert.hpp>

using namespace sq;

//============================================================================//

MessageSourceBase& MessageBus::impl_get_message_source(std::type_index type)
{
    auto iter = mMessageSources.find(type);
    SQASSERT(iter != mMessageSources.end(), "message type not registered");
    return *iter->second;
}

//============================================================================//

void MessageBus::impl_add_message_source(std::type_index type, std::unique_ptr<MessageSourceBase>&& source)
{
    auto [iter, success] = mMessageSources.emplace(type, std::move(source));
    SQASSERT(success, "message source already registered");
}

//============================================================================//

void MessageBus::impl_remove_message_source(std::type_index type)
{
    auto iter = mMessageSources.find(type);
    SQASSERT(iter != mMessageSources.end(), "message source not registered");
    SQASSERT(iter->second->subscribers.empty(), "message source has subscribers");
    mMessageSources.erase(iter);
}

//============================================================================//

void MessageBus::impl_subscribe(void* receiver, std::vector<void*>& receivers)
{
    SQASSERT(algo::none_of(receivers, algo::pred_equal_to(receiver)), "receiver already subscribed");
    receivers.push_back(receiver);
}

//============================================================================//

void MessageBus::impl_unsubscribe(void* receiver, std::vector<void*>& receivers)
{
    const auto iter = algo::find(receivers, receiver);
    SQASSERT(iter != receivers.end(), "receiver not subscribed");
    *iter = receivers.back(); receivers.pop_back();
}
