#include <algorithm>

#include <chaiscript/dispatchkit/bootstrap.hpp>
#include <chaiscript/dispatchkit/bootstrap_stl.hpp>

#include <sqee/redist/tinyformat.hpp>
#include <sqee/misc/StringCast.hpp>

#include <sqee/app/Application.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/app/MessageBus.hpp>

#include <sqee/physics/Bodies.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <sqee/ecs/Entity.hpp>
#include <sqee/ecs/Manager.hpp>

#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/scripts/BasicSetup.hpp>
#include <sqee/scripts/Helpers.hpp>

#include <sqee/messages.hpp>

using namespace sq;


void sq::chaiscript_setup_app(ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());

    auto get_overlay = [](Application* app) -> DebugOverlay& { return *app->overlay; };
    auto get_console = [](Application* app) -> ChaiConsole& { return *app->console; };

    add_class<Application>(*m, "Application", {}, {
        {fun(&Application::OPTION_WindowTitle), "OPTION_WindowTitle"},
        {fun(&Application::OPTION_WindowSize),  "OPTION_WindowSize"},
        {fun(&Application::update_options),     "update_options"},
        {fun(&Application::quit),               "quit"},
        {fun(get_overlay), "overlay"},
        {fun(get_console), "console"},
    });

    add_class<DebugOverlay>(*m, "DebugOverlay", {}, {
        {fun(&DebugOverlay::notify), "notify"}
    });

    add_class<ChaiConsole>(*m, "ChaiConsole", {}, {
        {fun(&ChaiConsole::cs_print), "print"},
        {fun(&ChaiConsole::cs_clear), "clear"},
        {fun(&ChaiConsole::cs_history), "history"}
    });

    add_class<MessageBus>(*m, "MessageBus", {}, {
    });

    _engine.add(m);
}

template<class T> inline
vector<T*> uptr_to_ptr_vec(vector<unique_ptr<T>>& _uptrVec) {
    vector<Entity*> vec; vec.reserve(_uptrVec.size());
    for (auto& c : _uptrVec) vec.push_back(c.get());
    return std::move(vec);
}

void sq::chaiscript_setup_entity(ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());

    auto get_children = [](Entity* entity) -> vector<Entity*> {
        return uptr_to_ptr_vec(entity->get_children()); };

    auto get_parent = [](Entity* entity) -> Entity* {
        return entity->get_parent(); };

    add_class<Entity>(*m, "Entity", {}, {
        {fun(get_parent), "parent"}, {fun(get_children), "children"},
        {fun(&Entity::get_attr), "get"}, {fun(&Entity::del_attr), "del"},
        {fun(&Entity::operator[]), "[]"}
    });

    add_class<EntityManager>(*m, "EntityManager", {}, {
        {fun(&EntityManager::create_root), "create_root"},
        {fun(&EntityManager::create), "create"},
        {fun(&EntityManager::configure), "configure"},
        {fun(&EntityManager::destroy), "destroy"},
        {fun(&EntityManager::set_name), "set_name"},
        {fun(&EntityManager::find), "find"}
    });

    chai_add_read_only_vector<Entity*>(*m, "EntityVector");

    m->add(vector_conversion<vector<string>>());
    chai::bootstrap::operators::assign<vector<string>>(m);

    _engine.add(m);
}


void sq::chaiscript_setup_physics(ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());

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

    _engine.add(m);
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

    _m.add(vector_conversion<vector<VecST>>());
    _m.add(type_conversion<VecST, string>(&chai_string<VecST>));

    _m.add(fun<VecST, VecST, VecST>(operator+), "+");
    _m.add(fun<VecST, VecST, VecST>(operator-), "-");
    _m.add(fun<VecST, VecST, VecST>(operator*), "*");
    _m.add(fun<VecST, VecST, VecST>(operator/), "/");
    _m.add(fun<VecST, VecST, T>(operator+), "+");
    _m.add(fun<VecST, VecST, T>(operator-), "-");
    _m.add(fun<VecST, VecST, T>(operator*), "*");
    _m.add(fun<VecST, VecST, T>(operator/), "/");
}

void sq::chaiscript_setup_maths(ChaiEngine& _engine) {
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

    _engine.add(m);
}


void sq::chaiscript_setup_messages(ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());

    chai_add_message_type<msg::Configure_Entity>(*m, "Configure_Entity");

    _engine.add(m);
}
