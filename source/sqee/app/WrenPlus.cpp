#include <sqee/app/WrenPlus.hpp>

// for use without sqee, replace with your own file loading
#include <sqee/misc/Files.hpp>

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

//============================================================================//

WrenPlusVM::~WrenPlusVM()
{
    for (WrenHandle* handle : mForeignClassHandles)
        if (handle != nullptr)
            wrenReleaseHandle(mWrenVM, handle);

    wrenFreeVM(mWrenVM);
    mWrenVM = nullptr;
}

//============================================================================//

void WrenPlusVM::interpret(const char* module, const char* source)
{
    mErrorString.clear();

    if (wrenInterpret(mWrenVM, module, source) != WREN_RESULT_SUCCESS)
        throw Exception(std::move(mErrorString));
}

std::string WrenPlusVM::safe_interpret(const char* module, const char* source)
{
    mErrorString.clear();

    if (wrenInterpret(mWrenVM, module, source) != WREN_RESULT_SUCCESS)
        return std::move(mErrorString);

    return std::string();
}

void WrenPlusVM::load_module(const char* module)
{
    if (wrenHasModule(mWrenVM, module) == true)
        return; // already loaded

    std::optional<std::string> source;

    for (const auto& dir : mModuleImportDirs)
        if ((source = sq::try_read_text_from_file(fmt::format("{}/{}.wren", dir, module))))
            break; // file found

    if (source.has_value() == false)
        throw Exception("could not find module '{}'", module);

    mErrorString.clear();

    if (wrenInterpret(mWrenVM, module, source->c_str()) != WREN_RESULT_SUCCESS)
        throw Exception(std::move(mErrorString));
}

void WrenPlusVM::validate_class_handle_cache()
{
    mForeignClassHandles.shrink_to_fit();

    for (size_t index = 0u; index < mForeignClassHandles.size(); ++index)
        assert(mForeignClassHandles[index] != nullptr); // missing class registration for index
}

//============================================================================//

void WrenPlusVM::impl_write_fn(WrenVM* /*vm*/, const char* text)
{
    std::fwrite(text, 1u, std::strlen(text), stdout);
    std::fflush(stdout);
}

void WrenPlusVM::impl_error_fn(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message)
{
    if (type == WREN_ERROR_COMPILE)
    {
        assert(module != NULL && line != -1);
        fmt::format_to (
            std::back_inserter(WrenPlusVM::access(vm).mErrorString),
            "COMPILE | {}:{} | {}\n", module, line, message
        );
    }
    else if (type == WREN_ERROR_RUNTIME)
    {
        assert(module == NULL && line == -1);
        fmt::format_to (
            std::back_inserter(WrenPlusVM::access(vm).mErrorString),
            "RUNTIME | {}\n", message
        );
    }
    else if (type == WREN_ERROR_STACK_TRACE)
    {
        assert(module != NULL && line != -1);
        fmt::format_to (
            std::back_inserter(WrenPlusVM::access(vm).mErrorString),
            "STACKTRACE | {}:{} | {}\n", module, line, message
        );
    }
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

    const auto key = fmt::format("{}.{}.{}", module, className, signature);
    const auto iter = pvm.mForiegnMethods.find(key);

    return iter != pvm.mForiegnMethods.end() ? iter->second : nullptr;
}

//============================================================================//

WrenLoadModuleResult WrenPlusVM::impl_load_module_fn(WrenVM* vm, const char* module)
{
    WrenPlusVM& pvm = WrenPlusVM::access(vm);

    std::optional<std::string> source;

    for (const auto& dir : pvm.mModuleImportDirs)
        if ((source = sq::try_read_text_from_file(fmt::format("{}/{}.wren", dir, module))))
            break; // file found

    if (source.has_value() == false)
        return { nullptr, nullptr, nullptr };

    std::string* ptr = new std::string(std::move(*source));

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
    auto key = fmt::format("{}.{}.{}", module, className, signature);

    const auto [iter, ok] = mForiegnMethods.try_emplace(std::move(key), func);
    (void)(iter); (void)(ok);

    assert(ok == true); // method already registered with key
}

void WrenPlusVM::impl_cache_handle(const char* module, const char* className, size_t index)
{
    if (index >= mForeignClassHandles.size())
        mForeignClassHandles.resize(index + 1u, nullptr);

    assert(mForeignClassHandles[index] == nullptr); // class already registered with index

    wrenEnsureSlots(mWrenVM, 1);
    wrenGetVariable(mWrenVM, module, className, 0);
    mForeignClassHandles[index] = wrenGetSlotHandle(mWrenVM, 0);
}

//============================================================================//

void WrenPlusVM::impl_pointer_equality_operator(WrenVM* vm)
{
    struct TaggedPointer { void* ptr; size_t index; };

    if (wrenGetSlotType(vm, 1) == WREN_TYPE_FOREIGN)
    {
        const auto& lhs = *static_cast<TaggedPointer*>(wrenGetSlotForeign(vm, 0));
        const auto& rhs = *static_cast<TaggedPointer*>(wrenGetSlotForeign(vm, 1));
        if (lhs.ptr == rhs.ptr) { wrenSetSlotBool(vm, 0, true); return; }
    }

    wrenSetSlotBool(vm, 0, false);
}

void WrenPlusVM::impl_pointer_inequality_operator(WrenVM* vm)
{
    struct TaggedPointer { void* ptr; size_t index; };

    if (wrenGetSlotType(vm, 1) == WREN_TYPE_FOREIGN)
    {
        const auto& lhs = *static_cast<TaggedPointer*>(wrenGetSlotForeign(vm, 0));
        const auto& rhs = *static_cast<TaggedPointer*>(wrenGetSlotForeign(vm, 1));
        if (lhs.ptr == rhs.ptr) { wrenSetSlotBool(vm, 0, false); return; }
    }

    wrenSetSlotBool(vm, 0, true);
}

//============================================================================//

size_t wren::detail::generate_type_index() noexcept
{
    static size_t index = 0u;
    return index++;
}

void wren::detail::exception_handler(WrenVM* vm, const std::exception& ex) noexcept
{
    const auto message = fmt::format("exception: {}", ex.what());
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBytes(vm, 0, message.data(), message.size());
    wrenAbortFiber(vm, 0);
}
