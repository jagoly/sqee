#include <algorithm>

#include <chaiscript/dispatchkit/bootstrap.hpp>
#include <chaiscript/dispatchkit/bootstrap_stl.hpp>

#include <sqee/messages.hpp>

#include <sqee/misc/StringCast.hpp>

#include <sqee/app/Application.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/app/MessageBus.hpp>

#include <sqee/physics/Bodies.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <sqee/scripts/Helpers.hpp>
#include <sqee/scripts/BasicSetup.hpp>

using namespace sq;

//============================================================================//

void sq::chaiscript_setup_app(ChaiEngine& engine)
{
    chai::ModulePtr m = std::make_shared<chai::Module>();

    //========================================================//

    m->add(user_type<Application>(), "Application");
    m->add(user_type<ChaiConsole>(), "ChaiConsole");
    m->add(user_type<DebugOverlay>(), "DebugOverlay");
    m->add(user_type<MessageBus>(), "MessageBus");

    //========================================================//

    m->add(fun(&Application::OPTION_WindowTitle), "OPTION_WindowTitle");
    m->add(fun(&Application::OPTION_WindowSize),  "OPTION_WindowSize");
    m->add(fun(&Application::update_options),     "update_options");
    m->add(fun(&Application::quit),               "quit");

    auto get_overlay = [](Application* app) -> DebugOverlay& { return app->get_debug_overlay(); };
    auto get_console = [](Application* app) -> ChaiConsole& { return app->get_chai_console(); };

    m->add(fun(get_console), "console");
    m->add(fun(get_overlay), "overlay");

    //========================================================//

    m->add(fun(&DebugOverlay::notify), "notify");

    m->add(fun(&ChaiConsole::cs_clear),   "clear");
    m->add(fun(&ChaiConsole::cs_history), "history");
    m->add(fun(&ChaiConsole::cs_print),   "print");

    //========================================================//

    m->add(vector_conversion<vector<string>>());
    chai::bootstrap::operators::assign<vector<string>>(*m);

    //========================================================//

    engine.add(m);
}

//============================================================================//

void sq::chaiscript_setup_physics(ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());
/*
    m->add(fun(&BaseBody::set_transform),  "set_transform");
    m->add(fun(&BaseBody::set_position),   "set_position");
    m->add(fun(&BaseBody::set_rotation),   "set_rotation");
    m->add(fun(&BaseBody::set_friction),   "set_friction");
    m->add(fun(&BaseBody::set_bounciness), "set_bounciness");

    m->add(fun(&BaseBody::get_transform),  "get_transform");
    m->add(fun(&BaseBody::get_position),   "get_position");
    m->add(fun(&BaseBody::get_rotation),   "get_rotation");
    m->add(fun(&BaseBody::get_friction),   "get_friction");
    m->add(fun(&BaseBody::get_bounciness), "get_bounciness");

    add_class<StaticBody>(*m, "StaticBody", {}, {
        {fun<void, StaticBody, uint, Vec3F, Vec3F, QuatF>        (&StaticBody::add_BoxShape),      "add_BoxShape"},
        {fun<void, StaticBody, uint, float, Vec3F, QuatF>        (&StaticBody::add_SphereShape),   "add_SphereShape"},
        {fun<void, StaticBody, uint, float, float, Vec3F, QuatF> (&StaticBody::add_ConeShape),     "add_ConeShape"},
        {fun<void, StaticBody, uint, float, float, Vec3F, QuatF> (&StaticBody::add_CylinderShape), "add_CylinderShape"},
        {fun<void, StaticBody, uint, float, float, Vec3F, QuatF> (&StaticBody::add_CapsuleShape),  "add_CapsuleShape"},

        {fun<void, StaticBody, uint, Vec3F>        (&StaticBody::add_BoxShape),      "add_BoxShape"},
        {fun<void, StaticBody, uint, float>        (&StaticBody::add_SphereShape),   "add_SphereShape"},
        {fun<void, StaticBody, uint, float, float> (&StaticBody::add_ConeShape),     "add_ConeShape"},
        {fun<void, StaticBody, uint, float, float> (&StaticBody::add_CylinderShape), "add_CylinderShape"},
        {fun<void, StaticBody, uint, float, float> (&StaticBody::add_CapsuleShape),  "add_CapsuleShape"}
    });

    add_class<DynamicBody>(*m, "DynamicBody", {}, {
        {fun<void, DynamicBody, uint, Vec3F, Vec3F, QuatF, float>        (&DynamicBody::add_BoxShape),      "add_BoxShape"},
        {fun<void, DynamicBody, uint, float, Vec3F, QuatF, float>        (&DynamicBody::add_SphereShape),   "add_SphereShape"},
        {fun<void, DynamicBody, uint, float, float, Vec3F, QuatF, float> (&DynamicBody::add_ConeShape),     "add_ConeShape"},
        {fun<void, DynamicBody, uint, float, float, Vec3F, QuatF, float> (&DynamicBody::add_CylinderShape), "add_CylinderShape"},
        {fun<void, DynamicBody, uint, float, float, Vec3F, QuatF, float> (&DynamicBody::add_CapsuleShape),  "add_CapsuleShape"},

        {fun<void, DynamicBody, uint, Vec3F, float>        (&DynamicBody::add_BoxShape),      "add_BoxShape"},
        {fun<void, DynamicBody, uint, float, float>        (&DynamicBody::add_SphereShape),   "add_SphereShape"},
        {fun<void, DynamicBody, uint, float, float, float> (&DynamicBody::add_ConeShape),     "add_ConeShape"},
        {fun<void, DynamicBody, uint, float, float, float> (&DynamicBody::add_CylinderShape), "add_CylinderShape"},
        {fun<void, DynamicBody, uint, float, float, float> (&DynamicBody::add_CapsuleShape),  "add_CapsuleShape"},

        {fun(&DynamicBody::set_linearDamp),      "set_linearDamp"},
        {fun(&DynamicBody::set_angularDamp),     "set_angularDamp"},
        {fun(&DynamicBody::set_linearVelocity),  "set_linearVelocity"},
        {fun(&DynamicBody::set_angularVelocity), "set_angularVelocity"},
        {fun(&DynamicBody::set_rollResistance),  "set_rollResistance"},

        {fun(&DynamicBody::get_linearDamp),      "get_linearDamp"},
        {fun(&DynamicBody::get_angularDamp),     "get_angularDamp"},
        {fun(&DynamicBody::get_linearVelocity),  "get_linearVelocity"},
        {fun(&DynamicBody::get_angularVelocity), "get_angularVelocity"},
        {fun(&DynamicBody::get_rollResistance),  "get_rollResistance"},
        {fun(&DynamicBody::get_mass),            "get_mass"},
    });
*/
    _engine.add(m);
}

//============================================================================//

namespace { // anonymous

template <int S, class T> inline
void impl_setup_VectorT(chai::Module& m, const string& name)
{
    using VecST = Vector<S, T>;

    m.add(constructor<VecST(const VecST&)>(), name);
    m.add(fun(&VecST::operator=), "=");

    m.add(vector_conversion<vector<VecST>>());
    m.add(type_conversion<VecST, string>(&chai_string<VecST>));

    m.add(fun<VecST, VecST, VecST>(operator+), "+");
    m.add(fun<VecST, VecST, VecST>(operator-), "-");
    m.add(fun<VecST, VecST, VecST>(operator*), "*");
    m.add(fun<VecST, VecST, VecST>(operator/), "/");
    m.add(fun<VecST, VecST, T>(operator+), "+");
    m.add(fun<VecST, VecST, T>(operator-), "-");
    m.add(fun<VecST, VecST, T>(operator*), "*");
    m.add(fun<VecST, VecST, T>(operator/), "/");
}

template <class T> inline
void impl_setup_Vector2(chai::Module& m, const string& name)
{
    impl_setup_VectorT<2, T>(m, name);
    m.add(constructor<Vector2<T>(T, T)>(), name);
    m.add(fun(&Vector2<T>::x), "x");
    m.add(fun(&Vector2<T>::y), "y");
}

template <class T> inline
void impl_setup_Vector3(chai::Module& m, const string& name)
{
    impl_setup_VectorT<3, T>(m, name);
    m.add(constructor<Vector3<T>(T, T, T)>(), name);
    m.add(fun(&Vector3<T>::x), "x");
    m.add(fun(&Vector3<T>::y), "y");
    m.add(fun(&Vector3<T>::z), "z");
}

template <class T> inline
void impl_setup_Vector4(chai::Module& m, const string& name)
{
    impl_setup_VectorT<4, T>(m, name);
    m.add(constructor<Vector4<T>(T, T, T, T)>(), name);
    m.add(fun(&Vector4<T>::x), "x");
    m.add(fun(&Vector4<T>::y), "y");
    m.add(fun(&Vector4<T>::z), "z");
    m.add(fun(&Vector4<T>::w), "w");
}

} // anonymous namespace

//============================================================================//

void sq::chaiscript_setup_maths(ChaiEngine& engine)
{
    chai::ModulePtr m = std::make_shared<chai::Module>();

    impl_setup_Vector2<int>(*m, "Vec2I");
    impl_setup_Vector3<int>(*m, "Vec3I");
    impl_setup_Vector4<int>(*m, "Vec4I");
    impl_setup_Vector2<uint>(*m, "Vec2U");
    impl_setup_Vector3<uint>(*m, "Vec3U");
    impl_setup_Vector4<uint>(*m, "Vec4U");
    impl_setup_Vector2<float>(*m, "Vec2F");
    impl_setup_Vector3<float>(*m, "Vec3F");
    impl_setup_Vector4<float>(*m, "Vec4F");

    m->add(constructor<QuatF(const QuatF&)>(), "QuatF");
    m->add(constructor<QuatF(float, float, float, float)>(), "QuatF");
    m->add(constructor<QuatF(float, float, float)>(), "QuatF");
    m->add(fun(&QuatF::x), "x"); m->add(fun(&QuatF::y), "y");
    m->add(fun(&QuatF::z), "z"); m->add(fun(&QuatF::w), "w");
    m->add(fun(&QuatF::operator=), "=");

    m->add(vector_conversion<vector<int>>());
    m->add(vector_conversion<vector<uint>>());
    m->add(vector_conversion<vector<float>>());
    m->add(vector_conversion<vector<QuatF>>());

    m->add(type_conversion<int, string>(&chai_string<int>));
    m->add(type_conversion<uint, string>(&chai_string<uint>));
    m->add(type_conversion<float, string>(&chai_string<float>));
    m->add(type_conversion<QuatF, string>(&chai_string<QuatF>));

    m->add(fun<Vec2F, Vec2F>(&maths::normalize), "normalize");
    m->add(fun<Vec3F, Vec3F>(&maths::normalize), "normalize");
    m->add(fun<Vec4F, Vec4F>(&maths::normalize), "normalize");
    m->add(fun<QuatF, QuatF>(&maths::normalize), "normalize");

    engine.add(m);
}

//============================================================================//

void sq::chaiscript_setup_messages(ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());

    //chai_add_message_type<msg::Configure_Entity>(*m, "Configure_Entity");

    _engine.add(m);
}
