#pragma once

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/dispatchkit/dispatchkit.hpp>
#include <chaiscript/utility/utility.hpp>

#include <sqee/app/MessageBus.hpp>

using chai::fun;
using chai::user_type;
using chai::base_class;
using chai::constructor;
using chai::type_conversion;
using chai::vector_conversion;

namespace sq {

//============================================================================//

template <class Message, class... Args> inline
void chai_add_message_type(chai::Module& m, const String& name)
{
//    constexpr auto construct = [](Args... args) { return Message { args... }; };

//    m.add(fun(construct), "MSG_" + name);
//    m.add(fun(&MessageBus::publish<Message>), "publish");
}

//============================================================================//

} // namespace sq
