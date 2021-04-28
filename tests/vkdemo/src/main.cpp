#ifdef SQEE_WINDOWS
#include <sqee/redist/backward.hpp>
namespace backward { SignalHandling sh; }
#endif

//============================================================================//

#include "DemoApp.hpp"

int main(int argc, char** argv)
{
    demo::DemoApp app;

    return app.run(argc, argv);
}
