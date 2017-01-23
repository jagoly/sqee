#pragma once

#include <sqee/builtins.hpp>

#include "WorldStuff.hpp"

namespace sqt {
namespace sys {

template <class T, uint X, uint Y>
using Array2D = array<array<T, Y>, X>;

//struct LightCascData
//{
//    Array2D<Vec4F, 6, 4> planes;
//    uint cascadeCount;
//};

//struct LightOrthoData
//{
//    array<Vec4F, 4> planes;
//};


struct SpotLightEdit
{
    SpotLightEdit(WorldStuff& stuff, int32_t id);

    //========================================================//

    sq::dop::Entry<SpotLightData> mEntry;
    WorldStuff& mStuff;

    //========================================================//

    void set_colour(Vec3F colour);
    void set_softness(float softness);
    void set_angle(float angle);
    void set_resolution(uint resolution);
};

class LightingSystem : sq::NonCopyable
{
public:

    void system_refresh();

private:

    LightingSystem(); // private constructor
    friend LightingSystem& static_LightingSystem();

    WorldStuff& mStuff;
};

LightingSystem& static_LightingSystem();

}} // namespace sqt::sys
