#pragma once

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/dispatchkit/dispatchkit.hpp>
#include <chaiscript/utility/utility.hpp>

#include <sqee/setup.hpp>
#include <sqee/builtins.hpp>

#include <sqee/app/MessageBus.hpp>

using chai::fun;
using chai::user_type;
using chai::base_class;
using chai::constructor;
using chai::type_conversion;
using chai::vector_conversion;
using chai::utility::add_class;

namespace sq {

template <class Message, class... Args> inline
void chai_add_message_type(chai::Module& m, const string& name)
{
    const auto send_func = [](MessageBus* mbus, Args... args) { mbus->send_message(Message{args...}); };
    m.add(fun(send_func), "send_" + name);
}

} // namespace sq
