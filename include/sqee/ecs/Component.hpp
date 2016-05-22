#pragma once

#include <sqee/builtins.hpp>

// Forward Declarations /////
namespace sq { class Entity; }

namespace sq {

class Component : sq::NonCopyable {
public:
    virtual ~Component() = default;
};

namespace ecs {

struct TagConfigure { bool needsConfigure = true; };
struct TagRefresh { bool needsRefresh = true; };
struct TagUpdate {}; struct TagTick {};

template<class T> using if_Component = std::enable_if_t<std::is_base_of<Component, T>::value>;

template<class T> using if_Configure = std::enable_if_t<std::is_base_of<TagConfigure, T>::value>;
template<class T> using if_Refresh = std::enable_if_t<std::is_base_of<TagRefresh, T>::value>;
template<class T> using if_Update = std::enable_if_t<std::is_base_of<TagUpdate, T>::value>;
template<class T> using if_Tick = std::enable_if_t<std::is_base_of<TagTick, T>::value>;

template<class T> using if_not_Configure = std::enable_if_t<!std::is_base_of<TagConfigure, T>::value>;
template<class T> using if_not_Refresh = std::enable_if_t<!std::is_base_of<TagRefresh, T>::value>;
template<class T> using if_not_Update = std::enable_if_t<!std::is_base_of<TagUpdate, T>::value>;
template<class T> using if_not_Tick = std::enable_if_t<!std::is_base_of<TagTick, T>::value>;

}}
