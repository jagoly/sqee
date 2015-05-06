#include <chaiscript/utility/utility.hpp>

#include "camera.hpp"
#include "../wcoe/world.hpp"
#include "../wcoe/cell.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/spotlight.hpp"
#include "../wcoe/obj/pointlight.hpp"
#include "../wcoe/obj/liquid.hpp"
#include "../wcoe/obj/reflector.hpp"
#include "../rndr/graph.hpp"
#include "scripting.hpp"

using namespace sqt;

using chai::constructor;
using chai::fun;
using chai::type_conversion;
using chai::utility::add_class;

void sqt::cs_setup_main(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<MainCamera>(*m, "Camera", {}, {});

    _cs.add(m);
}

void sqt::cs_setup_wcoe(chai::ChaiScript& _cs) {
    using namespace wcoe;
    chai::ModulePtr m(new chai::Module());
    add_class<SkyBox>(*m, "SkyBox", {},
        {{fun(&SkyBox::enabled), "enabled"},
         {fun(&SkyBox::set_colour), "set_colour"},
         {fun(&SkyBox::set_texture), "set_texture"}
    });

    add_class<Ambient>(*m, "Ambient", {},
        {{fun(&Ambient::enabled), "enabled"},
         {fun(&Ambient::set_colour), "set_colour"}
    });

    add_class<SkyLight>(*m, "SkyLight", {},
        {{fun(&SkyLight::enabled), "enabled"},
         {fun(&SkyLight::set_colour), "set_colour"},
         {fun(&SkyLight::set_direction), "set_direction"}
    });

    add_class<World>(*m, "World", {},
        {{fun(&World::skybox), "skybox"},
         {fun(&World::ambient), "ambient"},
         {fun(&World::skylight), "skylight"},
         {fun(&World::add_cell), "add_cell"},
         {fun(&World::get_cell), "get_cell"},
         {fun(&World::enable_cell), "enable_cell"}
    });

    add_class<Object>(*m, "Object", {},
       {{fun(&Object::name), "name"},
        {fun(&Object::cell), "cell"}
    });

    add_class<ModelStatic>(*m, "ModelStatic", {},
       {{fun(&ModelStatic::update_from_data), "refresh"},
        {fun(&ModelStatic::DAT_pos), "pos"},
        {fun(&ModelStatic::DAT_rot), "rot"},
        {fun(&ModelStatic::DAT_sca), "sca"},
        {fun(&ModelStatic::DAT_shadow), "shadow"},
        {fun(&ModelStatic::DAT_render), "render"},
        {fun(&ModelStatic::DAT_reflect), "reflect"},
        {fun(&ModelStatic::DAT_refract), "refract"},
        {fun(&ModelStatic::DAT_mPath), "mPath"},
        {fun(&ModelStatic::DAT_sPath), "sPath"}
    });

    add_class<Cell>(*m, "Cell", {},
        {{fun(&Cell::load_from_file), "load_from_file"},
         {fun(&Cell::add_object<ModelStatic>), "add_ModelStatic"},
         {fun(&Cell::add_object<ModelSkelly>), "add_ModelSkelly"},
         {fun(&Cell::add_object<SpotLight>),   "add_SpotLight"},
         {fun(&Cell::add_object<PointLight>),  "add_PointLight"},
         {fun(&Cell::add_object<Liquid>),      "add_Liquid"},
         {fun(&Cell::add_object<Reflector>),   "add_Reflector"},
         {fun(&Cell::get_object<ModelStatic>), "get_ModelStatic"},
         {fun(&Cell::get_object<ModelSkelly>), "get_ModelSkelly"},
         {fun(&Cell::get_object<SpotLight>),   "get_SpotLight"},
         {fun(&Cell::get_object<PointLight>),  "get_PointLight"},
         {fun(&Cell::get_object<Liquid>),      "get_Liquid"},
         {fun(&Cell::get_object<Reflector>),   "get_Reflector"}
    });

    _cs.add(m);
}

void sqt::cs_setup_rndr(chai::ChaiScript& _cs) {
    using namespace rndr;
    chai::ModulePtr m(new chai::Module());

    add_class<Graph>(*m, "Graph", {},
        {{fun(&Graph::update_from_world), "update_from_world"}
    });

    _cs.add(m);
}
