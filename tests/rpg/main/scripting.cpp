#include <chaiscript/utility/utility.hpp>

#include "camera.hpp"
#include "../rndr/graph.hpp"
#include "../wcoe/world.hpp"
#include "../wcoe/cell.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/pointlight.hpp"
#include "../wcoe/obj/spotlight.hpp"
#include "../wcoe/obj/reflector.hpp"
#include "../wcoe/obj/liquid.hpp"
#include "../wcoe/obj/decal.hpp"
#include "scripting.hpp"

using namespace sqt;

using chai::fun;
using chai::base_class;
using chai::constructor;
using chai::type_conversion;
using chai::utility::add_class;

void sqt::cs_setup_main(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<MainCamera>(*m, "MainCamera", {}, {});

    _cs.add(m);
}

void sqt::cs_setup_wcoe(chai::ChaiScript& _cs) {
    using namespace wcoe;
    chai::ModulePtr m(new chai::Module());

    add_class<SkyBox>(*m, "SkyBox", {},
        {{fun(&SkyBox::DAT_enabled), "enabled"},
         {fun(&SkyBox::DAT_colour), "colour"},
         {fun(&SkyBox::DAT_texPath), "texture"},
         {fun(&SkyBox::refresh), "refresh"}
    });

    add_class<Ambient>(*m, "Ambient", {},
        {{fun(&Ambient::DAT_enabled), "enabled"},
         {fun(&Ambient::DAT_colour), "colour"},
         {fun(&Ambient::refresh), "refresh"}
    });

    add_class<SkyLight>(*m, "SkyLight", {},
        {{fun(&SkyLight::DAT_enabled), "enabled"},
         {fun(&SkyLight::DAT_colour), "colour"},
         {fun(&SkyLight::DAT_normal), "normal"},
         {fun(&SkyLight::refresh), "refresh"}
    });

    add_class<World>(*m, "World", {},
        {{fun(&World::skybox), "skybox"},
         {fun(&World::ambient), "ambient"},
         {fun(&World::skylight), "skylight"},
         {fun(&World::add_cell), "add_cell"},
         {fun(&World::get_cell), "get_cell"},
         {fun(&World::reload_list), "reload_list"},
         {fun(&World::refresh), "refresh"}
    });

    add_class<Object>(*m, "Object", {},
       {{fun(&Object::name), "name"},
        {fun(&Object::cell), "cell"},
        {fun(&Object::refresh), "refresh"}
    });

    add_class<ModelStatic>(*m, "ModelStatic", {},
       {{fun(&ModelStatic::DAT_pos), "pos"},
        {fun(&ModelStatic::DAT_rot), "rot"},
        {fun(&ModelStatic::DAT_sca), "sca"},
        {fun(&ModelStatic::DAT_shadow), "shadow"},
        {fun(&ModelStatic::DAT_render), "render"},
        {fun(&ModelStatic::DAT_reflect), "reflect"},
        {fun(&ModelStatic::DAT_refract), "refract"},
        {fun(&ModelStatic::DAT_mPath), "mesh"},
        {fun(&ModelStatic::DAT_sPath), "skin"}
    });

    add_class<Decal>(*m, "Decal", {},
       {{fun(&Decal::DAT_position), "position"},
        {fun(&Decal::DAT_normal), "normal"},
        {fun(&Decal::DAT_tangent), "tangent"},
        {fun(&Decal::DAT_scale), "scale"},
        {fun(&Decal::DAT_dPath), "diff"},
        {fun(&Decal::DAT_nPath), "norm"},
        {fun(&Decal::DAT_sPath), "spec"}
    });

    add_class<Cell>(*m, "Cell", {},
        {{fun(&Cell::DAT_enabled), "enabled"},
         {fun(&Cell::DAT_position), "position"},
         {fun(&Cell::load_from_file), "load_from_file"},
         {fun(&Cell::add_object<ModelStatic>), "add_ModelStatic"},
         {fun(&Cell::get_object<ModelStatic>), "get_ModelStatic"},
         {fun(&Cell::add_object<ModelSkelly>), "add_ModelSkelly"},
         {fun(&Cell::get_object<ModelSkelly>), "get_ModelSkelly"},
         {fun(&Cell::add_object<PointLight>),  "add_PointLight"},
         {fun(&Cell::get_object<PointLight>),  "get_PointLight"},
         {fun(&Cell::add_object<SpotLight>),   "add_SpotLight"},
         {fun(&Cell::get_object<SpotLight>),   "get_SpotLight"},
         {fun(&Cell::add_object<Reflector>),   "add_Reflector"},
         {fun(&Cell::get_object<Reflector>),   "get_Reflector"},
         {fun(&Cell::add_object<Liquid>),      "add_Liquid"},
         {fun(&Cell::get_object<Liquid>),      "get_Liquid"},
         {fun(&Cell::add_object<Decal>),       "add_Decal"},
         {fun(&Cell::get_object<Decal>),       "get_Decal"},
         {fun(&Cell::refresh), "refresh"}
    });

    _cs.add(m);
    _cs.add(base_class<Object, ModelStatic>());
    _cs.add(base_class<Object, Decal>());
}

void sqt::cs_setup_rndr(chai::ChaiScript& _cs) {
    using namespace rndr;
    chai::ModulePtr m(new chai::Module());

    add_class<Graph>(*m, "Graph", {},
        {{fun(&Graph::reload_lists), "reload_lists"}
    });

    _cs.add(m);
}
