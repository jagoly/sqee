#pragma once
#include <sqee/forward.hpp>

#include <list>

namespace sqt {
class MainCamera;
namespace wcoe { class ModelStatic; class ModelSkelly;
                 class LightSky; class LightSpot; class LightPoint; }
namespace rndr {

class Graph : NonCopyable {
public:
    void update();
    void update_settings();
    void render_models();
    void render_shadows_sky();
    void render_shadows_spot();
    void render_shadows_point();
    void render_lights_sky();
    void render_lights_spot();
    void render_lights_point();

    std::list<weak_ptr<wcoe::ModelStatic>> modelStaticList;
    std::list<weak_ptr<wcoe::ModelSkelly>> modelSkellyList;
    std::list<weak_ptr<wcoe::LightSky>> lightSkyList;
    std::list<weak_ptr<wcoe::LightSpot>> lightSpotList;
    std::list<weak_ptr<wcoe::LightPoint>> lightPointList;

    struct {
        sq::Shader* modl_static = nullptr;
        sq::Shader* modl_skelly = nullptr;
        sq::Shader* shad_static = nullptr;
        sq::Shader* shad_skelly = nullptr;
    } VS;

    struct {
        sq::Shader* modl_write = nullptr;
        sq::Shader* shad_punch = nullptr;
        sq::Shader* shds_ambient = nullptr;
        sq::Shader* shds_skylight = nullptr;
        sq::Shader* shds_spotlight = nullptr;
        sq::Shader* shds_pointlight = nullptr;
    } FS;

    MainCamera* camera = nullptr;
    sq::SettingMap* settings = nullptr;
    sq::Pipeline* pipeline = nullptr;
};

}}
