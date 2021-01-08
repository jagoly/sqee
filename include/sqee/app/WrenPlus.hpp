// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

// some parts based on https://github.com/Nelarius/wrenpp
// should work without sqee with a few small changes

#pragma once

#include <sqee/export.hpp>

#include <wren.hpp> // IWYU pragma: export

#include <fmt/format.h>

#include <exception>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace wren {

//============================================================================//

/// Specialise this for each foreign class you want to add to wren.
template <class Type> struct Traits : std::false_type {};

/// Check if Traits has been specialised for a given type.
template <class Type> constexpr bool has_traits_v = Traits<Type>::value;

/// Overloads for getting, setting and checking wren slots.
template <class Type, class = void> struct SlotHelper;

/// Convert a normal c++ function to a WrenForeignMethodFn.
template <class Callable, Callable> struct MethodWrapper;

/// Pass to call helpers instead of using wrenGetVariable.
struct GetVar { const char* const module; const char* const name; };

//============================================================================//

/// Basically a crappy Either type, good enough for now.
template <class T> struct SafeResult
{
    SafeResult(std::optional<T> _value) : value(_value) {}
    SafeResult(std::string&& _errors) : errors(_errors) {}
    std::optional<T> value = std::nullopt;
    std::string errors = "";
};

//============================================================================//

/// General exception to throw from functions bound to wren.
class Exception : public std::exception
{
public: //====================================================//

    Exception(std::string&& str) : mMessage(str) {}

    template <class... Args>
    Exception(std::string_view str, Args&&... args) : mMessage(fmt::format(str, std::forward<Args>(args)...)) {}

    const char* what() const noexcept override { return mMessage.c_str(); }

private: //===================================================//

    std::string mMessage;
};

//============================================================================//

/// Specialisation for registered foreign classes.
template <class Ptr>
struct SlotHelper<Ptr, std::enable_if_t<has_traits_v<std::remove_pointer_t<Ptr>>>>
{
    // todo: this should check for the right kind of foreign object
    static bool check(WrenVM* vm, int slot)
    {
        return wrenGetSlotType(vm, slot) == WREN_TYPE_FOREIGN;
    }

    static Ptr get(WrenVM* vm, int slot)
    {
        return *static_cast<Ptr*>(wrenGetSlotForeign(vm, slot));
    }

    // todo: could be better, find a way to cache the class lookup
    static void set(WrenVM* vm, int slot, Ptr ptr)
    {
        using Type = std::remove_pointer_t<Ptr>;
        wrenGetVariable(vm, wren::Traits<Type>::module, wren::Traits<Type>::className, slot);
        void* ptrToPtr = wrenSetSlotNewForeign(vm, slot, slot, sizeof(void*));
        *static_cast<Ptr*>(ptrToPtr) = ptr;
    }
};

/// Specialisation for number and bool types.
template <class Type>
struct SlotHelper<Type, std::enable_if_t<std::is_arithmetic_v<Type>>>
{
    static bool check(WrenVM* vm, int slot)
    {
        if constexpr (std::is_same_v<Type, bool>) return wrenGetSlotType(vm, slot) == WREN_TYPE_BOOL;
        else return wrenGetSlotType(vm, slot) == WREN_TYPE_NUM;
    }

    static Type get(WrenVM* vm, int slot)
    {
        if constexpr (std::is_same_v<Type, bool>) return wrenGetSlotBool(vm, slot);
        else return static_cast<Type>(wrenGetSlotDouble(vm, slot));
    }

    static void set(WrenVM* vm, int slot, Type value)
    {
        if constexpr (std::is_same_v<Type, bool>) wrenSetSlotBool(vm, slot, value);
        else wrenSetSlotDouble(vm, slot, static_cast<double>(value));
    }
};

/// Specialisation for c strings.
template <>
struct SlotHelper<const char*>
{
    static bool check(WrenVM* vm, int slot)
    {
        return wrenGetSlotType(vm, slot) == WREN_TYPE_STRING;
    }

    static const char* get(WrenVM* vm, int slot)
    {
        return wrenGetSlotString(vm, slot);
    }

    static void set(WrenVM* vm, int slot, const char* cstr)
    {
        wrenSetSlotString(vm, slot, cstr);
    }
};

/// Specialisation for generic wren handles.
template <>
struct SlotHelper<WrenHandle*>
{
    static bool check(WrenVM* vm, int slot)
    {
        return wrenGetSlotType(vm, slot) == WREN_TYPE_UNKNOWN;
    }

    static WrenHandle* get(WrenVM* vm, int slot)
    {
        return wrenGetSlotHandle(vm, slot);
    }

    static void set(WrenVM* vm, int slot, WrenHandle* handle)
    {
        wrenSetSlotHandle(vm, slot, handle);
    }
};

/// Specialisation for get variable helpers.
template <>
struct SlotHelper<GetVar>
{
    static bool check(WrenVM* vm, int slot) = delete;

    static WrenHandle* get(WrenVM* vm, int slot) = delete;

    static void set(WrenVM* vm, int slot, const GetVar& var)
    {
        wrenGetVariable(vm, var.module, var.name, slot);
    }
};

/// Specialisation for c++ string views.
template <>
struct SlotHelper<std::string_view>
{
    static bool check(WrenVM* vm, int slot)
    {
        return wrenGetSlotType(vm, slot) == WREN_TYPE_STRING;
    }

    static std::string_view get(WrenVM* vm, int slot)
    {
        int len; auto cstr = wrenGetSlotBytes(vm, slot, &len);
        return std::string_view(cstr, size_t(len));
    }

    static void set(WrenVM* vm, int slot, const std::string_view& sv)
    {
        wrenSetSlotBytes(vm, slot, sv.data(), sv.length());
    }
};

/// Specialisation for c++ strings.
template <>
struct SlotHelper<std::string>
{
    static bool check(WrenVM* vm, int slot)
    {
        return wrenGetSlotType(vm, slot) == WREN_TYPE_STRING;
    }

    static std::string get(WrenVM* vm, int slot)
    {
        int len; auto cstr = wrenGetSlotBytes(vm, slot, &len);
        return std::string(cstr, size_t(len));
    }

    static void set(WrenVM* vm, int slot, const std::string& str)
    {
        wrenSetSlotBytes(vm, slot, str.data(), str.length());
    }
};

/// Specialisation for c++ optionals.
template <class Type>
struct SlotHelper<std::optional<Type>>
{
    static bool check(WrenVM* vm, int slot)
    {
        return wrenGetSlotType(vm, slot) == WREN_TYPE_NULL || SlotHelper<Type>::check(vm, slot);
    }

    static std::optional<Type> get(WrenVM* vm, int slot)
    {
        if (wrenGetSlotType(vm, slot) == WREN_TYPE_NULL) return std::nullopt;
        else return SlotHelper<Type>::get(vm, slot);
    }

    static void set(WrenVM* vm, int slot, std::optional<Type> value)
    {
        if (value == std::nullopt) wrenSetSlotNull(vm, slot);
        else SlotHelper<Type>::set(vm, slot, *value);
    }
};

//============================================================================//

template <class... Args, size_t... Slots>
inline bool impl_check_slots(WrenVM* vm, std::index_sequence<Slots...>)
{
    return ( SlotHelper<Args>::check(vm, Slots) && ... );
}

template <class... Args, size_t... Slots>
inline std::tuple<Args...> impl_get_slots(WrenVM* vm, std::index_sequence<Slots...>)
{
    return std::tuple(SlotHelper<Args>::get(vm, Slots)...);
}

template <class... Args, size_t... Slots>
inline void impl_set_slots(WrenVM* vm, Args... args, std::index_sequence<Slots...>)
{
    ( SlotHelper<Args>::set(vm, Slots, args), ... );
}

//============================================================================//

/// Check if wren slots hold the given types.
template <class... Args>
inline bool check_slots(WrenVM* vm)
{
    return impl_check_slots<Args...>(vm, std::make_index_sequence<sizeof...(Args)>());
}

/// Get a tuple with the given types from wren slots.
template <class... Args>
inline std::tuple<Args...> get_slots(WrenVM* vm)
{
    return impl_get_slots<Args...>(vm, std::make_index_sequence<sizeof...(Args)>());
}

/// Set wren slots to the given arguments.
template <class... Args>
inline void set_slots(WrenVM* vm, Args... args)
{
    wrenEnsureSlots(vm, sizeof...(Args));
    impl_set_slots<Args...>(vm, args..., std::make_index_sequence<sizeof...(Args)>());
}

//============================================================================//

/// RAII wrapper for WrenVM, with some nice extra features.
class SQEE_API WrenPlusVM final
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

    //--------------------------------------------------------//

    /// Define a foreign method using a member function pointer.
    template <auto FnPtr>
    void add_foreign_method(const char* signature)
    {
        // todo: c++20 template lambdas so this can be one function
        impl_add_foreign_method(signature, FnPtr, &wren::MethodWrapper<decltype(FnPtr), FnPtr>::call);
    }

    //--------------------------------------------------------//

    /// Call a wren method with the given arguments and return the result.
    template <class Result, class... Args>
    Result call(WrenHandle* method, Args... args)
    {
        set_slots(*this, args...);
        mErrorString.clear();
        if (wrenCall(*this, method) != WREN_RESULT_SUCCESS)
            throw Exception(std::move(mErrorString));
        return SlotHelper<Result>::get(*this, 0);
    }

    /// Call a wren method with the given arguments.
    template <class... Args>
    void call_void(WrenHandle* method, Args... args)
    {
        set_slots(*this, args...);
        mErrorString.clear();
        if (wrenCall(*this, method) != WREN_RESULT_SUCCESS)
            throw Exception(std::move(mErrorString));
    }

    //--------------------------------------------------------//

    /// Call a wren method with the given arguments and return the result or any errors.
    template <class Result, class... Args>
    SafeResult<Result> safe_call[[nodiscard]](WrenHandle* method, Args... args)
    {
        set_slots(*this, args...);
        mErrorString.clear();
        if (wrenCall(*this, method) != WREN_RESULT_SUCCESS)
            return std::move(mErrorString);
        if (SlotHelper<Result>::check(*this, 0) == false)
            return std::string("result type mismatch");
        return std::optional(SlotHelper<Result>::get(*this, 0));
    }

    /// Call a wren method with the given arguments and return errors as a string.
    template <class... Args>
    std::optional<std::string> safe_call_void[[nodiscard]](WrenHandle* method, Args... args)
    {
        set_slots(*this, args...);
        mErrorString.clear();
        if (wrenCall(*this, method) != WREN_RESULT_SUCCESS)
            return std::move(mErrorString);
        return std::nullopt;
    }

    //--------------------------------------------------------//

    /// Wrapper for wrenInterpret that throws.
    void interpret(const char* module, const char* source);

    /// Wrapper for wrenInterpret that returns errors as a string.
    std::optional<std::string> safe_interpret[[nodiscard]](const char* module, const char* source);

    //--------------------------------------------------------//

    /// Abort the current wren fiber with a formatted error message.
    template <class... Args>
    void abort(std::string_view str, const Args&... args)
    {
        const std::string message = fmt::format(str, args...);
        wrenEnsureSlots(*this, 1);
        wrenSetSlotBytes(*this, 0, message.data(), message.length());
        wrenAbortFiber(*this, 0);
    }

private: //===================================================//

    static void impl_write_fn(WrenVM*, const char*);

    static void impl_error_fn(WrenVM*, WrenErrorType, const char*, int, const char*);

    static WrenForeignClassMethods impl_bind_foreign_class_fn(WrenVM*, const char*, const char*);

    static WrenForeignMethodFn impl_bind_foreign_method_fn(WrenVM*, const char*, const char*, bool, const char*);

    static WrenLoadModuleResult impl_load_module_fn(WrenVM* vm, const char* name);

    //--------------------------------------------------------//

    template <class Object, class Result, class... Args>
    void impl_add_foreign_method(const char* signature, Result(Object::*)(Args...), WrenForeignMethodFn func)
    {
        static_assert(wren::has_traits_v<Object>, "missing wren::Traits specialisation");
        impl_register_method(Traits<Object>::module, Traits<Object>::className, signature, func);
    }

    void impl_register_method(const char* module, const char* className, const char* signature, WrenForeignMethodFn func);

    //--------------------------------------------------------//

    WrenVM* mWrenVM = nullptr;

    std::unordered_map<std::string, WrenForeignMethodFn> mForiegnMethods;

    std::string mErrorString;
};

//============================================================================//

template <class Result, class... Args, class Func>
inline void impl_apply_helper(WrenVM* vm, Func func)
{
    if (check_slots<Args...>(vm) == false)
        WrenPlusVM::access(vm).abort("argument type mismatch"); // todo: more detail

    else try
    {
        if constexpr (std::is_void_v<Result>) std::apply(func, get_slots<Args...>(vm));
        else SlotHelper<Result>::set(vm, 0, std::apply(func, get_slots<Args...>(vm)));
    }
    catch (const std::exception& ex)
    {
        WrenPlusVM::access(vm).abort(fmt::format("caught exception: {}", ex.what()));
    }
}

//============================================================================//

/// Wrapper for normal member functions.
template <class Result, class Object, class... Args, Result(Object::*FnPtr)(Args...)>
struct MethodWrapper<Result(Object::*)(Args...), FnPtr>
{
    static void call(WrenVM* vm)
    {
        impl_apply_helper<Result, Object*, Args...>(vm, FnPtr);
    }
};

// todo: wrappers for other kinds of functions

//============================================================================//

} // namespace wren

//===== SQEE SPECIFIC EXTRAS =================================================//

#include <sqee/misc/StackString.hpp>

namespace wren {

/// Specialisation for sqee stack strings.
template <size_t Capacity>
struct SlotHelper<sq::StackString<Capacity>>
{
    // todo: this should check that the string is short enough
    static bool check(WrenVM* vm, int slot)
    {
        return wrenGetSlotType(vm, slot) == WREN_TYPE_STRING;
    }

    static sq::StackString<Capacity> get(WrenVM* vm, int slot)
    {
        return SlotHelper<std::string_view>::get(vm, slot);
    }

    static void set(WrenVM* vm, int slot, const sq::StackString<Capacity>& str)
    {
        wrenSetSlotBytes(vm, slot, str.data(), str.length());
    }
};

} // namespace wren
