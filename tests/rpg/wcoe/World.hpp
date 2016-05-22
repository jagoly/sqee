#pragma once

#include <sqee/builtins.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/ecs/Entity.hpp>

namespace sq { class Camera; }
namespace reactphysics3d { class DynamicsWorld; }

namespace sqt {

class SkyBox; class Ambient; class SkyLight;

class World final : sq::NonCopyable {
public:
    World(sq::MessageBus& _messageBus, const sq::Settings& _settings, const sq::Camera& _camera);

    ~World();

    void configure();

    void update();

    void tick();

    sq::Entity root;

    unique_ptr<SkyBox> skybox;
    unique_ptr<Ambient> ambient;
    unique_ptr<SkyLight> skylight;

    sq::MessageBus& messageBus;
    const sq::Settings& settings;
    const sq::Camera& camera;

    double accum = 0.0;

    unique_ptr<rp3d::DynamicsWorld> physWorld;

private:
    void update_entity(sq::Entity* _e);
    void tick_entity(sq::Entity* _e);

    template<class T, sq::ecs::if_Configure<T>...> void configure_component(T* _c, sq::Entity* _e);
    template<class T, sq::ecs::if_Refresh<T>...> void refresh_component(T* _c, sq::Entity* _e);
    template<class T, sq::ecs::if_Update<T>...> void update_component(T* _c, sq::Entity* _e);
    template<class T, sq::ecs::if_Tick<T>...> void tick_component(T* _c, sq::Entity* _e);

    template<class T, sq::ecs::if_not_Configure<T>...> void configure_wrapper(T* _c, sq::Entity* _e) {}
    template<class T, sq::ecs::if_not_Refresh<T>...> void refresh_wrapper(T* _c, sq::Entity* _e) {}
    template<class T, sq::ecs::if_not_Update<T>...> void update_wrapper(T* _c, sq::Entity* _e) {}

    template<class T, sq::ecs::if_Configure<T>...> void configure_wrapper(T* _c, sq::Entity* _e) {
        if (_c->needsConfigure == true) _c->needsConfigure = false, configure_component(_c, _e); }
    template<class T, sq::ecs::if_Refresh<T>...> void refresh_wrapper(T* _c, sq::Entity* _e) {
        if (_c->needsRefresh == true) _c->needsRefresh = false, refresh_component(_c, _e); }
    template<class T, sq::ecs::if_Update<T>...> void update_wrapper(T* _c, sq::Entity* _e) {
        update_component(_c, _e); }
};

}
