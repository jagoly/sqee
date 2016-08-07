#pragma once

#include <functional>
#include <unordered_map>

#include <sqee/assert.hpp>
#include <sqee/builtins.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Algorithms.hpp>


namespace sq {

struct ReceiverBase {
    ReceiverBase(type_index _type) : type(_type) {}
    virtual ~ReceiverBase() { if (unsubscriber) unsubscriber(); }
    const type_index type; std::function<void()> unsubscriber;
};

template<class MessageType>
struct Receiver final : public ReceiverBase {
    Receiver() : ReceiverBase(type_index(typeid(MessageType))) {}
    std::function<void(MessageType)> func = nullptr;
};


class MessageBus final : NonCopyable {
public:

    void subscribe_front(ReceiverBase& _receiver) {
        SQASSERT(impl_check_registered(_receiver.type), "message type not registered");
        auto& receivers = impl_prepare_subscribe(_receiver);
        receivers.insert(receivers.begin(), &_receiver);
    }

    void subscribe_back(ReceiverBase& _receiver) {
        SQASSERT(impl_check_registered(_receiver.type), "message type not registered");
        auto& receivers = impl_prepare_subscribe(_receiver);
        receivers.push_back(&_receiver);
    }

    void unsubscribe(ReceiverBase& _receiver) {
        auto& subscribers = subscriberMap.at(_receiver.type);
        const auto iter = sq::algo::find(subscribers, &_receiver);
        SQASSERT(iter != subscribers.end(), "receiver is not subscribed");
        subscribers.erase(iter); _receiver.unsubscriber = nullptr;
    }

    template<class MType> void register_type() {
        const type_index type = type_index(typeid(MType));
        SQASSERT(!impl_check_registered(type), "message type not registered");
        subscriberMap.insert({type, {}});
    }

    template<class MType> void unregister_type() {
        const type_index type = type_index(typeid(MType));
        SQASSERT(impl_check_registered(type), "message type not registered");
        vector<ReceiverBase*>& receivers = subscriberMap.at(type);
        for (auto* rec : receivers) rec->unsubscriber = nullptr;
        subscriberMap.erase(type);
    }

    template<class MType> void send_message(const MType _message) {
        const type_index type = type_index(typeid(MType));
        SQASSERT(impl_check_registered(type), "message type not registered");
        vector<ReceiverBase*>& receivers = subscriberMap.at(type);
        for (auto* rec : receivers) static_cast<Receiver<MType>*>(rec)->func(_message);
    }

private:
    std::unordered_map<type_index, vector<ReceiverBase*>> subscriberMap;

    bool impl_check_registered(type_index _type) {
        return subscriberMap.find(_type) != subscriberMap.end();
    }

    vector<ReceiverBase*>& impl_prepare_subscribe(ReceiverBase& _receiver) {
        vector<ReceiverBase*>& receivers = subscriberMap.at(_receiver.type);
        _receiver.unsubscriber = [&]() { unsubscribe(_receiver); };
        return receivers;
    }
};

}
