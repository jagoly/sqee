#pragma once

#include <functional>
#include <unordered_map>

#include <sqee/assert.hpp>
#include <sqee/builtins.hpp>

#include <sqee/misc/Algorithms.hpp>

namespace sq {

//============================================================================//

struct ReceiverBase
{
    ReceiverBase(type_index type) : type(type) {}
    ~ReceiverBase() { if (unsubscriber) unsubscriber(); }

    const type_index type;
    std::function<void()> unsubscriber;
};

template <class Message>
struct Receiver final : public ReceiverBase
{
    Receiver() : ReceiverBase(type_index(typeid(Message))) {}

    std::function<void(Message)> func;
};

//============================================================================//

/// The SQEE MessageBus Class
class MessageBus final : NonCopyable
{
public:

    //========================================================//

    void subscribe_front(ReceiverBase& receiver);

    void subscribe_back(ReceiverBase& receiver);

    void unsubscribe(ReceiverBase& receiver);

    //========================================================//

    template <class Message> void register_type()
    { impl_register_type(type_index(typeid(Message))); }

    template <class Message> void unregister_type()
    { impl_unregister_type(type_index(typeid(Message))); }

    //========================================================//

    template <class Message> void send_message(Message message)
    {
        const auto type = type_index(typeid(Message));
        SQASSERT(impl_check_registered(type) == true, "");

        auto& receivers = mSubscriberMap.at(type);

        for (ReceiverBase* receiver : receivers)
            static_cast<Receiver<Message>*>(receiver)->func(message);
    }

private:

    //========================================================//

    bool impl_check_registered(type_index type)
    { return mSubscriberMap.find(type) != mSubscriberMap.end(); }


    bool impl_check_subscribed(ReceiverBase& receiver)
    { return algo::exists(mSubscriberMap.at(receiver.type), &receiver); }

    //========================================================//

    void impl_register_type(type_index type);

    void impl_unregister_type(type_index type);

    //========================================================//

    std::unordered_map<type_index, vector<ReceiverBase*>> mSubscriberMap;
};

//============================================================================//

} // namespace sq
