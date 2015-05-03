#include <chaiscript/utility/utility.hpp>

#include "camera.hpp"
#include "../wcoe/world.hpp"
#include "../wcoe/cell.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/lightsky.hpp"
#include "../wcoe/obj/lightspot.hpp"
#include "../wcoe/obj/lightpoint.hpp"
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

    add_class<World>(*m, "World", {}, {});

    add_class<Object>(*m, "Object", {},
       {{fun(&Object::name), "name"},
        {fun(&Object::cell), "cell"}
    });

    add_class<Cell>(*m, "Cell", {},
        {{fun(&Cell::add_object<ModelStatic>), "add_ModelStatic"},
         {fun(&Cell::add_object<ModelSkelly>), "add_ModelSkelly"},
         {fun(&Cell::add_object<LightSky>),    "add_LightSky"},
         {fun(&Cell::add_object<LightSpot>),   "add_LightSpot"},
         {fun(&Cell::add_object<LightPoint>),  "add_LightPoint"},
         {fun(&Cell::add_object<Liquid>),      "add_Liquid"},
         {fun(&Cell::add_object<Reflector>),   "add_Reflector"}
    });

    _cs.add(m);
}

void sqt::cs_setup_rndr(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<rndr::Graph>(*m, "Graph", {}, {});

    _cs.add(m);
}
