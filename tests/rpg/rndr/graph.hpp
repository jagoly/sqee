#pragma once
#include <sqee/forward.hpp>

#include <list>

namespace sqt {
class MainCamera;
namespace wcoe { class World; class Cell;
                 class ModelStatic;
                 class ModelSkelly;
                 class SpotLight;
                 class PointLight;
                 class Reflector; }

namespace rndr {

struct MetaModelStatic;
struct MetaModelSkelly;
struct MetaSpotLight;
struct MetaPointLight;
struct MetaReflector;

using ModelStaticList = std::list<pair<MetaModelStatic, weak_ptr<wcoe::ModelStatic>>>;
using ModelSkellyList = std::list<pair<MetaModelSkelly, weak_ptr<wcoe::ModelSkelly>>>;
using SpotLightList   = std::list<pair<MetaSpotLight,   weak_ptr<wcoe::SpotLight>>>;
using PointLightList  = std::list<pair<MetaPointLight,  weak_ptr<wcoe::PointLight>>>;
using ReflectorList   = std::list<pair<MetaReflector,   weak_ptr<wcoe::Reflector>>>;

struct MetaModelStatic { bool cullCam; };
struct MetaModelSkelly { bool cullCam; };
struct MetaSpotLight   { bool cullCam; };
struct MetaPointLight  { bool cullCam; };
struct MetaReflector   { bool cullCam; };

using ModelStaticList = std::list<pair<MetaModelStatic, weak_ptr<wcoe::ModelStatic>>>;
using ModelSkellyList = std::list<pair<MetaModelSkelly, weak_ptr<wcoe::ModelSkelly>>>;
using SpotLightList   = std::list<pair<MetaSpotLight,   weak_ptr<wcoe::SpotLight>>>;
using PointLightList  = std::list<pair<MetaPointLight,  weak_ptr<wcoe::PointLight>>>;
using ReflectorList   = std::list<pair<MetaReflector,   weak_ptr<wcoe::Reflector>>>;

class Graph : NonCopyable {
public:
    void update();
    void update_settings();
    void update_from_world();

    void update_render_metadata();

    void render_shadows_sky_A();
    void render_shadows_sky_B();
    void render_shadows_spot();
    void render_shadows_point();
    void render_models(bool _reflect);
    void render_rflcts(bool _reflect);
    void render_skybox(bool _reflect);
    void render_ambient(bool _reflect);
    void render_skylight(bool _reflect);
    void render_spotlights(bool _reflect);
    void render_pointlights(bool _reflect);
    void render_reflections();

    MainCamera* camera = nullptr; wcoe::World* world = nullptr;
    std::list<pair<MetaModelStatic, weak_ptr<wcoe::ModelStatic>>> modelStaticList;
    std::list<pair<MetaModelSkelly, weak_ptr<wcoe::ModelSkelly>>> modelSkellyList;
    std::list<pair<MetaSpotLight, weak_ptr<wcoe::SpotLight>>> spotLightList;
    std::list<pair<MetaPointLight, weak_ptr<wcoe::PointLight>>> pointLightList;
    std::list<pair<MetaReflector, weak_ptr<wcoe::Reflector>>> reflectorList;

    struct {
        sq::Shader* gnrc_screen = nullptr;
        sq::Shader* modl_static = nullptr;
        sq::Shader* modl_skelly = nullptr;
        sq::Shader* modl_stencil = nullptr;
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

    sq::SettingMap* settings = nullptr;
    sq::Pipeline* pipeline = nullptr;

private:
    sq::Frustum reflFrus;
};

}}
