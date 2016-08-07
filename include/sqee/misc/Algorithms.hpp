#pragma once

#include <sqee/setup.hpp>

namespace sq {
namespace algo {

template<class IterT, class ValueT> inline
IterT find(IterT _begin, IterT _end, const ValueT& _value) {
    while (_begin != _end && *_begin != _value) ++_begin; return _begin;
}

template<class IterT, class ValueT> inline
IterT find_first(IterT _begin, IterT _end, const ValueT& _value) {
    while (_begin != _end && _begin->first != _value) ++_begin; return _begin;
}

template<class IterT, class PredicateT> inline
IterT find_if(IterT _begin, IterT _end, const PredicateT& _pred) {
    while (_begin != _end && !_pred(*_begin)) ++_begin; return _begin;
}

template<class ContainerT, class ValueT> inline
auto find(const ContainerT& _container, const ValueT& _value) {
    return sq::algo::find(_container.begin(), _container.end(), _value);
}

template<class ContainerT, class ValueT> inline
auto find_first(const ContainerT& _container, const ValueT& _value) {
    return sq::algo::find_first(_container.begin(), _container.end(), _value);
}

template<class ContainerT, class PredicateT> inline
auto find_if(const ContainerT& _container, const PredicateT& _pred) {
    return sq::algo::find_if(_container.begin(), _container.end(), _pred);
}

}}
