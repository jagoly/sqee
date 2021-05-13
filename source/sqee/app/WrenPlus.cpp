#include <sqee/app/WrenPlus.hpp>

#include <sqee/core/Utilities.hpp> // sq::build_string
#include <sqee/debug/Assert.hpp>   // SQASSERT
#include <sqee/misc/Files.hpp>     // sq::try_get_string_from_file

using namespace wren;

//============================================================================//

WrenPlusVM::WrenPlusVM()
{
    WrenConfiguration config;
    wrenInitConfiguration(&config);

    config.userData = this;

    config.writeFn = &WrenPlusVM::impl_write_fn;
    config.errorFn = &WrenPlusVM::impl_error_fn;

    config.bindForeignClassFn = &WrenPlusVM::impl_bind_foreign_class_fn;
    config.bindForeignMethodFn = &WrenPlusVM::impl_bind_foreign_method_fn;

    config.loadModuleFn = &WrenPlusVM::impl_load_module_fn;

    mWrenVM = wrenNewVM(&config);
}

WrenPlusVM::~WrenPlusVM()
{
    wrenFreeVM(mWrenVM);
    mWrenVM = nullptr;
}

//============================================================================//

void WrenPlusVM::interpret(const char* module, const char* source)
{
    mErrorString.clear();

    if (wrenInterpret(*this, module, source) != WREN_RESULT_SUCCESS)
        throw Exception(std::move(mErrorString));
}

std::optional<std::string> WrenPlusVM::safe_interpret(const char* module, const char* source)
{
    mErrorString.clear();

    if (wrenInterpret(*this, module, source) != WREN_RESULT_SUCCESS)
    {
        SQASSERT(!mErrorString.empty(), "failed with no errors");
        return std::move(mErrorString);
    }

    SQASSERT(mErrorString.empty(), "succeeded with errors");
    return std::nullopt;
}

//============================================================================//

void WrenPlusVM::impl_write_fn(WrenVM* /*vm*/, const char* text)
{
    std::fwrite(text, 1u, std::strlen(text), stdout);
    std::fflush(stdout);
}

void WrenPlusVM::impl_error_fn(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message)
{
    const char* error = "UNKNOWN";
    if (type == WREN_ERROR_COMPILE) error = "COMPILE";
    if (type == WREN_ERROR_RUNTIME) error = "RUNTIME";
    if (type == WREN_ERROR_STACK_TRACE) error = "STACKTRACE";
    if (module == nullptr) module = "NULL";

    WrenPlusVM::access(vm).mErrorString += fmt::format("{} | {}:{} | {}\n", error, module, line, message);
}

//============================================================================//

WrenForeignClassMethods WrenPlusVM::impl_bind_foreign_class_fn(WrenVM* /*vm*/, const char* /*module*/, const char* /*className*/)
{
    // SuperTuxSmash doesn't need to allocate foreign classes inside wren
    return { nullptr, nullptr };
}

WrenForeignMethodFn WrenPlusVM::impl_bind_foreign_method_fn(WrenVM* vm, const char* module, const char* className,
                                                            bool isStatic, const char* signature)
{
    // SuperTuxSmash doesn't need static foreign methods
    if (isStatic == true) return nullptr;

    WrenPlusVM& pvm = WrenPlusVM::access(vm);

    const auto key = sq::build_string(module, '.', className, '.', signature);
    const auto iter = pvm.mForiegnMethods.find(key);

    return iter != pvm.mForiegnMethods.end() ? iter->second : nullptr;
}

WrenLoadModuleResult WrenPlusVM::impl_load_module_fn(WrenVM*, const char* name)
{
    const auto path = sq::build_string("wren/", name, ".wren");

    const auto maybe = sq::try_read_text_from_file(path);

    if (maybe.has_value() == false)
        return { nullptr, nullptr, nullptr };

    auto ptr = new std::string(std::move(*maybe));

    WrenLoadModuleResult result;
    result.source = ptr->c_str();
    result.userData = ptr;

    result.onComplete = [](WrenVM*, const char*, WrenLoadModuleResult result)
    {
        delete static_cast<std::string*>(result.userData);
    };

    return result;
}

//============================================================================//

void WrenPlusVM::impl_register_method(const char* module, const char* className, const char* signature, WrenForeignMethodFn func)
{
    const auto key = sq::build_string(module, '.', className, '.', signature);

    SQASSERT(mForiegnMethods.find(key) == mForiegnMethods.end(), "already registered");

    mForiegnMethods[key] = func;
}
