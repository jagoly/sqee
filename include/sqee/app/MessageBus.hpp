#pragma once

#include <functional>
#include <unordered_map>

#include <sqee/assert.hpp>
#include <sqee/builtins.hpp>

namespace sq {

struct ReceiverBase {
    virtual ~ReceiverBase() {
        if (unsubscriber) unsubscriber();
    } std::function<void()> unsubscriber;
};

template<class... Args>
struct Receiver final : public ReceiverBase {
    Receiver(std::function<void(Args...)> _func) : func(_func) {}
    const std::function<void(Args...)> func;
};


class MessageBus final : NonCopyable {
public:
    template<class... Args>
    void subscribe_first(const string& _type, Receiver<Args...>& _receiver) {
        auto& receivers = prepare_subscribe(_type, _receiver);
        receivers.insert(receivers.begin(), &_receiver);
    }

    template<class... Args>
    void subscribe_last(const string& _type, Receiver<Args...>& _receiver) {
        auto& receivers = prepare_subscribe(_type, _receiver);
        receivers.push_back(&_receiver);
    }

    template<class... Args>
    void subscribe_before(const string& _type, const Receiver<Args...>& _position, Receiver<Args...>& _receiver) {
        auto& receivers = prepare_subscribe(_type, _receiver);
        vector<ReceiverBase*>::iterator iter = receivers.begin();
        while (&*iter != &_position && ++iter != receivers.end()) {}
        SQASSERT(iter != receivers.end(), "Receiver is not subscribed");
        receivers.insert(iter, &_receiver);
    }

    template<class... Args>
    void subscribe_after(const string& _type, const Receiver<Args...>& _position, Receiver<Args...>& _receiver) {
        auto& receivers = prepare_subscribe(_type, _receiver);
        vector<ReceiverBase*>::iterator iter = receivers.begin();
        while (*iter != &_position && ++iter != receivers.end()) {}
        SQASSERT(iter != receivers.end(), "Receiver is not subscribed");
        receivers.insert(++iter, &_receiver);
    }

    void unsubscribe(const string& _type, ReceiverBase& _receiver) {
        const auto iterA = subscriberMap.find(_type);
        SQASSERT(iterA != subscriberMap.end(), "message type not registered");
        vector<ReceiverBase*>& receivers = iterA->second.second;
        vector<ReceiverBase*>::iterator iterB = receivers.begin();
        while (*iterB != &_receiver && ++iterB != receivers.end()) {}
        SQASSERT(iterB != receivers.end(), "Receiver is not subscribed");
        iterA->second.second.erase(iterB); _receiver.unsubscriber = nullptr;
    }

    template<class... Args>
    void register_type(const string& _type) {
        SQASSERT(!type_registered(_type), "message type already registered");
        subscriberMap.emplace(_type, pair<const std::type_info*, vector<ReceiverBase*>>(&typeid(void(Args...)), {}));
    }

    void unregister_type(const string& _type) {
        const auto iter = subscriberMap.find(_type);
        SQASSERT(iter != subscriberMap.end(), "message type not registered");
        for (auto rec : iter->second.second) rec->unsubscriber = nullptr;
        subscriberMap.erase(iter);
    }

    bool type_registered(const string& _type) {
        return subscriberMap.find(_type) != subscriberMap.end();
    }

    template<class... Args>
    void send_message(const string& _type, Args... _args) {
        const auto iter = subscriberMap.find(_type);
        SQASSERT(iter != subscriberMap.end(), "message type not registered");
        SQASSERT(iter->second.first == &typeid(void(Args...)), "message types do not match");
        for (const auto receiver : iter->second.second)
            static_cast<Receiver<Args...>*>(receiver)->func(_args...);
    }

private:
    std::unordered_map<string, pair<const std::type_info*, vector<ReceiverBase*>>> subscriberMap;

    template<class... Args>
    vector<ReceiverBase*>& prepare_subscribe(const string& _type, Receiver<Args...>& _receiver) {
        const auto iter = subscriberMap.find(_type);
        SQASSERT(iter != subscriberMap.end(), "message type not registered");
        SQASSERT(iter->second.first == &typeid(void(Args...)), "message types do not match");
        _receiver.unsubscriber = [&](){ unsubscribe(_type, _receiver); };
        return iter->second.second;
    }
};

}
