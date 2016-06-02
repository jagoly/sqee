#include <chaiscript/chaiscript_stdlib.hpp>

#include <sqee/scripts/ChaiScript.hpp>

using namespace sq;

std::unique_ptr<ChaiEngine> sq::create_ChaiEngine() {
    return std::make_unique<ChaiEngine>(chai::Std_Lib::library());
}
