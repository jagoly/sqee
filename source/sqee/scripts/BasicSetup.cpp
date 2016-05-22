#include <chaiscript/dispatchkit/dispatchkit.hpp>
#include <chaiscript/utility/utility.hpp>

#include <sqee/redist/tinyformat.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/misc/StringCast.hpp>
#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/scripts/BasicSetup.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/ecs/Component.hpp>
#include <sqee/ecs/Entity.hpp>

using chai::fun;
using chai::user_type;
using chai::base_class;
using chai::constructor;
using chai::type_conversion;
using chai::utility::add_class;
using chai::Boxed_Value;
using chai::boxed_cast;
using namespace sq;


void sq::cs_setup_app(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Application>(*m, "Application", {}, {
        {fun(&Application::configure), "configure"},
        {fun(&Application::settings), "settings"},
        {fun(&Application::overlay), "overlay"},
        {fun(&Application::console), "console"},
        {fun(&Application::quit), "quit"}
    });

    add_class<Settings>(*m, "Settings", {}, {
        {fun(&Settings::cs_mod), "mod"},
        {fun(&Settings::cs_get), "get"}
    });

    add_class<DebugOverlay>(*m, "DebugOverlay", {}, {
    });

    add_class<ChaiConsole>(*m, "ChaiConsole", {}, {
        {fun(&ChaiConsole::cs_print), "print"},
        {fun(&ChaiConsole::cs_clear), "clear"},
        {fun(&ChaiConsole::cs_history), "history"}
    });

    _cs.add(m);
}


void sq::cs_setup_render(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Camera>(*m, "Camera", {}, {
        {fun(&Camera::pos), "position"},
        {fun(&Camera::dir), "direction"}
    });

    _cs.add(m);
}


void sq::cs_setup_ecs(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Component>(*m, "Component", {}, {});

    add_class<Entity>(*m, "Entity", {}, {
        {fun(&Entity::add_child), "add_child"},
        {fun(&Entity::remove_child), "remove_child"},
        {fun<Entity* (Entity::*)()>(&Entity::get_parent), "get_parent"},
        {fun<Entity* (Entity::*)(const string&)>(&Entity::get_child), "get_child"},
        {fun<vector<unique_ptr<Entity>>& (Entity::*)()>(&Entity::get_children), "get_children"},
        {fun(&Entity::mark_configure_all), "mark_configure_all"},
        {fun(&Entity::mark_refresh_all), "mark_refresh_all"},
        {fun(&Entity::get_attr), "get"}, {fun(&Entity::erase_attr), "erase"},
        {fun<decltype(&Entity::operator[])>(&Entity::operator[]), "[]"}
    });

    m->add(user_type<vector<unique_ptr<Entity>>>(), "Vector<Entity>");

    _cs.add(m);
}


template<class T> vector<T> convert_vector(const vector<Boxed_Value>& vec) {
    vector<T> rv; for (const auto& bv : vec) rv.emplace_back(boxed_cast<T>(bv)); return rv;
}

template<int S, class T, class VecST, std::enable_if_t<S == 2>...>
inline void setup_VecST_sized(chai::Module& _m, const string& _name) {
    add_class<VecST>(_m, _name, { constructor<VecST(T, T)>() }, {
        {fun(&VecST::x), "x"}, {fun(&VecST::y), "y"} });
}

template<int S, class T, class VecST, std::enable_if_t<S == 3>...>
inline void setup_VecST_sized(chai::Module& _m, const string& _name) {
    add_class<VecST>(_m, _name, { constructor<VecST(T, T, T)>() }, {
        {fun(&VecST::x), "x"}, {fun(&VecST::y), "y"}, {fun(&VecST::z), "z"} });
}

template<int S, class T, class VecST, std::enable_if_t<S == 4>...>
inline void setup_VecST_sized(chai::Module& _m, const string& _name) {
    add_class<VecST>(_m, _name, { constructor<VecST(T, T, T, T)>() }, {
        {fun(&VecST::x), "x"}, {fun(&VecST::y), "y"}, {fun(&VecST::z), "z"}, {fun(&VecST::w), "w"} });
}

template<int S, class T, class VecST = Vector<S, T>>
inline void setup_VecST(chai::Module& _m, const string& _name) {
    setup_VecST_sized<S, T, VecST>(_m, _name);
    _m.add(constructor<VecST(const VecST&)>(), _name);
    _m.add(fun(&VecST::operator=), "=");

    _m.add(user_type<vector<VecST>>(), "Vector<"+_name+">");
    _m.add(type_conversion<vector<Boxed_Value>, vector<VecST>>(&convert_vector<VecST>));
    _m.add(fun<vector<VecST>&(vector<VecST>::*)(const vector<VecST>&)>(&vector<VecST>::operator=), "=");
    _m.add(type_conversion<VecST, string>(&chai_string<VecST>));

    _m.add(fun<VecST(VecST,VecST)>(operator+), "+");
    _m.add(fun<VecST(VecST,VecST)>(operator-), "-");
    _m.add(fun<VecST(VecST,VecST)>(operator*), "*");
    _m.add(fun<VecST(VecST,VecST)>(operator/), "/");
    _m.add(fun<VecST(VecST,T)>(operator+), "+");
    _m.add(fun<VecST(VecST,T)>(operator-), "-");
    _m.add(fun<VecST(VecST,T)>(operator*), "*");
    _m.add(fun<VecST(VecST,T)>(operator/), "/");
}

void sq::cs_setup_maths(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    setup_VecST<2, int>(*m, "Vec2I");
    setup_VecST<3, int>(*m, "Vec3I");
    setup_VecST<4, int>(*m, "Vec4I");
    setup_VecST<2, uint>(*m, "Vec2U");
    setup_VecST<3, uint>(*m, "Vec3U");
    setup_VecST<4, uint>(*m, "Vec4U");
    setup_VecST<2, float>(*m, "Vec2F");
    setup_VecST<3, float>(*m, "Vec3F");
    setup_VecST<4, float>(*m, "Vec4F");

    add_class<QuatF>(*m, "QuatF", { constructor<QuatF(const QuatF&)>(),
        constructor<QuatF(float, float, float, float)>(), constructor<QuatF(float, float, float)>() }, {
        {fun(&QuatF::operator=), "="}, {fun(&QuatF::x), "x"}, {fun(&QuatF::y), "y"}, {fun(&QuatF::z), "z"}, {fun(&QuatF::w), "w"}
    });

    m->add(user_type<vector<int>>(), "Vector<int>");
    m->add(user_type<vector<uint>>(), "Vector<uint>");
    m->add(user_type<vector<float>>(), "Vector<float>");
    m->add(user_type<vector<QuatF>>(), "Vector<QuatF>");
    m->add(type_conversion<int, string>(&chai_string<int>));
    m->add(type_conversion<uint, string>(&chai_string<uint>));
    m->add(type_conversion<float, string>(&chai_string<float>));
    m->add(type_conversion<QuatF, string>(&chai_string<QuatF>));
    m->add(type_conversion<vector<Boxed_Value>, vector<int>>(&convert_vector<int>));
    m->add(type_conversion<vector<Boxed_Value>, vector<uint>>(&convert_vector<uint>));
    m->add(type_conversion<vector<Boxed_Value>, vector<float>>(&convert_vector<float>));
    m->add(type_conversion<vector<Boxed_Value>, vector<QuatF>>(&convert_vector<QuatF>));
    m->add(fun<vector<int>&(vector<int>::*)(const vector<int>&)>(&vector<int>::operator=), "=");
    m->add(fun<vector<uint>&(vector<uint>::*)(const vector<uint>&)>(&vector<uint>::operator=), "=");
    m->add(fun<vector<float>&(vector<float>::*)(const vector<float>&)>(&vector<float>::operator=), "=");
    m->add(fun<vector<QuatF>&(vector<QuatF>::*)(const vector<QuatF>&)>(&vector<QuatF>::operator=), "=");

    m->add(fun<Vec2F (*)(Vec2F)>(&maths::normalize), "normalize");
    m->add(fun<Vec3F (*)(Vec3F)>(&maths::normalize), "normalize");
    m->add(fun<Vec4F (*)(Vec4F)>(&maths::normalize), "normalize");
    m->add(fun<QuatF (*)(QuatF)>(&maths::normalize), "normalize");

    _cs.add(m);
}
