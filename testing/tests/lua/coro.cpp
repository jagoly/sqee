#include "../../catch.hpp"

#include <wren.hpp>

#include <fmt/format.h>

//============================================================================//

constexpr const char SCRIPT[] = R"wren(

System.print("hello...")

class ScriptObject {
  static execute(action) {
    System.print("...world")
  }
}

)wren";

void write_fn(WrenVM*, const char* text)
{
    fmt::print(text);
}

void error_fn(WrenVM*, WrenErrorType type, const char* module, int line, const char* message)
{
    if (module == nullptr) module = "NULL";
    fmt::print("code {} | module {} | line {}\n > {}\n", type, module, line, message);
}

static WrenForeignMethodFn bind_foreign_method_fn(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature)
{
    return nullptr;
}

//============================================================================//

TEST_CASE("Wren General", "[wren]")
{
    SECTION("Test")
    {
        WrenConfiguration config;
        wrenInitConfiguration(&config);

        config.writeFn = &write_fn;
        config.errorFn = &error_fn;
        config.bindForeignMethodFn = &bind_foreign_method_fn;

        WrenVM* vm = wrenNewVM(&config);

        std::vector<WrenHandle*> classHandles;
        std::vector<WrenHandle*> executeHandles;

        for (int i = 0; i < 2000; ++i)
        {
            const std::string module = fmt::format("TestAction_{}", i);

            const WrenInterpretResult ec = wrenInterpret(vm, module.c_str(), SCRIPT);
            fmt::print("interpret result {} = {}\n", i, ec);

            wrenEnsureSlots(vm, 1);
            wrenGetVariable(vm, module.c_str(), "ScriptObject", 0);
            classHandles.push_back(wrenGetSlotHandle(vm, 0));

            executeHandles.push_back(wrenMakeCallHandle(vm, "execute(_)"));
        }

        for (WrenHandle* handle : classHandles)
            wrenReleaseHandle(vm, handle);

        for (WrenHandle* handle : executeHandles)
            wrenReleaseHandle(vm, handle);

        wrenFreeVM(vm);
    }
}
