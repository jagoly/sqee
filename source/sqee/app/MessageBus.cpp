#include <sqee/app/MessageBus.hpp>

using namespace sq;

//============================================================================//

void MessageBus::subscribe_front(ReceiverBase& receiver)
{
    SQASSERT(impl_check_registered(receiver.type) == true, "");
    SQASSERT(impl_check_subscribed(receiver) == false, "");

    receiver.unsubscriber = [&]() { this->unsubscribe(receiver); };

    auto& receivers = mSubscriberMap.at(receiver.type);
    receivers.insert(receivers.begin(), &receiver);
}

//============================================================================//

void MessageBus::subscribe_back(ReceiverBase& receiver)
{
    SQASSERT(impl_check_registered(receiver.type) == true, "");
    SQASSERT(impl_check_subscribed(receiver) == false, "");

    receiver.unsubscriber = [&]() { this->unsubscribe(receiver); };

    auto& receivers = mSubscriberMap.at(receiver.type);
    receivers.push_back(&receiver);
}

//============================================================================//

void MessageBus::unsubscribe(ReceiverBase& receiver)
{
    SQASSERT(impl_check_registered(receiver.type) == true, "");
    SQASSERT(impl_check_subscribed(receiver) == true, "");

    receiver.unsubscriber = nullptr;

    auto& receivers = mSubscriberMap.at(receiver.type);
    receivers.erase(algo::find(receivers, &receiver));
}


//============================================================================//

void MessageBus::impl_register_type(type_index type)
{
    SQASSERT(impl_check_registered(type) == false, "");

    mSubscriberMap.insert({ type, {} });
}

//============================================================================//

void MessageBus::impl_unregister_type(type_index type)
{
    SQASSERT(impl_check_registered(type) == true, "");

    auto& receivers = mSubscriberMap.at(type);
    for (auto r : receivers) r->unsubscriber = nullptr;

    mSubscriberMap.erase(type);
}
