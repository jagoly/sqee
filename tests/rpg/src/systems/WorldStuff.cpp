#include "WorldStuff.hpp"

using namespace sqt;

//============================================================================//

WorldStuff::WorldStuff()
{
    skybox = std::make_unique<world::Skybox>();
    ambient = std::make_unique<world::Ambient>();

    skylight = std::make_unique<world::Skylight>(camera);
}

WorldStuff::~WorldStuff() = default;
