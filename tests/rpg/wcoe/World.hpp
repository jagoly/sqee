#pragma once

#include <sqee/builtins.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/ecs/Entity.hpp>

#include "../components/Helpers.hpp"

namespace reactphysics3d { class DynamicsWorld; }

namespace sqt {

class Camera; class SkyBox;
class Ambient; class SkyLight;

class World final : sq::NonCopyable {
public:
    World(sq::MessageBus& _messageBus,
          const sq::Settings& _settings);

    ~World();

    void configure();
    void update();
    void tick();

    unique_ptr<rp3d::DynamicsWorld> physWorld;

    unique_ptr<Camera> camera;
    unique_ptr<SkyBox> skybox;
    unique_ptr<Ambient> ambient;
    unique_ptr<SkyLight> skylight;

    sq::Entity<World> root;

    float tickPercent = 0.f;

    template<class T> void mark_configure_component(const EntityRPG* _e, bool _recursive);
    template<class T> void mark_refresh_component(const EntityRPG* _e, bool _recursive);
    void mark_configure_all_components(const EntityRPG* _e, bool _recursive);
    void mark_refresh_all_components(const EntityRPG* _e, bool _recursive);

    template<class T, ecs::if_not_CleanUp<T>...> void clean_up_component(T* _c, EntityRPG* _e) {}
    template<class T, ecs::if_CleanUp<T>...> void clean_up_component(T* _c, EntityRPG* _e);
    void clean_up_entity(EntityRPG* _e);

private:
    template<class T, ecs::if_Configure<T>...> void configure_component(T* _c, EntityRPG* _e);
    template<class T, ecs::if_Refresh<T>...> void refresh_component(T* _c, EntityRPG* _e);
    template<class T, ecs::if_Update<T>...> void update_component(T* _c, EntityRPG* _e);
    template<class T, ecs::if_Tick<T>...> void tick_component(T* _c, EntityRPG* _e);

    sq::MessageBus& messageBus;
    const sq::Settings& settings;

    friend class SkyLight;

    struct Impl;
    friend struct Impl;
    unique_ptr<Impl> impl;
};

}
