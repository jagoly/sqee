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
        {fun(&Application::refresh), "refresh"},
        {fun(&Application::settings), "settings"},
        {fun(&Application::overlay), "overlay"},
        {fun(&Application::console), "console"},
        {fun(&Application::quit), "quit"} });

    add_class<Settings>(*m, "Settings", {}, {
        {fun(&Settings::cs_mod), "mod"},
        {fun(&Settings::cs_get), "get"} });

    add_class<DebugOverlay>(*m, "DebugOverlay", {}, {
        });

    add_class<ChaiConsole>(*m, "ChaiConsole", {}, {
        {fun(&ChaiConsole::cs_print), "print"},
        {fun(&ChaiConsole::cs_clear), "clear"},
        {fun(&ChaiConsole::cs_history), "history"} });

    _cs.add(m);
}


void sq::cs_setup_render(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Camera>(*m, "Camera", {}, {
        {fun(&Camera::pos), "position"},
        {fun(&Camera::dir), "direction"} });

    _cs.add(m);
}


void sq::cs_setup_ecs(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Component>(*m, "Component", {}, {
        {fun(&Component::mark_for_update), "mark_for_update"} });

    add_class<Entity>(*m, "Entity", {}, {
        {fun(&Entity::set_parent), "set_parent"}, {fun(&Entity::add_child), "add_child"},
        {fun<vector<unique_ptr<Entity>>& (Entity::*)()>(&Entity::get_children), "get_children"},
        {fun<Entity* (Entity::*)()>(&Entity::get_parent), "get_parent"} });

    m->add(user_type<vector<unique_ptr<Component>>>(), "Vector_Component");
    m->add(user_type<vector<unique_ptr<Entity>>>(), "Vector_Entity");

    m->add(type_conversion<unique_ptr<Entity>&, Entity*>([]
          (unique_ptr<Entity>& uptr) { return uptr.get(); }));

    _cs.add(m);
}


template<class T> vector<T> convert_vector(const vector<Boxed_Value>& vec) {
    vector<T> rv; for (const auto& bv : vec) rv.emplace_back(boxed_cast<T>(bv)); return rv;
}


void sq::cs_setup_maths(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Vec2I>(*m, "Vec2I", { constructor<Vec2I(const Vec2I&)>(),
        constructor<Vec2I(int, int)>() }, {
        {fun<Vec2I& (Vec2I::*)(const Vec2I&)>(&Vec2I::operator=), "="},
        {fun(&Vec2I::x), "x"}, {fun(&Vec2I::y), "y"}} );
    add_class<Vec2U>(*m, "Vec2U", { constructor<Vec2U(const Vec2U&)>(),
        constructor<Vec2U(uint, uint)>() }, {
        {fun<Vec2U& (Vec2U::*)(const Vec2U&)>(&Vec2U::operator=), "="},
        {fun(&Vec2U::x), "x"}, {fun(&Vec2U::y), "y"}} );
    add_class<Vec2B>(*m, "Vec2B", { constructor<Vec2B(const Vec2B&)>(),
        constructor<Vec2B(bool, bool)>() }, {
        {fun<Vec2B& (Vec2B::*)(const Vec2B&)>(&Vec2B::operator=), "="},
        {fun(&Vec2B::x), "x"}, {fun(&Vec2B::y), "y"}} );
    add_class<Vec2F>(*m, "Vec2F", { constructor<Vec2F(const Vec2F&)>(),
        constructor<Vec2F(float, float)>() }, {
        {fun<Vec2F& (Vec2F::*)(const Vec2F&)>(&Vec2F::operator=), "="},
        {fun(&Vec2F::x), "x"}, {fun(&Vec2F::y), "y"}} );

    add_class<Vec3I>(*m, "Vec3I", { constructor<Vec3I(const Vec3I&)>(),
        constructor<Vec3I(int, int, int)>() }, {
        {fun<Vec3I& (Vec3I::*)(const Vec3I&)>(&Vec3I::operator=), "="},
        {fun(&Vec3I::x), "x"}, {fun(&Vec3I::y), "y"}, {fun(&Vec3I::z), "z"}} );
    add_class<Vec3U>(*m, "Vec3U", { constructor<Vec3U(const Vec3U&)>(),
        constructor<Vec3U(uint, uint, uint)>() }, {
        {fun<Vec3U& (Vec3U::*)(const Vec3U&)>(&Vec3U::operator=), "="},
        {fun(&Vec3U::x), "x"}, {fun(&Vec3U::y), "y"}, {fun(&Vec3U::z), "z"}} );
    add_class<Vec3B>(*m, "Vec3B", { constructor<Vec3B(const Vec3B&)>(),
        constructor<Vec3B(bool, bool, bool)>() }, {
        {fun<Vec3B& (Vec3B::*)(const Vec3B&)>(&Vec3B::operator=), "="},
        {fun(&Vec3B::x), "x"}, {fun(&Vec3B::y), "y"}, {fun(&Vec3B::z), "z"}} );
    add_class<Vec3F>(*m, "Vec3F", { constructor<Vec3F(const Vec3F&)>(),
        constructor<Vec3F(float, float, float)>() }, {
        {fun<Vec3F& (Vec3F::*)(const Vec3F&)>(&Vec3F::operator=), "="},
        {fun(&Vec3F::x), "x"}, {fun(&Vec3F::y), "y"}, {fun(&Vec3F::z), "z"}} );

    add_class<Vec4I>(*m, "Vec4I", { constructor<Vec4I(const Vec4I&)>(),
        constructor<Vec4I(int, int, int, int)>() }, {
        {fun<Vec4I& (Vec4I::*)(const Vec4I&)>(&Vec4I::operator=), "="},
        {fun(&Vec4I::x), "x"}, {fun(&Vec4I::y), "y"}, {fun(&Vec4I::z), "z"}, {fun(&Vec4I::w), "w"}} );
    add_class<Vec4U>(*m, "Vec4U", { constructor<Vec4U(const Vec4U&)>(),
        constructor<Vec4U(uint, uint, uint, uint)>() }, {
        {fun<Vec4U& (Vec4U::*)(const Vec4U&)>(&Vec4U::operator=), "="},
        {fun(&Vec4U::x), "x"}, {fun(&Vec4U::y), "y"}, {fun(&Vec4U::z), "z"}, {fun(&Vec4U::w), "w"}} );
    add_class<Vec4B>(*m, "Vec4B", { constructor<Vec4B(const Vec4B&)>(),
        constructor<Vec4B(bool, bool, bool, bool)>() }, {
        {fun<Vec4B& (Vec4B::*)(const Vec4B&)>(&Vec4B::operator=), "="},
        {fun(&Vec4B::x), "x"}, {fun(&Vec4B::y), "y"}, {fun(&Vec4B::z), "z"}, {fun(&Vec4B::w), "w"}} );
    add_class<Vec4F>(*m, "Vec4F", { constructor<Vec4F(const Vec4F&)>(),
        constructor<Vec4F(float, float, float, float)>() }, {
        {fun<Vec4F& (Vec4F::*)(const Vec4F&)>(&Vec4F::operator=), "="},
        {fun(&Vec4F::x), "x"}, {fun(&Vec4F::y), "y"}, {fun(&Vec4F::z), "z"}, {fun(&Vec4F::w), "w"}} );

    add_class<QuatF>(*m, "QuatF", { constructor<QuatF(const QuatF&)>(),
        constructor<QuatF(float, float, float, float)>(),
        constructor<QuatF(float, float, float)>() }, {
        {fun<QuatF& (QuatF::*)(const QuatF&)>(&QuatF::operator=), "="},
        {fun(&QuatF::x), "x"}, {fun(&QuatF::y), "y"}, {fun(&QuatF::z), "z"}, {fun(&QuatF::w), "w"}} );

    m->add(user_type<vector<int>>(), "Vector_int");
    m->add(user_type<vector<uint>>(), "Vector_uint");
    m->add(user_type<vector<float>>(), "Vector_float");
    m->add(user_type<vector<Vec2I>>(), "Vector_Vec2I");
    m->add(user_type<vector<Vec3I>>(), "Vector_Vec3I");
    m->add(user_type<vector<Vec4I>>(), "Vector_Vec4I");
    m->add(user_type<vector<Vec2U>>(), "Vector_Vec2U");
    m->add(user_type<vector<Vec3U>>(), "Vector_Vec3U");
    m->add(user_type<vector<Vec4U>>(), "Vector_Vec4U");
    m->add(user_type<vector<Vec2B>>(), "Vector_Vec2B");
    m->add(user_type<vector<Vec3B>>(), "Vector_Vec3B");
    m->add(user_type<vector<Vec4B>>(), "Vector_Vec4B");
    m->add(user_type<vector<Vec2F>>(), "Vector_Vec2F");
    m->add(user_type<vector<Vec3F>>(), "Vector_Vec3F");
    m->add(user_type<vector<Vec4F>>(), "Vector_Vec4F");
    m->add(user_type<vector<QuatF>>(), "Vector_QuatF");
    m->add(type_conversion<int, string>(&chai_string<int>));
    m->add(type_conversion<uint, string>(&chai_string<uint>));
    m->add(type_conversion<float, string>(&chai_string<float>));
    m->add(type_conversion<Vec2I, string>(&chai_string<Vec2I>));
    m->add(type_conversion<Vec3I, string>(&chai_string<Vec3I>));
    m->add(type_conversion<Vec4I, string>(&chai_string<Vec4I>));
    m->add(type_conversion<Vec2U, string>(&chai_string<Vec2U>));
    m->add(type_conversion<Vec3U, string>(&chai_string<Vec3U>));
    m->add(type_conversion<Vec4U, string>(&chai_string<Vec4U>));
    m->add(type_conversion<Vec2B, string>(&chai_string<Vec2B>));
    m->add(type_conversion<Vec3B, string>(&chai_string<Vec3B>));
    m->add(type_conversion<Vec4B, string>(&chai_string<Vec4B>));
    m->add(type_conversion<Vec2F, string>(&chai_string<Vec2F>));
    m->add(type_conversion<Vec3F, string>(&chai_string<Vec3F>));
    m->add(type_conversion<Vec4F, string>(&chai_string<Vec4F>));
    m->add(type_conversion<QuatF, string>(&chai_string<QuatF>));
    m->add(type_conversion<vector<Boxed_Value>, vector<int>>(&convert_vector<int>));
    m->add(type_conversion<vector<Boxed_Value>, vector<uint>>(&convert_vector<uint>));
    m->add(type_conversion<vector<Boxed_Value>, vector<float>>(&convert_vector<float>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec2I>>(&convert_vector<Vec2I>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec3I>>(&convert_vector<Vec3I>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec4I>>(&convert_vector<Vec4I>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec2U>>(&convert_vector<Vec2U>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec3U>>(&convert_vector<Vec3U>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec4U>>(&convert_vector<Vec4U>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec2B>>(&convert_vector<Vec2B>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec3B>>(&convert_vector<Vec3B>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec4B>>(&convert_vector<Vec4B>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec2F>>(&convert_vector<Vec2F>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec3F>>(&convert_vector<Vec3F>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec4F>>(&convert_vector<Vec4F>));
    m->add(type_conversion<vector<Boxed_Value>, vector<QuatF>>(&convert_vector<QuatF>));
    m->add(fun<vector<int>&(vector<int>::*)(const vector<int>&)>(&vector<int>::operator=), "=");
    m->add(fun<vector<uint>&(vector<uint>::*)(const vector<uint>&)>(&vector<uint>::operator=), "=");
    m->add(fun<vector<float>&(vector<float>::*)(const vector<float>&)>(&vector<float>::operator=), "=");
    m->add(fun<vector<Vec2I>&(vector<Vec2I>::*)(const vector<Vec2I>&)>(&vector<Vec2I>::operator=), "=");
    m->add(fun<vector<Vec3I>&(vector<Vec3I>::*)(const vector<Vec3I>&)>(&vector<Vec3I>::operator=), "=");
    m->add(fun<vector<Vec4I>&(vector<Vec4I>::*)(const vector<Vec4I>&)>(&vector<Vec4I>::operator=), "=");
    m->add(fun<vector<Vec2U>&(vector<Vec2U>::*)(const vector<Vec2U>&)>(&vector<Vec2U>::operator=), "=");
    m->add(fun<vector<Vec3U>&(vector<Vec3U>::*)(const vector<Vec3U>&)>(&vector<Vec3U>::operator=), "=");
    m->add(fun<vector<Vec4U>&(vector<Vec4U>::*)(const vector<Vec4U>&)>(&vector<Vec4U>::operator=), "=");
    m->add(fun<vector<Vec2B>&(vector<Vec2B>::*)(const vector<Vec2B>&)>(&vector<Vec2B>::operator=), "=");
    m->add(fun<vector<Vec3B>&(vector<Vec3B>::*)(const vector<Vec3B>&)>(&vector<Vec3B>::operator=), "=");
    m->add(fun<vector<Vec4B>&(vector<Vec4B>::*)(const vector<Vec4B>&)>(&vector<Vec4B>::operator=), "=");
    m->add(fun<vector<Vec2F>&(vector<Vec2F>::*)(const vector<Vec2F>&)>(&vector<Vec2F>::operator=), "=");
    m->add(fun<vector<Vec3F>&(vector<Vec3F>::*)(const vector<Vec3F>&)>(&vector<Vec3F>::operator=), "=");
    m->add(fun<vector<Vec4F>&(vector<Vec4F>::*)(const vector<Vec4F>&)>(&vector<Vec4F>::operator=), "=");
    m->add(fun<vector<QuatF>&(vector<QuatF>::*)(const vector<QuatF>&)>(&vector<QuatF>::operator=), "=");

    m->add(fun<Vec2I(Vec2I,Vec2I)>(operator+), "+"); m->add(fun<Vec2I(Vec2I,int)>(operator+), "+");
    m->add(fun<Vec3I(Vec3I,Vec3I)>(operator+), "+"); m->add(fun<Vec3I(Vec3I,int)>(operator+), "+");
    m->add(fun<Vec4I(Vec4I,Vec4I)>(operator+), "+"); m->add(fun<Vec4I(Vec4I,int)>(operator+), "+");
    m->add(fun<Vec2U(Vec2U,Vec2U)>(operator+), "+"); m->add(fun<Vec2U(Vec2U,uint)>(operator+), "+");
    m->add(fun<Vec3U(Vec3U,Vec3U)>(operator+), "+"); m->add(fun<Vec3U(Vec3U,uint)>(operator+), "+");
    m->add(fun<Vec4U(Vec4U,Vec4U)>(operator+), "+"); m->add(fun<Vec4U(Vec4U,uint)>(operator+), "+");
    m->add(fun<Vec2F(Vec2F,Vec2F)>(operator+), "+"); m->add(fun<Vec2F(Vec2F,float)>(operator+), "+");
    m->add(fun<Vec3F(Vec3F,Vec3F)>(operator+), "+"); m->add(fun<Vec3F(Vec3F,float)>(operator+), "+");
    m->add(fun<Vec4F(Vec4F,Vec4F)>(operator+), "+"); m->add(fun<Vec4F(Vec4F,float)>(operator+), "+");

    m->add(fun<Vec2I(Vec2I,Vec2I)>(operator-), "-"); m->add(fun<Vec2I(Vec2I,int)>(operator-), "-");
    m->add(fun<Vec3I(Vec3I,Vec3I)>(operator-), "-"); m->add(fun<Vec3I(Vec3I,int)>(operator-), "-");
    m->add(fun<Vec4I(Vec4I,Vec4I)>(operator-), "-"); m->add(fun<Vec4I(Vec4I,int)>(operator-), "-");
    m->add(fun<Vec2U(Vec2U,Vec2U)>(operator-), "-"); m->add(fun<Vec2U(Vec2U,uint)>(operator-), "-");
    m->add(fun<Vec3U(Vec3U,Vec3U)>(operator-), "-"); m->add(fun<Vec3U(Vec3U,uint)>(operator-), "-");
    m->add(fun<Vec4U(Vec4U,Vec4U)>(operator-), "-"); m->add(fun<Vec4U(Vec4U,uint)>(operator-), "-");
    m->add(fun<Vec2F(Vec2F,Vec2F)>(operator-), "-"); m->add(fun<Vec2F(Vec2F,float)>(operator-), "-");
    m->add(fun<Vec3F(Vec3F,Vec3F)>(operator-), "-"); m->add(fun<Vec3F(Vec3F,float)>(operator-), "-");
    m->add(fun<Vec4F(Vec4F,Vec4F)>(operator-), "-"); m->add(fun<Vec4F(Vec4F,float)>(operator-), "-");

    m->add(fun<Vec2I(Vec2I,Vec2I)>(operator*), "*"); m->add(fun<Vec2I(Vec2I,int)>(operator*), "*");
    m->add(fun<Vec3I(Vec3I,Vec3I)>(operator*), "*"); m->add(fun<Vec3I(Vec3I,int)>(operator*), "*");
    m->add(fun<Vec4I(Vec4I,Vec4I)>(operator*), "*"); m->add(fun<Vec4I(Vec4I,int)>(operator*), "*");
    m->add(fun<Vec2U(Vec2U,Vec2U)>(operator*), "*"); m->add(fun<Vec2U(Vec2U,uint)>(operator*), "*");
    m->add(fun<Vec3U(Vec3U,Vec3U)>(operator*), "*"); m->add(fun<Vec3U(Vec3U,uint)>(operator*), "*");
    m->add(fun<Vec4U(Vec4U,Vec4U)>(operator*), "*"); m->add(fun<Vec4U(Vec4U,uint)>(operator*), "*");
    m->add(fun<Vec2F(Vec2F,Vec2F)>(operator*), "*"); m->add(fun<Vec2F(Vec2F,float)>(operator*), "*");
    m->add(fun<Vec3F(Vec3F,Vec3F)>(operator*), "*"); m->add(fun<Vec3F(Vec3F,float)>(operator*), "*");
    m->add(fun<Vec4F(Vec4F,Vec4F)>(operator*), "*"); m->add(fun<Vec4F(Vec4F,float)>(operator*), "*");

    m->add(fun<Vec2I(Vec2I,Vec2I)>(operator/), "/"); m->add(fun<Vec2I(Vec2I,int)>(operator/), "/");
    m->add(fun<Vec3I(Vec3I,Vec3I)>(operator/), "/"); m->add(fun<Vec3I(Vec3I,int)>(operator/), "/");
    m->add(fun<Vec4I(Vec4I,Vec4I)>(operator/), "/"); m->add(fun<Vec4I(Vec4I,int)>(operator/), "/");
    m->add(fun<Vec2U(Vec2U,Vec2U)>(operator/), "/"); m->add(fun<Vec2U(Vec2U,uint)>(operator/), "/");
    m->add(fun<Vec3U(Vec3U,Vec3U)>(operator/), "/"); m->add(fun<Vec3U(Vec3U,uint)>(operator/), "/");
    m->add(fun<Vec4U(Vec4U,Vec4U)>(operator/), "/"); m->add(fun<Vec4U(Vec4U,uint)>(operator/), "/");
    m->add(fun<Vec2F(Vec2F,Vec2F)>(operator/), "/"); m->add(fun<Vec2F(Vec2F,float)>(operator/), "/");
    m->add(fun<Vec3F(Vec3F,Vec3F)>(operator/), "/"); m->add(fun<Vec3F(Vec3F,float)>(operator/), "/");
    m->add(fun<Vec4F(Vec4F,Vec4F)>(operator/), "/"); m->add(fun<Vec4F(Vec4F,float)>(operator/), "/");

    m->add(fun<Vec2F (*)(Vec2F)>(&maths::normalize), "normalize");
    m->add(fun<Vec3F (*)(Vec3F)>(&maths::normalize), "normalize");
    m->add(fun<Vec4F (*)(Vec4F)>(&maths::normalize), "normalize");
    m->add(fun<QuatF (*)(QuatF)>(&maths::normalize), "normalize");

    _cs.add(m);
}
