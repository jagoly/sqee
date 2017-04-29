#include <sqee/scripts/ChaiEngine.hpp>

using namespace sq;

//============================================================================//

std::unique_ptr<ChaiEngine> sq::create_ChaiEngine()
{
    const std::vector<std::string> modulePaths = {};
    const std::vector<std::string> usePaths = {};
    const auto chaiOptions = chai::default_options();

    return std::make_unique<ChaiEngine>(modulePaths, usePaths, chaiOptions);
}
