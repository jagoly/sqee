#include "app/application.hpp"
#include "events/handler.hpp"
#include "gl/gl_ext_3_3.hpp"
#include "gl/maths.hpp"
#include "models/mesh.hpp"
#include "models/skeleton.hpp"
#include "models/skin.hpp"
#include "scenes/scene.hpp"
#include "sounds/soundmanager.hpp"

#include "scripts/intergration.hpp"

using namespace sq;

using chai::constructor;
using chai::fun;
using chai::type_conversion;
using chai::utility::add_class;

void sq::cs_setup_glm(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<glm::vec2>(*m, "vec2", {
            constructor<glm::vec2(const float&, const float&)>(),
            constructor<glm::vec2(const glm::vec2&)>()
        }, {{fun(&glm::vec2::x), "x"},
            {fun(&glm::vec2::y), "y"}});
    add_class<glm::vec3>(*m, "vec3", {
            constructor<glm::vec3(const float&, const float&, const float&)>(),
            constructor<glm::vec3(const glm::vec3&)>()
        }, {{fun(&glm::vec2::x), "x"},
            {fun(&glm::vec2::y), "y"},
            {fun(&glm::vec3::z), "z"}});
    add_class<glm::vec4>(*m, "vec4", {
            constructor<glm::vec4(const float&, const float&, const float&, const float&)>(),
            constructor<glm::vec4(const glm::vec4&)>()
        }, {{fun(&glm::vec4::x), "x"},
            {fun(&glm::vec4::y), "y"},
            {fun(&glm::vec4::z), "z"},
            {fun(&glm::vec4::w), "w"}});

    add_class<glm::ivec2>(*m, "ivec2", {
            constructor<glm::ivec2(const int&, const int&)>(),
            constructor<glm::ivec2(const glm::ivec2&)>()
        }, {{fun(&glm::ivec2::x), "x"},
            {fun(&glm::ivec2::y), "y"}});
    add_class<glm::ivec3>(*m, "ivec3", {
            constructor<glm::ivec3(const int&, const int&, const int&)>(),
            constructor<glm::ivec3(const glm::ivec3&)>()
        }, {{fun(&glm::ivec2::x), "x"},
            {fun(&glm::ivec2::y), "y"},
            {fun(&glm::ivec3::z), "z"}});
    add_class<glm::ivec4>(*m, "ivec4", {
            constructor<glm::ivec4(const int&, const int&, const int&, const int&)>(),
            constructor<glm::ivec4(const glm::ivec4&)>()
        }, {{fun(&glm::ivec4::x), "x"},
            {fun(&glm::ivec4::y), "y"},
            {fun(&glm::ivec4::z), "z"},
            {fun(&glm::ivec4::w), "w"}});

    add_class<glm::uvec2>(*m, "uvec2", {
            constructor<glm::uvec2(const uint&, const uint&)>(),
            constructor<glm::uvec2(const glm::uvec2&)>()
        }, {{fun(&glm::uvec2::x), "x"},
            {fun(&glm::uvec2::y), "y"}});
    add_class<glm::uvec3>(*m, "uvec3", {
            constructor<glm::uvec3(const uint&, const uint&, const uint&)>(),
            constructor<glm::uvec3(const glm::uvec3&)>()
        }, {{fun(&glm::uvec2::x), "x"},
            {fun(&glm::uvec2::y), "y"},
            {fun(&glm::uvec3::z), "z"}});
    add_class<glm::uvec4>(*m, "uvec4", {
            constructor<glm::uvec4(const uint&, const uint&, const uint&, const uint&)>(),
            constructor<glm::uvec4(const glm::uvec4&)>()
        }, {{fun(&glm::uvec4::x), "x"},
            {fun(&glm::uvec4::y), "y"},
            {fun(&glm::uvec4::z), "z"},
            {fun(&glm::uvec4::w), "w"}});

    _cs.add(m);
}

void sq::cs_setup_application(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Application>(*m, "Application", {
        }, {{fun(&Application::settings), "settings"},
            {fun(&Application::quit), "quit"},
            {fun(&Application::set_size), "set_size"},
            {fun(&Application::get_size), "get_size"}});

    _cs.add(m);
}

void sq::cs_setup_settings(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<SettingValue>(*m, "SettingValue",  {
            constructor<SettingValue(bool)>(),
            constructor<SettingValue(int)>(),
            constructor<SettingValue(float)>(),
            constructor<SettingValue(string)>()
        }, {{fun(&SettingValue::b), "b"},
            {fun(&SettingValue::i), "i"},
            {fun(&SettingValue::f), "f"},
            {fun(&SettingValue::s), "s"}});

    m->add(type_conversion<bool, SettingValue>());
    m->add(type_conversion<int, SettingValue>());
    m->add(type_conversion<float, SettingValue>());
    m->add(type_conversion<string, SettingValue>());

    add_class<SettingMap>(*m, "SettingMap", {
        }, {{fun(&SettingMap::crnt), "crnt"},
            {fun(&SettingMap::next), "next"},
            {fun(&SettingMap::add), "add"},
            {fun(&SettingMap::modify), "modify"},
            {fun(&SettingMap::apply), "apply"}});

    _cs.add(m);
}
