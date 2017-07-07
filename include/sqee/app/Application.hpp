#pragma once

#include <sqee/builtins.hpp>

namespace sq {

//============================================================================//

/// The SQEE Application base class.
class Application : NonCopyable
{
public: //====================================================//

    /// Constructor.
    Application() = default;

    /// Virtual Destructor.
    virtual ~Application() = default;

    //--------------------------------------------------------//

    /// Launch the main application loop.
    int run(int argc, char** argv);

protected: //=================================================//

    /// Called once before the main loop starts.
    virtual void initialise(std::vector<string> args) = 0;

    /// Called continuously while running.
    virtual void update(double elapsed) = 0;

    //--------------------------------------------------------//

    /// Set to a non-negative value to quit.
    int mReturnCode = -1;
};

//============================================================================//

} // namespace sq
