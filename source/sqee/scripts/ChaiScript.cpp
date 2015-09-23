#include <chaiscript/chaiscript_stdlib.hpp>

#include <sqee/scripts/ChaiScript.hpp>

using namespace sq;

chai::ChaiScript* sq::make_ChaiScript() {
    return new chai::ChaiScript(chai::Std_Lib::library());
}
