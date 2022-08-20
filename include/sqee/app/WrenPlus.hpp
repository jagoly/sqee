// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

// some parts based on https://github.com/Nelarius/wrenpp
// should work without sqee with a few small changes

#pragma once

#include <sqee/app/WrenForward.hpp>

#include <wren.hpp> // IWYU pragma: export

#include <fmt/format.h>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <exception>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace wren {

//============================================================================//

/// General exception to throw from functions bound to wren.
struct Exception final : public std::exception
{
    Exception(std::string str) : message(std::move(str)) {}

    template <class... Args>
    Exception(fmt::format_string<Args...> str, Args&&... args) : message(fmt::format(str, std::forward<Args>(args)...)) {}

    const char* what() const noexcept override { return message.c_str(); }

    std::string message;
};

/// Minimal either type used for returning errors as strings.
template <class Type>
struct SafeResult
{
    struct ValueTag {}; struct ErrorTag {};

    template <class Arg>
    SafeResult(ValueTag, Arg&& arg) : value(std::forward<Arg>(arg)), ok(true) {}

    template <class Arg>
    SafeResult(ErrorTag, Arg&& arg) : error(std::forward<Arg>(arg)), ok(false) {}

    ~SafeResult() { if (ok) value.~Type(); else error.~basic_string(); }

    union { Type value; std::string error; };
    const bool ok;
};

/// Pass to call methods instead of using wrenGetVariable.
struct GetVar
{
    GetVar(const char* module, const char* name) : module(module), name(name) {}

    const char* const module;
    const char* const name;
};

//============================================================================//

namespace detail {

/// Convert a c++ callable to a WrenForeignMethodFn.
template <class Callable, Callable> struct MethodWrapper;

/// Allow using c++ member object pointers with MethodWrapper.
template <auto FieldPtr> struct FieldHelper;

template <class Object, class Field, Field Object::*FieldPtr>
struct FieldHelper<FieldPtr>
{
    static auto get(Object* self)
    {
        if constexpr (has_traits_v<Field>) return &(self->*FieldPtr);
        else return self->*FieldPtr;
    }

    static void set(Object* self, Field value)
    {
        self->*FieldPtr = value;
    }
};

} // namespace detail

//============================================================================//

/// Create a new SafeResult with either a value or an error.
template <class Type, bool Ok, class Arg>
inline SafeResult<Type> make_safe_result(Arg&& arg)
{
    if constexpr (Ok == true)
        return SafeResult<Type>(typename SafeResult<Type>::ValueTag(), std::forward<Arg>(arg));

    if constexpr (Ok == false)
        return SafeResult<Type>(typename SafeResult<Type>::ErrorTag(), std::forward<Arg>(arg));
}

/// Get a value of the given type from the specified slot.
template <class Type>
inline std::decay_t<Type> get_slot(WrenVM* vm, int slot)
{
    // decay so that we also support const Type&, Type&&, etc.
    return SlotHelper<std::decay_t<Type>>::get(vm, slot, wrenGetSlotType(vm, slot));
}

/// Set receiver and argument slots for calling a wren method.
template <class Receiver, class... Args>
inline void set_call_slots(WrenVM* vm, Receiver receiver, const Args&... args)
{
    static_assert (
        has_traits_v<std::remove_pointer_t<Receiver>> ||
        std::is_same_v<Receiver, WrenHandle*> || std::is_same_v<Receiver, GetVar>,
        "unsupported receiver, must be an Object*, WrenHandle*, or GetVar"
    );

    // todo: should be a mutable reference, not a pointer
    if constexpr (has_traits_v<std::remove_pointer_t<Receiver>>)
        assert(receiver != nullptr);

    wrenEnsureSlots(vm, 1 + sizeof...(Args));
    SlotHelper<Receiver>::set(vm, 0, receiver);
    int slot = 1;
    (SlotHelper<Args>::set(vm, slot++, args), ...);
}

/// Wrap a member object pointer for use with WrenPlusVM::register_method.
template <auto FieldPtr> constexpr auto FieldGetter = &detail::FieldHelper<FieldPtr>::get;

/// Wrap a member object pointer for use with WrenPlusVM::register_method.
template <auto FieldPtr> constexpr auto FieldSetter = &detail::FieldHelper<FieldPtr>::set;

//============================================================================//

/// RAII wrapper for WrenVM, with some nice extra features.
class WRENPLUS_API WrenPlusVM
{
public: //====================================================//

    WrenPlusVM();

    ~WrenPlusVM();

    //--------------------------------------------------------//

    WrenPlusVM(const WrenPlusVM&) = delete;
    WrenPlusVM& operator=(const WrenPlusVM&) = delete;

    WrenPlusVM(WrenPlusVM&&) = delete;
    WrenPlusVM& operator=(WrenPlusVM&&) = delete;

    //--------------------------------------------------------//

    /// Allows WrenPlusVM& instead of WrenVM* with the wren api.
    operator WrenVM*() { return mWrenVM; }

    /// Access the WrenPlusVM stored in userData of WrenVM.
    static WrenPlusVM& access(WrenVM* vm)
    {
        return *static_cast<WrenPlusVM*>(wrenGetUserData(vm));
    }

    /// Get the class handle for a registered foreign class.
    template <class Object>
    WrenHandle* get_class_handle() const
    {
        return mForeignClassHandles[Traits<Object>::index];
    }

    //--------------------------------------------------------//

    /// Set the list of directories in which wren should search for modules.
    void set_module_import_dirs(std::vector<std::string> dirs)
    {
        mModuleImportDirs = std::move(dirs);
    }

    /// Define a foreign method using either a member or free function pointer.
    template <auto FnPtr, class Object>
    void register_method(const char* signature)
    {
        static_assert (
            (std::is_member_function_pointer_v<decltype(FnPtr)>) ||
            (std::is_pointer_v<decltype(FnPtr)> && std::is_function_v<std::remove_pointer_t<decltype(FnPtr)>>),
            "FnPtr must be a member function pointer or free function"
        );
        static_assert(has_traits_v<Object>, "missing specialisation for wren::Traits");

        using Wrapper = detail::MethodWrapper<decltype(FnPtr), FnPtr>;

        impl_register_method(Traits<Object>::module, Traits<Object>::className, signature, &Wrapper::call);
    }

    /// Allow foreign object pointers to be tested for equality and inequality.
    template <class Object>
    void register_pointer_comparison_operators()
    {
        static_assert(has_traits_v<Object>, "Object must be a foreign class");

        impl_register_method(Traits<Object>::module, Traits<Object>::className, "==(_)", &impl_pointer_equality_operator);
        impl_register_method(Traits<Object>::module, Traits<Object>::className, "!=(_)", &impl_pointer_inequality_operator);
    }

    /// Cache the class handles for one or more classes.
    template <class... Objects>
    void cache_handles()
    {
        static_assert((has_traits_v<Objects> && ...), "missing traits for one or more types");

        (impl_cache_handle(Traits<Objects>::module, Traits<Objects>::className, Traits<Objects>::index), ...);
    }

    //--------------------------------------------------------//

    /// Call a wren method with the given arguments and return the result.
    template <class Result, class Receiver, class... Args>
    Result call(WrenHandle* method, Receiver receiver, const Args&... args)
    {
        set_call_slots<Receiver, Args...>(mWrenVM, receiver, args...);
        mErrorString.clear();

        if (wrenCall(mWrenVM, method) != WREN_RESULT_SUCCESS)
            throw Exception(std::move(mErrorString));

        return get_slot<Result>(mWrenVM, 0);
    }

    /// Call a wren method with the given arguments.
    template <class Receiver, class... Args>
    void call_void(WrenHandle* method, Receiver receiver, const Args&... args)
    {
        set_call_slots<Receiver, Args...>(mWrenVM, receiver, args...);
        mErrorString.clear();

        if (wrenCall(mWrenVM, method) != WREN_RESULT_SUCCESS)
            throw Exception(std::move(mErrorString));
    }

    //--------------------------------------------------------//

    /// Call a wren method with the given arguments and return the result or any errors.
    template <class Result, class Receiver, class... Args>
    SafeResult<Result> safe_call[[nodiscard]](WrenHandle* method, Receiver receiver, const Args&... args) noexcept
    {
        set_call_slots<Receiver, Args...>(mWrenVM, receiver, args...);
        mErrorString.clear();

        if (wrenCall(mWrenVM, method) != WREN_RESULT_SUCCESS)
            return make_safe_result<Result, false>(std::move(mErrorString));

        try {
            return make_safe_result<Result, true>(get_slot<Result>(mWrenVM, 0));
        }
        catch (const std::exception& ex) {
            return make_safe_result<Result, false>(ex.what());
        }
    }

    /// Call a wren method with the given arguments and return any errors.
    template <class Receiver, class... Args>
    std::string safe_call_void[[nodiscard]](WrenHandle* method, Receiver receiver, const Args&... args) noexcept
    {
        set_call_slots<Receiver, Args...>(mWrenVM, receiver, args...);
        mErrorString.clear();

        if (wrenCall(mWrenVM, method) != WREN_RESULT_SUCCESS)
            return std::move(mErrorString);

        return std::string();
    }

    //--------------------------------------------------------//

    /// Wrapper for wrenInterpret that throws.
    void interpret(const char* module, const char* source);

    /// Same as interpret, but return errors as a string.
    std::string safe_interpret[[nodiscard]](const char* module, const char* source);

    /// Interpret a module from file, as if using import.
    void load_module(const char* module);

    /// Same as load_module, but return errors as a string.
    std::string safe_load_module(const char* module);

    /// Makes sure that no foreign class indices are missing.
    void validate_class_handle_cache();

    //--------------------------------------------------------//

    /// Lookup a variable that is known to exist.
    WrenHandle* get_variable(const char* module, const char* variable);

    //--------------------------------------------------------//

    /// Abort the current wren fiber with a formatted error message.
    //template <class... Args>
    //void abort(fmt::format_string<Args...> str, Args&&... args) noexcept
    //{
    //    const std::string message = fmt::format(str, std::forward<Args>(args)...);
    //    wrenEnsureSlots(mWrenVM, 1);
    //    wrenSetSlotBytes(mWrenVM, 0, message.data(), message.length());
    //    wrenAbortFiber(mWrenVM, 0);
    //}

private: //===================================================//

    static void impl_write_fn(WrenVM*, const char*);

    static void impl_error_fn(WrenVM*, WrenErrorType, const char*, int, const char*);

    static WrenForeignClassMethods impl_bind_foreign_class_fn(WrenVM*, const char*, const char*);

    static WrenForeignMethodFn impl_bind_foreign_method_fn(WrenVM*, const char*, const char*, bool, const char*);

    static WrenLoadModuleResult impl_load_module_fn(WrenVM*, const char*);

    //--------------------------------------------------------//

    void impl_register_method(const char* module, const char* className, const char* signature, WrenForeignMethodFn func);

    void impl_cache_handle(const char* module, const char* className, size_t index);

    //--------------------------------------------------------//

    static void impl_pointer_equality_operator(WrenVM* vm);

    static void impl_pointer_inequality_operator(WrenVM* vm);

    //--------------------------------------------------------//

    WrenVM* mWrenVM = nullptr;

    std::vector<std::string> mModuleImportDirs;

    std::map<std::string, WrenForeignMethodFn> mForiegnMethods;

    std::vector<WrenHandle*> mForeignClassHandles;

    std::string mErrorString;
};

//============================================================================//

namespace detail {

/// Generate a new unique index for a foreign type.
WRENPLUS_API size_t generate_type_index() noexcept;

/// Handled in a seperate function to reduce code bloat.
WRENPLUS_API void exception_handler(WrenVM* vm, const std::exception& ex) noexcept;

/// Calls a function with arguments from slots and captures exceptions.
template <class Result, class Object, class... Args, class Func, int... Slots>
inline void invoke_helper(WrenVM* vm, Func func, std::integer_sequence<int, Slots...>) noexcept
{
    // don't need to do any checks since we already know the slot is correct
    Object* self = *static_cast<Object**>(wrenGetSlotForeign(vm, 0));

    try // capture any exceptions and pass them to the handler
    {
        if constexpr (std::is_void_v<Result>)
            std::invoke(func, self, get_slot<Args>(vm, 1 + Slots)...);

        else // set return value
            SlotHelper<std::decay_t<Result>>::set(vm, 0, std::invoke(func, self, get_slot<Args>(vm, 1 + Slots)...));
    }
    catch (const std::exception& ex) { exception_handler(vm, ex); }
}

/// Wrapper for member functions.
template <class Result, class Object, class... Args, Result(Object::*FnPtr)(Args...)>
struct MethodWrapper<Result(Object::*)(Args...), FnPtr>
{
    static void call(WrenVM* vm) noexcept
    {
        invoke_helper<Result, Object, Args...>(vm, FnPtr, std::make_integer_sequence<int, sizeof...(Args)>());
    }
};

/// Wrapper for free functions.
template <class Result, class Object, class... Args, Result(*FnPtr)(Object*, Args...)>
struct MethodWrapper<Result(*)(Object*, Args...), FnPtr>
{
    static void call(WrenVM* vm) noexcept
    {
        invoke_helper<Result, Object, Args...>(vm, FnPtr, std::make_integer_sequence<int, sizeof...(Args)>());
    }
};

} // namespace detail

} // namespace wren

//===== STANDARD SLOT HELPERS ================================================//

/// Specialisation for compile-time null.
template <>
struct wren::SlotHelper<std::nullptr_t>
{
    static void set(WrenVM* vm, int slot, std::nullptr_t)
    {
        wrenSetSlotNull(vm, slot);
    }
};

/// Specialisation for registered foreign classes.
template <class Object>
struct wren::SlotHelper<Object*, std::enable_if_t<wren::has_traits_v<Object>>>
{
    struct TaggedPointer { Object* ptr; size_t index; };

    static Object* get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_FOREIGN)
        {
            const auto& data = *static_cast<TaggedPointer*>(wrenGetSlotForeign(vm, slot));
            if (data.index != Traits<Object>::index)
                throw Exception("slot holds different type of Object");
            return data.ptr;
        }

        if (slotType == WREN_TYPE_NULL)
            return nullptr;

        throw Exception("slot does not hold Object*");
    }

    static void set(WrenVM* vm, int slot, Object* ptr)
    {
        if (ptr != nullptr)
        {
            wrenSetSlotHandle(vm, slot, WrenPlusVM::access(vm).get_class_handle<Object>());
            auto& data = *static_cast<TaggedPointer*>(wrenSetSlotNewForeign(vm, slot, slot, sizeof(TaggedPointer)));
            data = { ptr, Traits<Object>::index };
        }
        else wrenSetSlotNull(vm, slot);
    }
};

/// Specialisation for bools.
template <>
struct wren::SlotHelper<bool>
{
    static bool get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_BOOL)
            return wrenGetSlotBool(vm, slot);

        throw Exception("slot does not hold Bool");
    }

    static void set(WrenVM* vm, int slot, bool value)
    {
        wrenSetSlotBool(vm, slot, value);
    }
};

/// Specialisation for numbers.
template <class Number>
struct wren::SlotHelper<Number, std::enable_if_t<std::is_arithmetic_v<Number>>>
{
    static Number get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_NUM)
        {
            const double value = wrenGetSlotDouble(vm, slot);

            #ifdef WRENPLUS_DEBUG
            if constexpr (std::is_signed_v<Number> == false)
                if (std::signbit(value) == true)
                    throw Exception("number is not positive");

            if constexpr (std::is_integral_v<Number> == true)
                if (std::trunc(value) != value)
                    throw Exception("number is not an integer");
            #endif

            return static_cast<Number>(value);
        }

        throw Exception("slot does not hold Number");
    }

    static void set(WrenVM* vm, int slot, Number value)
    {
        wrenSetSlotDouble(vm, slot, static_cast<double>(value));
    }
};

/// Specialisation for c strings.
template <>
struct wren::SlotHelper<const char*>
{
    static const char* get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_STRING)
            return wrenGetSlotString(vm, slot);

        throw Exception("slot does not hold String");
    }

    static void set(WrenVM* vm, int slot, const char* cstr)
    {
        wrenSetSlotString(vm, slot, cstr);
    }
};

/// Specialisation for c++ string views.
template <>
struct wren::SlotHelper<std::string_view>
{
    static std::string_view get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_STRING)
        {
            int len; auto cstr = wrenGetSlotBytes(vm, slot, &len);
            return std::string_view(cstr, size_t(len));
        }

        throw Exception("slot does not hold String");
    }

    static void set(WrenVM* vm, int slot, const std::string_view& sv)
    {
        wrenSetSlotBytes(vm, slot, sv.data(), sv.length());
    }
};

/// Specialisation for c++ strings.
template <>
struct wren::SlotHelper<std::string>
{
    static std::string get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_STRING)
        {
            int len; auto cstr = wrenGetSlotBytes(vm, slot, &len);
            return std::string(cstr, size_t(len));
        }

        throw Exception("slot does not hold String");
    }

    static void set(WrenVM* vm, int slot, const std::string& str)
    {
        wrenSetSlotBytes(vm, slot, str.data(), str.length());
    }
};

/// Specialisation for opaque wren handles.
template <>
struct wren::SlotHelper<WrenHandle*>
{
    static WrenHandle* get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_UNKNOWN)
            return wrenGetSlotHandle(vm, slot);

        if (slotType == WREN_TYPE_NULL)
            return nullptr;

        throw Exception("slot does not hold WrenHandle");
    }

    static void set(WrenVM* vm, int slot, WrenHandle* handle)
    {
        if (handle != nullptr) wrenSetSlotHandle(vm, slot, handle);
        else wrenSetSlotNull(vm, slot);
    }
};

/// Specialisation for the get variable helper (set only).
template <>
struct wren::SlotHelper<wren::GetVar>
{
    static void set(WrenVM* vm, int slot, const wren::GetVar& var)
    {
        wrenGetVariable(vm, var.module, var.name, slot);
    }
};

/// Specialisation for c++ optionals.
template <class Type>
struct wren::SlotHelper<std::optional<Type>>
{
    static std::optional<Type> get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_NULL)
            return std::nullopt;

        return SlotHelper<Type>::get(vm, slot, slotType);
    }

    static void set(WrenVM* vm, int slot, const std::optional<Type>& opt)
    {
        if (opt.has_value() == true)
            SlotHelper<Type>::set(vm, slot, *opt);

        else wrenSetSlotNull(vm, slot);
    }
};

/// Specialisation for c++ vectors.
template <class Type>
struct wren::SlotHelper<std::vector<Type>>
{
    static std::vector<Type> get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_LIST)
        {
            const int elemCount = wrenGetListCount(vm, slot);

            std::vector<Type> result;
            result.reserve(size_t(elemCount));

            for (int i = 0; i < elemCount; ++i)
            {
                wrenGetListElement(vm, slot, i, 0);
                result.emplace_back(SlotHelper<Type>::get(vm, 0, wrenGetSlotType(vm, 0)));
            }

            return result;
        }

        throw Exception("slot does not hold List");
    }

    // todo: compilicated since we need an extra slot for insertion
    //static void set(WrenVM* vm, int slot, const std::vector<Type>& vec)
    //{
    //    wrenSetSlotNewList(vm, slot);
    //}
};

//===== SQEE SLOT HELPERS ====================================================//

#include <sqee/core/EnumHelper.hpp>

/// Specialisation for the sqee enum helper.
template <class Enum>
struct wren::SlotHelper<Enum, std::enable_if_t<sq::has_enum_traits_v<Enum>>>
{
    static Enum get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_STRING)
        {
            const auto sv = SlotHelper<std::string_view>::get(vm, slot, WREN_TYPE_STRING);
            return sq::enum_from_string<Enum>(sv); // throw if invalid
        }

        throw Exception("slot does not hold Enum (String)");
    }

    static void set(WrenVM* vm, int slot, Enum value)
    {
        const auto str = sq::enum_to_string_safe(value);
        assert(str.has_value() == true);
        SlotHelper<std::string_view>::set(vm, slot, *str);
    }
};

#include <sqee/misc/StackString.hpp>

/// Specialisation for sqee stack strings.
template <size_t Capacity>
struct wren::SlotHelper<sq::StackString<Capacity>>
{
    static sq::StackString<Capacity> get(WrenVM* vm, int slot, WrenType slotType)
    {
        return SlotHelper<std::string_view>::get(vm, slot, slotType);
    }

    static void set(WrenVM* vm, int slot, const sq::StackString<Capacity>& str)
    {
        SlotHelper<const char*>::set(vm, slot, str.c_str());
    }
};

#include <sqee/misc/StackVector.hpp>

/// Specialisation for sqee stack vectors.
template <class Type, size_t Capacity>
struct wren::SlotHelper<sq::StackVector<Type, Capacity>>
{
    static sq::StackVector<Type, Capacity> get(WrenVM* vm, int slot, WrenType slotType)
    {
        if (slotType == WREN_TYPE_LIST)
        {
            const int elemCount = wrenGetListCount(vm, slot);

            if (elemCount > Capacity)
                throw Exception("too many elements for StackVector");

            sq::StackVector<Type, Capacity> result;

            for (int i = 0; i < elemCount; ++i)
            {
                wrenGetListElement(vm, slot, i, 0);
                result.emplace_back(SlotHelper<Type>::get(vm, 0, wrenGetSlotType(vm, 0)));
            }

            return result;
        }

        throw Exception("slot does not hold List");
    }

    // todo: compilicated since we need an extra slot for insertion
    //static void set(WrenVM* vm, int slot, const sq::StackVector<Type, Capacity>& vec)
    //{
    //    wrenSetSlotNewList(vm, slot);
    //}
};
