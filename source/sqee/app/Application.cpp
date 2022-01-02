#include <sqee/app/Application.hpp>

#include <chrono>

using namespace sq;

//============================================================================//

int Application::run(int argc, char** argv)
{
    initialise(std::vector<String>(argv, argv + argc));

    //--------------------------------------------------------//

    auto startTime = std::chrono::steady_clock::now();

    while (mReturnCode < 0)
    {
        if (mCallNextFrame != nullptr)
        {
            mCallNextFrame();
            mCallNextFrame = nullptr;
        }

        const auto newTime = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = newTime - startTime;
        startTime = newTime;

        update(elapsed.count());
    }

    //--------------------------------------------------------//

    return mReturnCode;
}
