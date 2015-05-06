#pragma once
#include <sqee/forward.hpp>

#include <list>

namespace sqt {
class MainCamera;
namespace wcoe { class World; class Cell;
                 class ModelStatic; class ModelSkelly;
                 class SpotLight; class PointLight; }
namespace rndr {

class Graph : NonCopyable {
public:
    void update();
    void update_settings();
    void update_from_world();

    void render_models();
    void render_shadows_sky();
    void render_shadows_spot();
    void render_shadows_point();
    void render_skybox();
    void render_ambient();
    void render_skylight();
    void render_spotlights();
    void render_pointlights();

    MainCamera* camera = nullptr;
    wcoe::World* world = nullptr;
    std::list<weak_ptr<wcoe::ModelStatic>> modelStaticList;
    std::list<weak_ptr<wcoe::ModelSkelly>> modelSkellyList;
    std::list<weak_ptr<wcoe::SpotLight>> spotLightList;
    std::list<weak_ptr<wcoe::PointLight>> pointLightList;

    struct {
        sq::Shader* gnrc_quad = nullptr;
        sq::Shader* modl_static = nullptr;
        sq::Shader* modl_skelly = nullptr;
        sq::Shader* shad_static = nullptr;
        sq::Shader* shad_skelly = nullptr;
        sq::Shader* shds_skybox = nullptr;
    } VS;

    struct {
        sq::Shader* modl_write = nullptr;
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
    } FS;

    struct {
        sq::Framebuffer* defr = nullptr;
        sq::Framebuffer* hdr = nullptr;
    } FB;

    sq::SettingMap* settings = nullptr;
    sq::Pipeline* pipeline = nullptr;
};

}}
