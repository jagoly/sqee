#pragma once

#include <sqee/forward.hpp>
#include <sqee/ecs/Component.hpp>

namespace sqt {

class World;

using EntityRPG = sq::Entity<World>;
using ComponentRPG = sq::Component<World>;

class AnimatorComponent;
class DynamicBodyComponent;
class TransformComponent;
class StaticBodyComponent;
class ModelComponent;
class DecalComponent;
class SpotLightComponent;
class PointLightComponent;
class ReflectComponent;

namespace ecs {

struct TagCleanUp {};
struct TagConfigure {};
struct TagRefresh {};
struct TagUpdate {};
struct TagTick {};

template<class T> using if_CleanUp = std::enable_if_t<std::is_base_of<TagCleanUp, T>::value>;
template<class T> using if_Configure = std::enable_if_t<std::is_base_of<TagConfigure, T>::value>;
template<class T> using if_Refresh = std::enable_if_t<std::is_base_of<TagRefresh, T>::value>;
template<class T> using if_Update = std::enable_if_t<std::is_base_of<TagUpdate, T>::value>;
template<class T> using if_Tick = std::enable_if_t<std::is_base_of<TagTick, T>::value>;

template<class T> using if_not_CleanUp = std::enable_if_t<!std::is_base_of<TagCleanUp, T>::value>;
template<class T> using if_not_Configure = std::enable_if_t<!std::is_base_of<TagConfigure, T>::value>;
template<class T> using if_not_Refresh = std::enable_if_t<!std::is_base_of<TagRefresh, T>::value>;
template<class T> using if_not_Update = std::enable_if_t<!std::is_base_of<TagUpdate, T>::value>;
template<class T> using if_not_Tick = std::enable_if_t<!std::is_base_of<TagTick, T>::value>;

}}
