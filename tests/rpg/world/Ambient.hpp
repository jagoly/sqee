#pragma once

#include <sqee/maths/Builtins.hpp>

namespace sqt::world {

//============================================================================//

class Ambient final : sq::NonCopyable
{
public: //====================================================//

    /// Set the ambient light colour.
    void set_colour(Vec3F colour);

    //--------------------------------------------------------//

    /// Get the ambient light colour.
    Vec3F get_colour() const { return mColour; }

private: //===================================================//

    Vec3F mColour = { 1.f, 1.f, 1.f };
};

//============================================================================//

} // namespace sqt::world
