#include <sqee/app/Application.hpp>

#include <sqee/debug/Logging.hpp>

#include <chrono>
#include <thread>

#define STRINGIFY_INNER(Arg) #Arg
#define STRINGIFY(Arg) STRINGIFY_INNER(Arg)

using namespace sq;

//============================================================================//

int Application::run(int argc, char** argv)
{
  #if defined(SQEE_LINUX)
    constexpr auto platform = "Linux x86_64";
  #elif defined(SQEE_WINDOWS)
    constexpr auto platform = "Windows x86_64";
  #endif

  #if defined(SQEE_GNU)
    constexpr auto compiler = "GCC " STRINGIFY(__GNUC__) "." STRINGIFY(__GNUC_MINOR__) "." STRINGIFY(__GNUC_PATCHLEVEL__);
  #elif defined(SQEE_CLANG)
    constexpr auto compiler = "Clang " STRINGIFY(__clang_major__) "." STRINGIFY(__clang_minor__) "." STRINGIFY(__clang_patchlevel__);
  #elif defined(SQEE_MSVC)
    constexpr auto compiler = "MSVC " STRINGIFY(_MSC_FULL_VER);
  #endif

  #if defined(SQEE_STATIC_LIB)
    constexpr auto linkMode = "Static";
  #else
    constexpr auto linkMode = "Shared";
  #endif

  #if defined(SQEE_DEBUG)
    constexpr auto buildMode = "Debug";
  #else
    constexpr auto buildMode = "Release";
  #endif

    log_info("Platform: {} | Compiler: {} | Config: {} {}", platform, compiler, linkMode, buildMode);

    initialise(std::vector<String>(argv, argv + argc));

    //--------------------------------------------------------//

    // seconds stored as a double
    using Seconds = std::chrono::duration<double>;

    auto previousTime = std::chrono::steady_clock::now();

    while (mReturnCode < 0)
    {
        if (mCallNextFrame != nullptr)
        {
            mCallNextFrame();
            mCallNextFrame = nullptr;
        }

        const auto newTime = std::chrono::steady_clock::now();
        const Seconds elapsed = newTime - previousTime;
        previousTime = newTime;

        update(elapsed.count());

        mElapsedSinceActivity += elapsed.count();

        if (mElapsedSinceActivity >= mMaxInactivePeriod && elapsed.count() < mMinSleepUpdatePeriod)
            std::this_thread::sleep_for(Seconds(mMinSleepUpdatePeriod) - elapsed);
    }

    //--------------------------------------------------------//

    return mReturnCode;
}
