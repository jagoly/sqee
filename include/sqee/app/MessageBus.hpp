#pragma once

#include <map>
#include <typeindex>

#include <sqee/builtins.hpp>
#include <sqee/helpers.hpp>

namespace sq {

//============================================================================//

class MessageBus;

class ReceiverBase {};

//============================================================================//

template <class Message>
class ReceiverMethod : ReceiverBase
{
    static_assert(std::is_copy_constructible_v<Message>);
    static_assert(std::is_move_constructible_v<Message>);

protected: //=================================================//

    virtual void handle_message(Message message) = 0;

    friend class MessageBus;
};

//============================================================================//

template <class... Messages>
class Receiver : NonCopyable, public ReceiverMethod<Messages>...
{
public: //====================================================//

    inline Receiver(MessageBus& bus);

    inline virtual ~Receiver();

protected: //=================================================//

    std::array<bool, sizeof...(Messages)> mSubscribed;
    MessageBus& mBus;

    friend class MessageBus;
};

//============================================================================//

/// The SQEE MessageBus Class
class MessageBus final : NonCopyable
{
public: //====================================================//

    /// Subscribe a @ref Receiver to one message type.
    /// @tparam Message type of message to subscribe to
    /// @param receiver the message receiver object

    template <class Message, class... Messages>
    void subscribe(Receiver<Messages...>& receiver)
    {
        static_assert((std::is_same_v<Message, Messages> || ...));
        constexpr size_t index = index_in_pack_v<Message, Messages...>;

        if (receiver.mSubscribed[index] == false)
        {
            ReceiverMethod<Message>* base = &receiver;
            impl_subscribe(base, std::type_index(typeid(Message)));
            receiver.mSubscribed[index] = true;
        }
    }

    //--------------------------------------------------------//

    /// Subscribe a @ref Receiver to all message types.
    /// @param receiver the message receiver object

    template <class... Messages>
    void subscribe_all(Receiver<Messages...>& receiver)
    {
        ( subscribe<Messages>(receiver) , ... );
    }

    //--------------------------------------------------------//

    /// Unsubscribe a @ref Receiver from one message type.
    /// @tparam Message type of message to unsubscribe from
    /// @param receiver the message receiver object

    template <class Message, class... Messages>
    void unsubscribe(Receiver<Messages...>& receiver)
    {
        static_assert((std::is_same_v<Message, Messages> || ...));
        constexpr size_t index = index_in_pack_v<Message, Messages...>;

        if (receiver.mSubscribed[index] == true)
        {
            ReceiverMethod<Message>* base = &receiver;
            impl_unsubscribe(base, std::type_index(typeid(Message)));
            receiver.mSubscribed[index] = false;
        }
    }

    //--------------------------------------------------------//

    /// Unsubscribe a @ref Receiver from all message types.
    /// @param receiver the message receiver object

    template <class... Messages>
    void unsubscribe_all(Receiver<Messages...>& receiver)
    {
        ( unsubscribe<Messages>(receiver) , ... );
    }

    //--------------------------------------------------------//

    /// Register a new message type.
    /// @tparam Message type of message to register

    template <class Message> void register_type()
    { impl_register_type(std::type_index(typeid(Message))); }

    //--------------------------------------------------------//

    /// Unregister an existing message type.
    /// @tparam Message type of message to unregister

    template <class Message> void unregister_type()
    { impl_unregister_type(std::type_index(typeid(Message))); }

    //--------------------------------------------------------//

    /// Send a message to all subscribed receivers.
    /// @param message the message to publish

    template <class Message>
    void publish(Message message)
    {
        for (auto base : impl_get_subscribers(std::type_index(typeid(Message))))
            static_cast<ReceiverMethod<Message>*>(base)->handle_message(message);
    }

private: //===================================================//

    void impl_register_type(std::type_index type);

    void impl_unregister_type(std::type_index type);

    //--------------------------------------------------------//

    void impl_subscribe(ReceiverBase* receiver, std::type_index type);

    void impl_unsubscribe(ReceiverBase* receiver, std::type_index type);

    //--------------------------------------------------------//

    std::vector<ReceiverBase*>& impl_get_subscribers(std::type_index type);

    //--------------------------------------------------------//

    std::map<std::type_index, std::vector<ReceiverBase*>> mSubscriberMap;
};

//============================================================================//

template <class... Messages>
Receiver<Messages...>::Receiver(MessageBus& bus) : mBus(bus) {}

template <class... Messages>
Receiver<Messages...>::~Receiver() { mBus.unsubscribe_all(*this); }

//============================================================================//

} // namespace sq
