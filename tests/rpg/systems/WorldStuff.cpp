#include <sqee/render/Mesh.hpp>
#include <sqee/render/Material.hpp>
#include <sqee/render/Armature.hpp>

#include "WorldStuff.hpp"

namespace sqt { namespace sys {

WorldStuff::WorldStuff() = default;
WorldStuff::~WorldStuff() = default;

WorldStuff& static_WorldStuff()
{
    static WorldStuff stuff;
    return stuff;
}

}} // namespace sqt::sys
