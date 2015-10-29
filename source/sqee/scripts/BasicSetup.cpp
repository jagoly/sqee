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
    _cs.add(m);

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


template<class T> vector<T>& assign_vector(vector<T>& lhs, const vector<T>& rhs) { return lhs = rhs; }
template<class T> vector<T> convert_vector(const vector<Boxed_Value>& vec) { vector<T> ret;
    for (const auto& bv : vec) ret.emplace_back(boxed_cast<T>(bv)); return ret; }


void sq::cs_setup_maths(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Vec2F>(*m, "Vec2F", { constructor<Vec2F(const Vec2F&)>(),
        constructor<Vec2F(float, float)>() }, {
        {fun<Vec2F& (Vec2F::*)(const Vec2F&)>(&Vec2F::operator=), "="},
        {fun(&Vec2F::x), "x"}, {fun(&Vec2F::y), "y"}} );
    add_class<Vec2I>(*m, "Vec2I", { constructor<Vec2I(const Vec2I&)>(),
        constructor<Vec2I(int, int)>() }, {
        {fun<Vec2I& (Vec2I::*)(const Vec2I&)>(&Vec2I::operator=), "="},
        {fun(&Vec2I::x), "x"}, {fun(&Vec2I::y), "y"}} );
    add_class<Vec2U>(*m, "Vec2U", { constructor<Vec2U(const Vec2U&)>(),
        constructor<Vec2U(uint, uint)>() }, {
        {fun<Vec2U& (Vec2U::*)(const Vec2U&)>(&Vec2U::operator=), "="},
        {fun(&Vec2U::x), "x"}, {fun(&Vec2U::y), "y"}} );

    add_class<Vec3F>(*m, "Vec3F", { constructor<Vec3F(const Vec3F&)>(),
        constructor<Vec3F(float, float, float)>() }, {
        {fun<Vec3F& (Vec3F::*)(const Vec3F&)>(&Vec3F::operator=), "="},
        {fun(&Vec3F::x), "x"}, {fun(&Vec3F::y), "y"}, {fun(&Vec3F::z), "z"}} );
    add_class<Vec3I>(*m, "Vec3I", { constructor<Vec3I(const Vec3I&)>(),
        constructor<Vec3I(int, int, int)>() }, {
        {fun<Vec3I& (Vec3I::*)(const Vec3I&)>(&Vec3I::operator=), "="},
        {fun(&Vec3I::x), "x"}, {fun(&Vec3I::y), "y"}, {fun(&Vec3I::z), "z"}} );
    add_class<Vec3U>(*m, "Vec3U", { constructor<Vec3U(const Vec3U&)>(),
        constructor<Vec3U(uint, uint, uint)>() }, {
        {fun<Vec3U& (Vec3U::*)(const Vec3U&)>(&Vec3U::operator=), "="},
        {fun(&Vec3U::x), "x"}, {fun(&Vec3U::y), "y"}, {fun(&Vec3U::z), "z"}} );

    add_class<Vec4F>(*m, "Vec4F", { constructor<Vec4F(const Vec4F&)>(),
        constructor<Vec4F(float, float, float, float)>() }, {
        {fun<Vec4F& (Vec4F::*)(const Vec4F&)>(&Vec4F::operator=), "="},
        {fun(&Vec4F::x), "x"}, {fun(&Vec4F::y), "y"}, {fun(&Vec4F::z), "z"}, {fun(&Vec4F::w), "w"}} );
    add_class<Vec4I>(*m, "Vec4I", { constructor<Vec4I(const Vec4I&)>(),
        constructor<Vec4I(int, int, int, int)>() }, {
        {fun<Vec4I& (Vec4I::*)(const Vec4I&)>(&Vec4I::operator=), "="},
        {fun(&Vec4I::x), "x"}, {fun(&Vec4I::y), "y"}, {fun(&Vec4I::z), "z"}, {fun(&Vec4I::w), "w"}} );
    add_class<Vec4U>(*m, "Vec4U", { constructor<Vec4U(const Vec4U&)>(),
        constructor<Vec4U(uint, uint, uint, uint)>() }, {
        {fun<Vec4U& (Vec4U::*)(const Vec4U&)>(&Vec4U::operator=), "="},
        {fun(&Vec4U::x), "x"}, {fun(&Vec4U::y), "y"}, {fun(&Vec4U::z), "z"}, {fun(&Vec4U::w), "w"}} );

    add_class<QuatF>(*m, "QuatF", { constructor<QuatF(const QuatF&)>(),
        constructor<QuatF(float, float, float, float)>() }, {
        {fun<QuatF& (QuatF::*)(const QuatF&)>(&QuatF::operator=), "="},
        {fun(&QuatF::x), "x"}, {fun(&QuatF::y), "y"}, {fun(&QuatF::z), "z"}, {fun(&QuatF::w), "w"}} );

    m->add(fun(&assign_vector<int>), "=");
    m->add(fun(&assign_vector<uint>), "=");
    m->add(fun(&assign_vector<float>), "=");
    m->add(fun(&assign_vector<Vec2F>), "=");
    m->add(fun(&assign_vector<Vec2I>), "=");
    m->add(fun(&assign_vector<Vec2U>), "=");
    m->add(fun(&assign_vector<Vec3F>), "=");
    m->add(fun(&assign_vector<Vec3I>), "=");
    m->add(fun(&assign_vector<Vec3U>), "=");
    m->add(fun(&assign_vector<Vec4F>), "=");
    m->add(fun(&assign_vector<Vec4I>), "=");
    m->add(fun(&assign_vector<Vec4U>), "=");
    m->add(fun(&assign_vector<QuatF>), "=");
    m->add(user_type<vector<int>>(), "Vector_int");
    m->add(user_type<vector<uint>>(), "Vector_uint");
    m->add(user_type<vector<float>>(), "Vector_float");
    m->add(user_type<vector<Vec2I>>(), "Vector_Vec2I");
    m->add(user_type<vector<Vec3I>>(), "Vector_Vec3I");
    m->add(user_type<vector<Vec4I>>(), "Vector_Vec4I");
    m->add(user_type<vector<Vec2U>>(), "Vector_Vec2U");
    m->add(user_type<vector<Vec3U>>(), "Vector_Vec3U");
    m->add(user_type<vector<Vec4U>>(), "Vector_Vec4U");
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
    m->add(type_conversion<Vec2F, string>(&chai_string<Vec2F>));
    m->add(type_conversion<Vec3F, string>(&chai_string<Vec3F>));
    m->add(type_conversion<Vec4F, string>(&chai_string<Vec4F>));
    m->add(type_conversion<QuatF, string>(&chai_string<QuatF>));
    m->add(type_conversion<vector<Boxed_Value>, vector<int>>(&convert_vector<int>));
    m->add(type_conversion<vector<Boxed_Value>, vector<uint>>(&convert_vector<uint>));
    m->add(type_conversion<vector<Boxed_Value>, vector<float>>(&convert_vector<float>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec2F>>(&convert_vector<Vec2F>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec2I>>(&convert_vector<Vec2I>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec2U>>(&convert_vector<Vec2U>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec3F>>(&convert_vector<Vec3F>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec3I>>(&convert_vector<Vec3I>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec3U>>(&convert_vector<Vec3U>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec4F>>(&convert_vector<Vec4F>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec4I>>(&convert_vector<Vec4I>));
    m->add(type_conversion<vector<Boxed_Value>, vector<Vec4U>>(&convert_vector<Vec4U>));
    m->add(type_conversion<vector<Boxed_Value>, vector<QuatF>>(&convert_vector<QuatF>));

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
