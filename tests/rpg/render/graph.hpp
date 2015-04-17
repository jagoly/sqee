#pragma once
#include <sqee/forward.hpp>

#include <list>

namespace sqt {

class MainCamera;

class Graph : NonCopyable {
public:
    Graph();

    void update();
    void render_models();
    void render_shadows_sky();
    void render_shadows_spot();
    void render_shadows_point();

    std::list<weak_ptr<sq::ModelStatic>> staticList;
    std::list<weak_ptr<sq::ModelSkelly>> skellyList;
    std::list<weak_ptr<sq::SkyLight>> skyLightList;
    std::list<weak_ptr<sq::SpotLight>> spotLightList;
    std::list<weak_ptr<sq::PointLight>> pointLightList;

    struct {
        sq::Shader* modl_static = nullptr;
        sq::Shader* modl_skelly = nullptr;
        sq::Shader* shad_static = nullptr;
        sq::Shader* shad_skelly = nullptr;
        sq::Shader* shad_static_punch = nullptr;
        sq::Shader* shad_skelly_punch = nullptr;

        sq::Shader* cube_static = nullptr;
        sq::Shader* cube_skelly = nullptr;
    } VS;

    struct {
        sq::Shader* modl_write = nullptr;
        sq::Shader* shds_ambient = nullptr;
        sq::Shader* shds_skylight = nullptr;
        sq::Shader* shds_spotlight = nullptr;
        sq::Shader* shds_pointlight = nullptr;
        sq::Shader* shad_punch = nullptr;

        sq::Shader* cube_frag = nullptr;
    } FS;

    sq::Pipeline* pipeline = nullptr;
    MainCamera* camera = nullptr;
};

}
