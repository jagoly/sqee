#include <sqee/app/WrenPlus.hpp>

#include "Common.hpp"

//============================================================================//

constexpr const char SCRIPT[] = R"wren(

foreign class AlphaObject {

  foreign return_int()
  foreign pass_int(arg)

  foreign beta
  foreign beta=(arg)
}

foreign class BetaObject {

  foreign alpha
  foreign alpha=(arg)
}

class ScriptObject {

  construct new(alpha, beta) {
    _alpha = alpha
    _beta = beta
  }

  get_alpha() { _alpha }
  get_beta() { _beta }

  set_alpha_beta() {
    _alpha.beta = _beta
  }
  set_beta_alpha() {
    _beta.alpha = _alpha
  }

  do_return_int() {
    return _alpha.return_int()
  }
  do_pass_int(arg) {
    _alpha.pass_int(arg)
  }

  set_alpha_beta(arg) {
    _alpha.beta = arg
  }
}

)wren";

//============================================================================//

static int gIntValue = 0;

struct AlphaObject;
struct BetaObject;

struct AlphaObject
{
    int wren_return_int() { return gIntValue; }
    void wren_pass_int(int arg) { gIntValue = arg; }

    double doubleValue = 0.0;
    BetaObject* betaPtr = nullptr;
};

struct BetaObject
{
    AlphaObject* alphaPtr = nullptr;
};

WRENPLUS_TRAITS_HEADER(AlphaObject)
WRENPLUS_TRAITS_HEADER(BetaObject)

WRENPLUS_TRAITS_DEFINITION(AlphaObject, "test", "AlphaObject")
WRENPLUS_TRAITS_DEFINITION(BetaObject, "test", "BetaObject")

//============================================================================//

TEST_CASE("WrenPlus General", "[wrenplus]")
{
    SECTION("Test")
    {
        wren::WrenPlusVM vm;

        WRENPLUS_ADD_METHOD(vm, AlphaObject, wren_return_int, "return_int()");
        WRENPLUS_ADD_METHOD(vm, AlphaObject, wren_pass_int, "pass_int(_)");
        WRENPLUS_ADD_FIELD_RW(vm, AlphaObject, betaPtr, "beta");
        WRENPLUS_ADD_FIELD_RW(vm, BetaObject, alphaPtr, "alpha");

        vm.interpret("test", SCRIPT);
        vm.cache_handles<AlphaObject, BetaObject>();

        vm.validate_class_handle_cache();

        auto alpha = std::make_unique<AlphaObject>();
        auto beta = std::make_unique<BetaObject>();

        WrenHandle* handle_new = wrenMakeCallHandle(vm, "new(_,_)");
        WrenHandle* handle_get_alpha = wrenMakeCallHandle(vm, "get_alpha()");
        WrenHandle* handle_get_beta = wrenMakeCallHandle(vm, "get_beta()");
        WrenHandle* handle_set_alpha_beta = wrenMakeCallHandle(vm, "set_alpha_beta()");
        WrenHandle* handle_set_beta_alpha = wrenMakeCallHandle(vm, "set_beta_alpha()");
        WrenHandle* handle_do_return_int = wrenMakeCallHandle(vm, "do_return_int()");
        WrenHandle* handle_do_pass_int = wrenMakeCallHandle(vm, "do_pass_int(_)");
        WrenHandle* handle_set_alpha_beta1 = wrenMakeCallHandle(vm, "set_alpha_beta(_)");

        WrenHandle* testInstance = vm.call<WrenHandle*>(handle_new, wren::GetVar("test", "ScriptObject"), alpha.get(), beta.get());

        REQUIRE((alpha->betaPtr == nullptr && beta->alphaPtr == nullptr));

        REQUIRE(vm.call<AlphaObject*>(handle_get_alpha, testInstance) == alpha.get());
        REQUIRE(vm.call<BetaObject*>(handle_get_beta, testInstance) == beta.get());

        vm.call_void(handle_set_alpha_beta, testInstance);
        REQUIRE(alpha->betaPtr == beta.get());

        vm.call_void(handle_set_beta_alpha, testInstance);
        REQUIRE(beta->alphaPtr == alpha.get());

        REQUIRE(vm.call<int>(handle_do_return_int, testInstance) == 0);
        vm.call_void(handle_do_pass_int, testInstance, 666);
        REQUIRE(gIntValue == 666);
        REQUIRE(vm.call<int>(handle_do_return_int, testInstance) == 666);

        auto betaX = std::make_unique<BetaObject>();

        vm.call_void(handle_set_alpha_beta1, testInstance, betaX.get());
        REQUIRE(alpha->betaPtr == betaX.get());

        wrenReleaseHandle(vm, handle_new);
        wrenReleaseHandle(vm, handle_get_alpha);
        wrenReleaseHandle(vm, handle_get_beta);
        wrenReleaseHandle(vm, handle_set_alpha_beta);
        wrenReleaseHandle(vm, handle_set_beta_alpha);
        wrenReleaseHandle(vm, handle_do_return_int);
        wrenReleaseHandle(vm, handle_do_pass_int);
        wrenReleaseHandle(vm, handle_set_alpha_beta1);

        wrenReleaseHandle(vm, testInstance);
    }
}
