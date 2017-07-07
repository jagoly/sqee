#pragma once

#include <sqee/setup.hpp>

namespace sq {

//============================================================================//

/// The SQEE Scene base class.
class Scene : NonCopyable
{
public: //====================================================//

    /// Constructor.
    Scene(double tickTime);

    /// Virtual Destructor.
    virtual ~Scene() = default;

    //--------------------------------------------------------//

    /// Call to update and then render the scene.
    void update_and_render(double elapsed);

protected: //=================================================//

    /// Implement to simulate one tick.
    virtual void update() = 0;

    /// Implement to render the scene.
    virtual void render(double elapsed) = 0;

    //--------------------------------------------------------//

    const double mTickTime;

    double mAccumulation = 0.0;
};

//============================================================================//

} // namespace sq
