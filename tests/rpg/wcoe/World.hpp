#pragma once

#include <sqee/builtins.hpp>
#include <sqee/ecs/Entity.hpp>

#include "../RpgOptions.hpp"
#include "../components/Helpers.hpp"

// Forward Declarations /////
namespace sqt { class Camera; class Ambient; class SkyBox; class SkyLight; }
namespace reactphysics3d { class DynamicsWorld; }

namespace sqt {

class World final : sq::NonCopyable {
public:
    World(RpgOptions& _options);

    ~World();

    void update_options();
    void tick(); void update();

    sq::Entity<World>& get_RootEntity();
    rp3d::DynamicsWorld& get_PhysicsWorld();
    const sq::Entity<World>& get_RootEntity() const;
    const rp3d::DynamicsWorld& get_PhysicsWorld() const;

    Camera& get_Camera();
    Ambient& get_Ambient();
    const Camera& get_Camera() const;
    const Ambient& get_Ambient() const;

    SkyBox& get_SkyBox();
    SkyLight& get_SkyLight();
    const SkyBox& get_SkyBox() const;
    const SkyLight& get_SkyLight() const;

    bool check_SkyBox() const;
    bool check_SkyLight() const;
    void enable_SkyBox(bool _enable);
    void enable_SkyLight(bool _enable);

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

    friend class SkyLight;

    const RpgOptions& options;

    struct Impl;
    friend struct Impl;
    unique_ptr<Impl> impl;
};

}
