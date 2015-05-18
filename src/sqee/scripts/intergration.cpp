#include <chaiscript/utility/utility.hpp>

#include "sqee/app/application.hpp"
#include "sqee/events/handler.hpp"
#include "sqee/redist/gl_ext_3_3.hpp"
#include "sqee/render/animation.hpp"
#include "sqee/render/mesh.hpp"
#include "sqee/render/skeleton.hpp"
#include "sqee/render/skin.hpp"
#include "sqee/scenes/scene.hpp"
#include "sqee/sounds/soundmanager.hpp"
#include "sqee/scripts/scene.hpp"
#include "sqee/scripts/intergration.hpp"

using namespace sq;

using chai::constructor;
using chai::fun;
using chai::type_conversion;
using chai::utility::add_class;

void sq::cs_setup_glm(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<vec2>(*m, "vec2", {
            constructor<vec2(float, float)>(), constructor<vec2(const vec2&)>()
        }, {{fun<vec2& (vec2::*)(const vec2&)>(&vec2::operator=), "="},
            {fun(&vec2::x), "x"}, {fun(&vec2::y), "y"}}
    );
    add_class<vec3>(*m, "vec3", {
            constructor<vec3(float, float, float)>(), constructor<vec3(const vec3&)>()
        }, {{fun<vec3& (vec3::*)(const vec3&)>(&vec3::operator=), "="},
            {fun(&vec3::x), "x"}, {fun(&vec3::y), "y"}, {fun(&vec3::z), "z"}}
    );
    add_class<vec4>(*m, "vec4", {
            constructor<vec4(float, float, float, float)>(), constructor<vec4(const vec4&)>()
        }, {{fun<vec4& (vec4::*)(const vec4&)>(&vec4::operator=), "="},
            {fun(&vec4::x), "x"}, {fun(&vec4::y), "y"}, {fun(&vec4::z), "z"}, {fun(&vec4::w), "w"}}
    );

    add_class<ivec2>(*m, "ivec2", {
            constructor<ivec2(int, int)>(), constructor<ivec2(const ivec2&)>()
        }, {{fun<ivec2& (ivec2::*)(const ivec2&)>(&ivec2::operator=), "="},
            {fun(&ivec2::x), "x"}, {fun(&ivec2::y), "y"}}
    );
    add_class<ivec3>(*m, "ivec3", {
            constructor<ivec3(int, int, int)>(), constructor<ivec3(const ivec3&)>()
        }, {{fun<ivec3& (ivec3::*)(const ivec3&)>(&ivec3::operator=), "="},
            {fun(&ivec3::x), "x"}, {fun(&ivec3::y), "y"}, {fun(&ivec3::z), "z"}}
    );
    add_class<ivec4>(*m, "ivec4", {
            constructor<ivec4(int, int, int, int)>(), constructor<ivec4(const ivec4&)>()
        }, {{fun<ivec4& (ivec4::*)(const ivec4&)>(&ivec4::operator=), "="},
            {fun(&ivec4::x), "x"}, {fun(&ivec4::y), "y"}, {fun(&ivec4::z), "z"}, {fun(&ivec4::w), "w"}}
    );

    add_class<uvec2>(*m, "uvec2", {
            constructor<uvec2(uint, uint)>(), constructor<uvec2(const uvec2&)>()
        }, {{fun<uvec2& (uvec2::*)(const uvec2&)>(&uvec2::operator=), "="},
            {fun(&uvec2::x), "x"}, {fun(&uvec2::y), "y"}}
    );
    add_class<uvec3>(*m, "uvec3", {
            constructor<uvec3(uint, uint, uint)>(), constructor<uvec3(const uvec3&)>()
        }, {{fun<uvec3& (uvec3::*)(const uvec3&)>(&uvec3::operator=), "="},
            {fun(&uvec3::x), "x"}, {fun(&uvec3::y), "y"}, {fun(&uvec3::z), "z"}}
    );
    add_class<uvec4>(*m, "uvec4", {
            constructor<uvec4(uint, uint, uint, uint)>(), constructor<uvec4(const uvec4&)>()
        }, {{fun<uvec4& (uvec4::*)(const uvec4&)>(&uvec4::operator=), "="},
            {fun(&uvec4::x), "x"}, {fun(&uvec4::y), "y"}, {fun(&uvec4::z), "z"}, {fun(&uvec4::w), "w"}}
    );

    _cs.add(m);
}

void sq::cs_setup_application(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Application>(*m, "Application", {
        }, {{fun(&Application::quit), "quit"}});

    _cs.add(m);
}

void sq::cs_setup_settings(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<SettingMap>(*m, "SettingMap", {
        }, {{fun(&SettingMap::add<int>), "addI"},
            {fun(&SettingMap::mod<int>), "modI"},
            {fun(&SettingMap::crnt<int>), "crntI"},
            {fun(&SettingMap::next<int>), "nextI"},
            {fun(&SettingMap::add<bool>), "addB"},
            {fun(&SettingMap::mod<bool>), "modB"},
            {fun(&SettingMap::crnt<bool>), "crntB"},
            {fun(&SettingMap::next<bool>), "nextB"},
            {fun(&SettingMap::add<float>), "addF"},
            {fun(&SettingMap::mod<float>), "modF"},
            {fun(&SettingMap::crnt<float>), "crntF"},
            {fun(&SettingMap::next<float>), "nextF"},
            {fun(&SettingMap::add<string>), "addS"},
            {fun(&SettingMap::mod<string>), "modS"},
            {fun(&SettingMap::crnt<string>), "crntS"},
            {fun(&SettingMap::next<string>), "nextS"},
            {fun(&SettingMap::apply), "apply"}});

    _cs.add(m);
}

void sq::cs_setup_console(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<SceneConsole>(*m, "Console", {
        }, {{fun(&SceneConsole::cs_print), "print"},
            {fun(&SceneConsole::cs_clear), "clear"},
            {fun(&SceneConsole::cs_history), "history"}});

    _cs.add(m);
}
