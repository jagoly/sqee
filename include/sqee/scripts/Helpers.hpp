#pragma once

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

template<class ValueT> inline
void chai_add_read_only_vector(chai::Module& _m, const string& _name) {
    const auto access_func = [](vector<ValueT>* vec, int index) -> ValueT { return vec->at(index); };
    _m.add(user_type<vector<ValueT*>>(), _name);
    _m.add(fun(access_func), "[]");
}

template<class MType, class... Args> inline
void chai_add_message_type(chai::Module& _m, const string& _name) {
    const auto send_func = [](sq::MessageBus* mbus, Args... args) { mbus->send_message(MType{args...}); };
    _m.add(fun(send_func), "send_" + _name);
}

}
