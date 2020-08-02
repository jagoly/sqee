// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

namespace sq {

//============================================================================//

template <class Message>
struct MessageReceiverSingle : private NonCopyable
{
    std::function<void(const Message&)> func = nullptr;
};

template <class... Messages>
class MessageReceiver final : public MessageReceiverSingle<Messages>...
{
public: //================================================//

    inline void subscribe(class MessageBus& messageBus);

    inline ~MessageReceiver();

    template <class Message, class Callable>
    void assign(Callable&& callable)
    {
        auto base = static_cast<MessageReceiverSingle<Message>*>(this);
        base->func = callable;
    }

private: //===============================================//

    class MessageBus* mMessageBus = nullptr;
};

//============================================================================//

struct MessageSourceBase : private NonCopyable
{
    virtual ~MessageSourceBase() = default;

    std::vector<void*> subscribers;
};

template <class Message>
struct MessageSource final : public MessageSourceBase
{
    void publish(const Message& message)
    {
        for (void* ptr : subscribers)
        {
            auto receiver = static_cast<MessageReceiver<Message>*>(ptr);
            if (receiver->func != nullptr) receiver->func(message);
        }
    }
};

//============================================================================//

/// The SQEE MessageBus Class
class SQEE_API MessageBus final : private NonCopyable
{
public: //====================================================//

    template <class Message>
    MessageSource<Message>& get_message_source()
    {
        auto& messageSource = impl_get_message_source(std::type_index(typeid(Message)));
        return static_cast<MessageSource<Message>&>(messageSource);
    }

    //--------------------------------------------------------//

    template <class Message> void add_message_source()
    {
        auto source = std::make_unique<MessageSource<Message>>();
        impl_add_message_source(std::type_index(typeid(Message)), std::move(source));
    }

    template <class Message> void remove_message_source()
    {
        impl_remove_message_source(std::type_index(typeid(Message)));
    }

    //--------------------------------------------------------//

    template <class Message>
    void subscribe(MessageReceiverSingle<Message>& receiver)
    {
        impl_subscribe(&receiver, get_message_source<Message>().subscribers);
    }

    template <class Message>
    void unsubscribe(MessageReceiverSingle<Message>& receiver)
    {
        impl_unsubscribe(&receiver, get_message_source<Message>().subscribers);
    }

private: //===================================================//

    MessageSourceBase& impl_get_message_source(std::type_index type);

    //--------------------------------------------------------//

    void impl_add_message_source(std::type_index type, std::unique_ptr<MessageSourceBase>&& source);

    void impl_remove_message_source(std::type_index type);

    //--------------------------------------------------------//

    void impl_subscribe(void* receiver, std::vector<void*>& receivers);

    void impl_unsubscribe(void* receiver, std::vector<void*>& receivers);

    //--------------------------------------------------------//

    std::map<std::type_index, std::unique_ptr<MessageSourceBase>> mMessageSources;
};

//============================================================================//

template <class... Messages>
void MessageReceiver<Messages...>::subscribe(MessageBus& messageBus)
{
    mMessageBus = &messageBus;
    ( mMessageBus->subscribe(static_cast<MessageReceiverSingle<Messages>&>(*this)), ... );
}

template <class... Messages>
MessageReceiver<Messages...>::~MessageReceiver()
{
    if (mMessageBus != nullptr)
    {
        ( mMessageBus->unsubscribe(static_cast<MessageReceiverSingle<Messages>&>(*this)), ... );
    }
}

//============================================================================//

// macros are nicer than a heap of annoying wrapper lambdas

#define SQEE_MB_BIND_METHOD(Receiver, Type, Method) \
Receiver.assign<Type>([this](const Type& msg) { Method(msg); })

//============================================================================//

} // namespace sq
