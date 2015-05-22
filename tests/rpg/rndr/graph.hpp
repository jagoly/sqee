#pragma once
#include <sqee/forward.hpp>

#include <list>

namespace sqt {
class MainCamera;
namespace wcoe { class World; class Cell;
                 class ModelStatic; class ModelSkelly;
                 class PointLight; class SpotLight;
                 class Reflector; class Emitter;
                 class Liquid; class Decal; }

namespace rndr {

using ModelStaticList = std::list<weak_ptr<wcoe::ModelStatic>>;
using ModelSkellyList = std::list<weak_ptr<wcoe::ModelSkelly>>;
using PointLightList  = std::list<weak_ptr<wcoe::PointLight>>;
using SpotLightList   = std::list<weak_ptr<wcoe::SpotLight>>;
using ReflectorList   = std::list<weak_ptr<wcoe::Reflector>>;
using EmitterList     = std::list<weak_ptr<wcoe::Emitter>>;
using LiquidList      = std::list<weak_ptr<wcoe::Liquid>>;
using DecalList       = std::list<weak_ptr<wcoe::Decal>>;

class Graph : NonCopyable {
public:
    Graph(MainCamera* _camera, sq::SettingsMaps* _settings);

    void update();
    void reload_lists();
    void update_settings();

    void render_shadows_sky_A();
    void render_shadows_sky_B();
    void render_shadows_spot();
    void render_shadows_point();
    void render_mstatics(bool _reflect);
    void render_mskellys(bool _reflect);
    void render_reflects(bool _reflect);
    void render_skybox(bool _reflect);
    void render_ambient(bool _reflect);
    void render_skylight(bool _reflect);
    void render_spotlights(bool _reflect);
    void render_pointlights(bool _reflect);
    void render_reflections();
    void render_decals();

    wcoe::World* world = nullptr;
    ModelStaticList modelStaticList;
    ModelSkellyList modelSkellyList;
    PointLightList  pointLightList;
    SpotLightList   spotLightList;
    ReflectorList   reflectorList;
    EmitterList     emitterList;
    LiquidList      liquidList;
    DecalList       decalList;

    struct {
        sq::Shader* gnrc_screen = nullptr;
        sq::Shader* modl_static = nullptr;
        sq::Shader* modl_skelly = nullptr;
        sq::Shader* modl_stencil = nullptr;
        sq::Shader* modl_decal = nullptr;
        sq::Shader* modl_static_refl = nullptr;
        sq::Shader* modl_skelly_refl = nullptr;
        sq::Shader* modl_stencil_refl = nullptr;
        sq::Shader* shad_static = nullptr;
        sq::Shader* shad_skelly = nullptr;
        sq::Shader* shds_skybox = nullptr;
        sq::Shader* shds_skybox_refl = nullptr;
        sq::Shader* shds_reflector = nullptr;
    } VS;

    struct {
        sq::Shader* gnrc_fillwith = nullptr;
        sq::Shader* gnrc_lumalpha = nullptr;
        sq::Shader* gnrc_passthru = nullptr;
        sq::Shader* modl_write = nullptr;
        sq::Shader* modl_decal = nullptr;
        sq::Shader* modl_write_refl = nullptr;
        sq::Shader* shad_punch = nullptr;
        sq::Shader* shds_skybox = nullptr;
        sq::Shader* shds_ambient = nullptr;
        sq::Shader* shds_skylight = nullptr;
        sq::Shader* shds_spot_none = nullptr;
        sq::Shader* shds_spot_shad = nullptr;
        sq::Shader* shds_spot_spec = nullptr;
        sq::Shader* shds_spot_both = nullptr;
        sq::Shader* shds_point_none = nullptr;
        sq::Shader* shds_point_shad = nullptr;
        sq::Shader* shds_point_spec = nullptr;
        sq::Shader* shds_point_both = nullptr;
        sq::Shader* shds_skybox_refl = nullptr;
        sq::Shader* shds_ambient_refl = nullptr;
        sq::Shader* shds_skylight_refl = nullptr;
        sq::Shader* shds_spot_none_refl = nullptr;
        sq::Shader* shds_spot_shad_refl = nullptr;
        sq::Shader* shds_point_none_refl = nullptr;
        sq::Shader* shds_point_shad_refl = nullptr;
        sq::Shader* shds_reflector = nullptr;
    } FS;

    struct {
        sq::Texture2D* depth = nullptr;
        sq::Texture2D* diff = nullptr;
        sq::Texture2D* surf = nullptr;
        sq::Texture2D* norm = nullptr;
        sq::Texture2D* spec = nullptr;
        sq::Texture2D* hdr = nullptr;
    } TX;

    struct {
        sq::Framebuffer* defr = nullptr;
        sq::Framebuffer* hdr = nullptr;
    } FB;

    struct {
        int shadMult;
        bool shadFltr;
        float viewDist;
        uvec2 fullSize;
        uvec2 halfSize;
        uvec2 qterSize;
        vec2 fPixSize;
        vec2 hPixSize;
        vec2 qPixSize;
    } INFO;

    const MainCamera* const camera;
    sq::SettingsMaps* const settings;
    sq::Pipeline* pipeline = nullptr;

//private:
    sq::Frustum reflFrus;
};

}}
