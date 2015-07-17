#include <chaiscript/utility/utility.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/redist/tinyformat.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/SettingsMaps.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/scripts/ConsoleScene.hpp>
#include <sqee/scripts/Intergration.hpp>

using namespace sq;

using chai::fun;
using chai::user_type;
using chai::constructor;
using chai::type_conversion;
using chai::utility::add_class;
using chai::Boxed_Value;
using chai::boxed_cast;

template<class T> const T add_tvec(const T& lhs, const T& rhs) { return lhs + rhs; }
template<class T> const T sub_tvec(const T& lhs, const T& rhs) { return lhs - rhs; }
template<class T> const T mul_tvec(const T& lhs, const T& rhs) { return lhs * rhs; }
template<class T> const T div_tvec(const T& lhs, const T& rhs) { return lhs / rhs; }
template<class T> const T mul_tvec_val(const T& lhs, typename T::value_type rhs) { return lhs * rhs; }
template<class T> const T div_tvec_val(const T& lhs, typename T::value_type rhs) { return lhs / rhs; }
template<class T> vector<T>& assign_vector(vector<T>& lhs, const vector<T>& rhs) { return lhs = rhs; }
template<class T> vector<T> convert_vector(const vector<Boxed_Value>& vec) { vector<T> ret;
    for (const auto& bv : vec) ret.emplace_back(boxed_cast<T>(bv)); return ret; }
string tostring_int(int i) { return tfm::format("int(%i)", i); };
string tostring_uint(uint u) { return tfm::format("uint(%u)", u); };
string tostring_float(float f) { return tfm::format("float(%f)", f); };
string tostring_fvec2(fvec2 v) { return tfm::format("fvec2(%f, %f)", v.x,v.y); };
string tostring_ivec2(ivec2 v) { return tfm::format("ivec2(%i, %i)", v.x,v.y); };
string tostring_uvec2(uvec2 v) { return tfm::format("uvec2(%u, %u)", v.x,v.y); };
string tostring_fvec3(fvec3 v) { return tfm::format("fvec3(%f, %f, %f)", v.x,v.y,v.z); };
string tostring_ivec3(ivec3 v) { return tfm::format("ivec3(%i, %i, %i)", v.x,v.y,v.z); };
string tostring_uvec3(uvec3 v) { return tfm::format("uvec3(%u, %u, %u)", v.x,v.y,v.z); };
string tostring_fvec4(fvec4 v) { return tfm::format("fvec4(%f, %f, %f, %f)", v.x,v.y,v.z,v.w); };
string tostring_ivec4(ivec4 v) { return tfm::format("ivec4(%i, %i, %i, %i)", v.x,v.y,v.z,v.w); };
string tostring_uvec4(uvec4 v) { return tfm::format("uvec4(%u, %u, %u, %u)", v.x,v.y,v.z,v.w); };
string tostring_fquat(fquat q) { return tfm::format("fquat(%f, %f, %f, %f)", q.w,q.x,q.y,q.z); };
fvec3 normalize_fvec3(fvec3 v) { return glm::normalize(v); }
fquat normalize_fquat(fquat q) { return glm::normalize(q); }

void sq::cs_setup_maths(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<fvec2>(*m, "fvec2", { constructor<fvec2(const fvec2&)>(),
        constructor<fvec2(float, float)>() }, {
        {fun<fvec2& (fvec2::*)(const fvec2&)>(&fvec2::operator=), "="},
        {fun(&fvec2::x), "x"}, {fun(&fvec2::y), "y"}} );
    add_class<ivec2>(*m, "ivec2", { constructor<ivec2(const ivec2&)>(),
        constructor<ivec2(int, int)>() }, {
        {fun<ivec2& (ivec2::*)(const ivec2&)>(&ivec2::operator=), "="},
        {fun(&ivec2::x), "x"}, {fun(&ivec2::y), "y"}} );
    add_class<uvec2>(*m, "uvec2", { constructor<uvec2(const uvec2&)>(),
        constructor<uvec2(uint, uint)>() }, {
        {fun<uvec2& (uvec2::*)(const uvec2&)>(&uvec2::operator=), "="},
        {fun(&uvec2::x), "x"}, {fun(&uvec2::y), "y"}} );

    add_class<fvec3>(*m, "fvec3", { constructor<fvec3(const fvec3&)>(),
        constructor<fvec3(float, float, float)>() }, {
        {fun<fvec3& (fvec3::*)(const fvec3&)>(&fvec3::operator=), "="},
        {fun(&fvec3::x), "x"}, {fun(&fvec3::y), "y"}, {fun(&fvec3::z), "z"}} );
    add_class<ivec3>(*m, "ivec3", { constructor<ivec3(const ivec3&)>(),
        constructor<ivec3(int, int, int)>() }, {
        {fun<ivec3& (ivec3::*)(const ivec3&)>(&ivec3::operator=), "="},
        {fun(&ivec3::x), "x"}, {fun(&ivec3::y), "y"}, {fun(&ivec3::z), "z"}} );
    add_class<uvec3>(*m, "uvec3", { constructor<uvec3(const uvec3&)>(),
        constructor<uvec3(uint, uint, uint)>() }, {
        {fun<uvec3& (uvec3::*)(const uvec3&)>(&uvec3::operator=), "="},
        {fun(&uvec3::x), "x"}, {fun(&uvec3::y), "y"}, {fun(&uvec3::z), "z"}} );

    add_class<fvec4>(*m, "fvec4", { constructor<fvec4(const fvec4&)>(),
        constructor<fvec4(float, float, float, float)>() }, {
        {fun<fvec4& (fvec4::*)(const fvec4&)>(&fvec4::operator=), "="},
        {fun(&fvec4::x), "x"}, {fun(&fvec4::y), "y"}, {fun(&fvec4::z), "z"}, {fun(&fvec4::w), "w"}} );
    add_class<ivec4>(*m, "ivec4", { constructor<ivec4(const ivec4&)>(),
        constructor<ivec4(int, int, int, int)>() }, {
        {fun<ivec4& (ivec4::*)(const ivec4&)>(&ivec4::operator=), "="},
        {fun(&ivec4::x), "x"}, {fun(&ivec4::y), "y"}, {fun(&ivec4::z), "z"}, {fun(&ivec4::w), "w"}} );
    add_class<uvec4>(*m, "uvec4", { constructor<uvec4(const uvec4&)>(),
        constructor<uvec4(uint, uint, uint, uint)>() }, {
        {fun<uvec4& (uvec4::*)(const uvec4&)>(&uvec4::operator=), "="},
        {fun(&uvec4::x), "x"}, {fun(&uvec4::y), "y"}, {fun(&uvec4::z), "z"}, {fun(&uvec4::w), "w"}} );

    add_class<fquat>(*m, "fquat", { constructor<fquat(const fquat&)>(),
        constructor<fquat(float, float, float, float)>() }, {
        {fun<fquat& (fquat::*)(const fquat&)>(&fquat::operator=), "="},
        {fun(&fquat::w), "w"}, {fun(&fquat::x), "x"}, {fun(&fquat::y), "y"}, {fun(&fquat::z), "z"}} );


    m->add(fun(&assign_vector<int>), "=");
    m->add(fun(&assign_vector<uint>), "=");
    m->add(fun(&assign_vector<float>), "=");
    m->add(fun(&assign_vector<fvec2>), "=");
    m->add(fun(&assign_vector<ivec2>), "=");
    m->add(fun(&assign_vector<uvec2>), "=");
    m->add(fun(&assign_vector<fvec3>), "=");
    m->add(fun(&assign_vector<ivec3>), "=");
    m->add(fun(&assign_vector<uvec3>), "=");
    m->add(fun(&assign_vector<fvec4>), "=");
    m->add(fun(&assign_vector<ivec4>), "=");
    m->add(fun(&assign_vector<uvec4>), "=");
    m->add(fun(&assign_vector<fquat>), "=");
    m->add(user_type<vector<int>>(), "Vector_int");
    m->add(user_type<vector<uint>>(), "Vector_uint");
    m->add(user_type<vector<float>>(), "Vector_float");
    m->add(user_type<vector<fvec2>>(), "Vector_fvec2");
    m->add(user_type<vector<ivec2>>(), "Vector_ivec2");
    m->add(user_type<vector<uvec2>>(), "Vector_uvec2");
    m->add(user_type<vector<fvec3>>(), "Vector_fvec3");
    m->add(user_type<vector<ivec3>>(), "Vector_ivec3");
    m->add(user_type<vector<uvec3>>(), "Vector_uvec3");
    m->add(user_type<vector<fvec4>>(), "Vector_fvec4");
    m->add(user_type<vector<ivec4>>(), "Vector_ivec4");
    m->add(user_type<vector<uvec4>>(), "Vector_uvec4");
    m->add(user_type<vector<fquat>>(), "Vector_fquat");
    m->add(type_conversion<int, string>(&tostring_int));
    m->add(type_conversion<uint, string>(&tostring_uint));
    m->add(type_conversion<float, string>(&tostring_float));
    m->add(type_conversion<fvec2, string>(&tostring_fvec2));
    m->add(type_conversion<ivec2, string>(&tostring_ivec2));
    m->add(type_conversion<uvec2, string>(&tostring_uvec2));
    m->add(type_conversion<fvec3, string>(&tostring_fvec3));
    m->add(type_conversion<ivec3, string>(&tostring_ivec3));
    m->add(type_conversion<uvec3, string>(&tostring_uvec3));
    m->add(type_conversion<fvec4, string>(&tostring_fvec4));
    m->add(type_conversion<ivec4, string>(&tostring_ivec4));
    m->add(type_conversion<uvec4, string>(&tostring_uvec4));
    m->add(type_conversion<fquat, string>(&tostring_fquat));
    m->add(type_conversion<vector<Boxed_Value>, vector<int>>(&convert_vector<int>));
    m->add(type_conversion<vector<Boxed_Value>, vector<uint>>(&convert_vector<uint>));
    m->add(type_conversion<vector<Boxed_Value>, vector<float>>(&convert_vector<float>));
    m->add(type_conversion<vector<Boxed_Value>, vector<fvec2>>(&convert_vector<fvec2>));
    m->add(type_conversion<vector<Boxed_Value>, vector<ivec2>>(&convert_vector<ivec2>));
    m->add(type_conversion<vector<Boxed_Value>, vector<uvec2>>(&convert_vector<uvec2>));
    m->add(type_conversion<vector<Boxed_Value>, vector<fvec3>>(&convert_vector<fvec3>));
    m->add(type_conversion<vector<Boxed_Value>, vector<ivec3>>(&convert_vector<ivec3>));
    m->add(type_conversion<vector<Boxed_Value>, vector<uvec3>>(&convert_vector<uvec3>));
    m->add(type_conversion<vector<Boxed_Value>, vector<fvec4>>(&convert_vector<fvec4>));
    m->add(type_conversion<vector<Boxed_Value>, vector<ivec4>>(&convert_vector<ivec4>));
    m->add(type_conversion<vector<Boxed_Value>, vector<uvec4>>(&convert_vector<uvec4>));
    m->add(type_conversion<vector<Boxed_Value>, vector<fquat>>(&convert_vector<fquat>));
    m->add(fun(&mul_tvec_val<fvec2>), "*"); m->add(fun(&div_tvec_val<fvec2>), "/");
    m->add(fun(&mul_tvec_val<ivec2>), "*"); m->add(fun(&div_tvec_val<ivec2>), "/");
    m->add(fun(&mul_tvec_val<uvec2>), "*"); m->add(fun(&div_tvec_val<uvec2>), "/");
    m->add(fun(&mul_tvec_val<fvec3>), "*"); m->add(fun(&div_tvec_val<fvec3>), "/");
    m->add(fun(&mul_tvec_val<ivec3>), "*"); m->add(fun(&div_tvec_val<ivec3>), "/");
    m->add(fun(&mul_tvec_val<uvec3>), "*"); m->add(fun(&div_tvec_val<uvec3>), "/");
    m->add(fun(&mul_tvec_val<fvec4>), "*"); m->add(fun(&div_tvec_val<fvec4>), "/");
    m->add(fun(&mul_tvec_val<ivec4>), "*"); m->add(fun(&div_tvec_val<ivec4>), "/");
    m->add(fun(&mul_tvec_val<uvec4>), "*"); m->add(fun(&div_tvec_val<uvec4>), "/");
    m->add(fun(&add_tvec<fvec2>), "+"); m->add(fun(&sub_tvec<fvec2>), "-");
    m->add(fun(&add_tvec<ivec2>), "+"); m->add(fun(&sub_tvec<ivec2>), "-");
    m->add(fun(&add_tvec<uvec2>), "+"); m->add(fun(&sub_tvec<uvec2>), "-");
    m->add(fun(&add_tvec<fvec3>), "+"); m->add(fun(&sub_tvec<fvec3>), "-");
    m->add(fun(&add_tvec<ivec3>), "+"); m->add(fun(&sub_tvec<ivec3>), "-");
    m->add(fun(&add_tvec<uvec3>), "+"); m->add(fun(&sub_tvec<uvec3>), "-");
    m->add(fun(&add_tvec<fvec4>), "+"); m->add(fun(&sub_tvec<fvec4>), "-");
    m->add(fun(&add_tvec<ivec4>), "+"); m->add(fun(&sub_tvec<ivec4>), "-");
    m->add(fun(&add_tvec<uvec4>), "+"); m->add(fun(&sub_tvec<uvec4>), "-");
    m->add(fun(&mul_tvec<fvec2>), "*"); m->add(fun(&div_tvec<fvec2>), "/");
    m->add(fun(&mul_tvec<ivec2>), "*"); m->add(fun(&div_tvec<ivec2>), "/");
    m->add(fun(&mul_tvec<uvec2>), "*"); m->add(fun(&div_tvec<uvec2>), "/");
    m->add(fun(&mul_tvec<fvec3>), "*"); m->add(fun(&div_tvec<fvec3>), "/");
    m->add(fun(&mul_tvec<ivec3>), "*"); m->add(fun(&div_tvec<ivec3>), "/");
    m->add(fun(&mul_tvec<uvec3>), "*"); m->add(fun(&div_tvec<uvec3>), "/");
    m->add(fun(&mul_tvec<fvec4>), "*"); m->add(fun(&div_tvec<fvec4>), "/");
    m->add(fun(&mul_tvec<ivec4>), "*"); m->add(fun(&div_tvec<ivec4>), "/");
    m->add(fun(&mul_tvec<uvec4>), "*"); m->add(fun(&div_tvec<uvec4>), "/");
    m->add(fun(normalize_fvec3), "normalize");
    m->add(fun(normalize_fquat), "normalize");

    _cs.add(m);
}

void sq::cs_setup_application(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Application>(*m, "Application", {}, {
        {fun(&Application::update), "update"},
        {fun(&Application::quit), "quit"} });

    _cs.add(m);
}

void sq::cs_setup_settings(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<SettingsMaps>(*m, "SettingsMaps", {}, {
        {fun(&SettingsMaps::add<int>), "addI"},
        {fun(&SettingsMaps::mod<int>), "modI"},
        {fun(&SettingsMaps::crnt<int>), "crntI"},
        {fun(&SettingsMaps::next<int>), "nextI"},
        {fun(&SettingsMaps::add<bool>), "addB"},
        {fun(&SettingsMaps::mod<bool>), "modB"},
        {fun(&SettingsMaps::crnt<bool>), "crntB"},
        {fun(&SettingsMaps::next<bool>), "nextB"},
        {fun(&SettingsMaps::add<float>), "addF"},
        {fun(&SettingsMaps::mod<float>), "modF"},
        {fun(&SettingsMaps::crnt<float>), "crntF"},
        {fun(&SettingsMaps::next<float>), "nextF"},
        {fun(&SettingsMaps::add<string>), "addS"},
        {fun(&SettingsMaps::mod<string>), "modS"},
        {fun(&SettingsMaps::crnt<string>), "crntS"},
        {fun(&SettingsMaps::next<string>), "nextS"},
        {fun(&SettingsMaps::apply), "apply"} });
    _cs.add(m);
}

void sq::cs_setup_console(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<ConsoleScene>(*m, "Console", {}, {
        {fun(&ConsoleScene::cs_print), "print"},
        {fun(&ConsoleScene::cs_clear), "clear"},
        {fun(&ConsoleScene::cs_history), "history"} });

    _cs.add(m);
}

void sq::cs_setup_render(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Camera>(*m, "Camera", {}, {
        {fun(&Camera::pos), "position"},
        {fun(&Camera::dir), "direction"} });

    _cs.add(m);
}
