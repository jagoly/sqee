#include <sqee/app/Application.hpp>

#include <chrono>

using namespace sq;

//============================================================================//

int Application::run(int argc, char** argv)
{
    std::vector<String> args;
    args.reserve(size_t(argc));

    for (int i = 0; i < argc; ++i)
        args.emplace_back(argv[i]);

    initialise(std::move(args));

    //--------------------------------------------------------//

    auto startTime = std::chrono::steady_clock::now();

    while (mReturnCode < 0)
    {
        const auto newTime = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = newTime - startTime;
        startTime = newTime;

        update(elapsed.count());
    }

    //--------------------------------------------------------//

    return mReturnCode;
}
