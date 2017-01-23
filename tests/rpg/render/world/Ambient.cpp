#include <sqee/redist/gl_ext_4_2.hpp>

#include "Ambient.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

void AmbientData::refresh(const world::AmbientObject& _obj) {
    colour = _obj.PROP_colour;
}
