// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/export.hpp>

#include <sqee/core/TypeAliases.hpp>
#include <sqee/core/TypeNames.hpp>
#include <sqee/core/TypeTraits.hpp>

#include <ranges>

//==============================================================================

extern "C" {
typedef struct yyjson_doc yyjson_doc;
typedef struct yyjson_val yyjson_val;
typedef struct yyjson_mut_doc yyjson_mut_doc;
typedef struct yyjson_mut_val yyjson_mut_val;
} // extern "C"

namespace sq { //###############################################################

template <class Type>
struct JsonTraits final {};

template <class Type>
concept JsonCastableAs = HasTypeName<Type> && requires (JsonAny json) { JsonTraits<Type>::as(json); };

template <class Type>
concept JsonCastableFrom = HasTypeName<Type> && requires (JsonMutDocument& doc, const Type& arg) { JsonTraits<Type>::from(doc, arg); };

//==============================================================================

namespace detail {

constexpr uint8_t JSON_TYPE_NULL = 2;
constexpr uint8_t JSON_TYPE_BOOL = 3;
constexpr uint8_t JSON_TYPE_NUM  = 4;
constexpr uint8_t JSON_TYPE_STR  = 5;
constexpr uint8_t JSON_TYPE_ARR  = 6;
constexpr uint8_t JSON_TYPE_OBJ  = 7;

constexpr uint8_t JSON_SUBTYPE_FALSE = 0 << 3;
constexpr uint8_t JSON_SUBTYPE_TRUE  = 1 << 3;
constexpr uint8_t JSON_SUBTYPE_UINT  = 0 << 3;
constexpr uint8_t JSON_SUBTYPE_SINT  = 1 << 3;
constexpr uint8_t JSON_SUBTYPE_REAL  = 2 << 3;

inline bool json_check_tag(void* val, uint8_t type)
{
    return (*reinterpret_cast<uint8_t*>(val) & uint8_t(0x7)) == type;
}

inline bool json_check_tag(void* val, uint8_t type, uint8_t subtype)
{
    return *reinterpret_cast<uint8_t*>(val) == (type | subtype);
}

inline size_t json_get_length(void* val)
{
    return size_t(*reinterpret_cast<uint64_t*>(val) >> 8);
}

inline yyjson_val* json_get_first(yyjson_val* val)
{
    return json_get_length(val) ? reinterpret_cast<yyjson_val*>(reinterpret_cast<uint64_t*>(val)+2) : nullptr;
}

template <class Integer>
inline std::optional<Integer> json_as_integer(void* val) noexcept
{
    if (json_check_tag(val, JSON_TYPE_NUM, JSON_SUBTYPE_UINT))
    {
        if (uint64_t v = reinterpret_cast<uint64_t*>(val)[1]; v <= uint64_t(std::numeric_limits<Integer>::max()))
            return Integer(v);
    }
    else if constexpr (std::is_signed_v<Integer>)
    {
        if (json_check_tag(val, JSON_TYPE_NUM, JSON_SUBTYPE_SINT))
            if (int64_t v = reinterpret_cast<int64_t*>(val)[1]; v >= int64_t(std::numeric_limits<Integer>::min()))
                return Integer(v);
    }
    return std::nullopt;
}

} // namespace detail

//==============================================================================

class SQEE_API JsonAny
{
public: //======================================================

    /// Access the document that contains this element.
    const JsonDocument& document() const { return *mDoc; }

    /// Conveniently check if the element is null.
    bool is_null() const noexcept
    {
        return detail::json_check_tag(mVal, detail::JSON_TYPE_NULL);
    }

    /// Cast the element to the given type, return nullopt if the conversion fails.
    template <JsonCastableAs Type>
    std::optional<Type> as_safe() const noexcept
    {
        return JsonTraits<Type>::as(*this);
    }

    /// Cast the element to the given type, throw if the conversion fails.
    template <JsonCastableAs Type>
    Type as() const
    {
       if (auto opt = JsonTraits<Type>::as(*this)) return *opt;
       impl_throw_as(type_name_v<Type>.c_str());
    }

    struct AutoCaster final
    {
        const JsonAny& ref;
        template <JsonCastableAs Type>
        operator Type() const { return ref.as<Type>(); }
    };

    /// Get an object that can be implicitly converted to any supported type.
    AutoCaster as_auto() const
    {
        return AutoCaster{*this};
    }

    /// Build a minified or pretty document string from this element.
    String dump(bool pretty) const noexcept;

    /// Build a json pointer string for this element.
    String pointer() const noexcept;

    /// Throw a std::runtime_error with extra information about this element.
    template <class... Args>
    [[noreturn]] void throw_with_context(FmtStrIfArgs<Args...> fstr, Args&&... args) const
    {
        impl_throw_with_context(fstr, fmt::make_format_args(args...));
    }

protected: //===================================================

    yyjson_val* mVal; const JsonDocument* mDoc;

    JsonAny(yyjson_val* val, const JsonDocument* doc) noexcept : mVal(val), mDoc(doc) {}

    [[noreturn]] void impl_throw_as(const char* typeName) const;

    [[noreturn]] void impl_throw_with_context(fmt::string_view fstr, fmt::format_args args) const;

    friend JsonArray; friend JsonObject; friend JsonDocument;
    template <class Type> friend struct JsonTraits;
};

//------------------------------------------------------------------------------

class SQEE_API JsonArray final : public JsonAny
{
public: //======================================================

    using IndexValue = std::pair<size_t, JsonAny>;

    struct PointerFacade final { IndexValue item; IndexValue* operator->() noexcept { return &item; } };

    struct Iterator final
    {
        size_t iterIdx; yyjson_val* iterCur; const JsonArray* array;

        using element_type = IndexValue;
        using iterator_concept = std::forward_iterator_tag;

        Iterator& operator++() noexcept;
        Iterator operator++(int) noexcept { Iterator tmp = *this; ++(*this); return tmp; }

        ptrdiff_t operator-(const Iterator& other) const noexcept { return ptrdiff_t(iterIdx) - ptrdiff_t(other.iterIdx); }

        IndexValue operator*() const noexcept;
        PointerFacade operator->() const noexcept { return PointerFacade{**this}; }

        auto operator<=>(const Iterator& other) const noexcept { return iterIdx <=> other.iterIdx; };
        bool operator==(const Iterator& other) const noexcept { return iterIdx == other.iterIdx; }
    };

    size_t size() const noexcept { return detail::json_get_length(mVal); }

    Iterator begin() const noexcept { return { 0u, detail::json_get_first(mVal), this }; }

    Iterator end() const noexcept { return { detail::json_get_length(mVal), nullptr, this }; }

    /// Get a value by index if the index is in range.
    std::optional<JsonAny> get_safe(size_t index) const noexcept;

    /// Get a value by index or throw if the index is not in range.
    JsonAny operator[](size_t index) const { if (auto opt = get_safe(index)) return *opt; impl_throw_get(index); }

protected: //===================================================

    explicit JsonArray(JsonAny base) noexcept : JsonAny(base.mVal, base.mDoc) {}

    void impl_iter_next(Iterator& iter) const noexcept;

    [[noreturn]] void impl_throw_get(size_t index) const;

    template <class Type> friend struct JsonTraits;
};

//------------------------------------------------------------------------------

class SQEE_API JsonObject final : public JsonAny
{
public: //======================================================

    using KeyValue = std::pair<StringView, JsonAny>;

    struct PointerFacade final { KeyValue item; KeyValue* operator->() noexcept { return &item; } };

    struct Iterator final
    {
        size_t iterIdx; yyjson_val* iterCur; const JsonObject* object;

        using element_type = KeyValue;
        using iterator_concept = std::forward_iterator_tag;

        Iterator& operator++() noexcept;
        Iterator operator++(int) noexcept { Iterator tmp = *this; ++(*this); return tmp; }

        ptrdiff_t operator-(const Iterator& other) const noexcept { return ptrdiff_t(iterIdx) - ptrdiff_t(other.iterIdx); }

        KeyValue operator*() const noexcept;
        PointerFacade operator->() const noexcept { return PointerFacade{**this}; }

        auto operator<=>(const Iterator& other) const noexcept { return iterIdx <=> other.iterIdx; };
        bool operator==(const Iterator& other) const noexcept { return iterIdx == other.iterIdx; }
    };

    size_t size() const noexcept { return detail::json_get_length(mVal); }

    Iterator begin() const noexcept { return { 0u, detail::json_get_first(mVal), this }; }

    Iterator end() const noexcept { return { detail::json_get_length(mVal), nullptr, this }; }

    /// Get a value by key if the key exists.
    std::optional<JsonAny> get_safe(StringView key) const noexcept;

    /// Get a value by key or throw if the key does not exist.
    JsonAny operator[](StringView key) const { if (auto opt = get_safe(key)) return *opt; impl_throw_get(key); }

protected: //===================================================

    explicit JsonObject(JsonAny base) noexcept : JsonAny(base.mVal, base.mDoc) {}

    void impl_iter_next(Iterator& iter) const noexcept;

    [[noreturn]] void impl_throw_get(StringView Key) const;

    template <class Type> friend struct JsonTraits;
};

//------------------------------------------------------------------------------

class SQEE_API JsonDocument final
{
public: //======================================================

    ~JsonDocument() noexcept;

    /// Load a new document from a file. Return an error string on failure.
    static tl::expected<JsonDocument, StringView> parse_file_safe(String path) noexcept;

    /// Load a new document from a file. Throw an exception on failure.
    static JsonDocument parse_file(String path);

    /// Load a new document from a string. Return an error string on failure.
    static tl::expected<JsonDocument, StringView> parse_string_safe(StringView source, String fakePath) noexcept;

    /// Load a new document from a string. Throw an exception on failure.
    static JsonDocument parse_string(StringView source, String fakePath);

    /// Access the document's root element.
    JsonAny root() const noexcept { return JsonAny(mRoot, this); }

private: //=====================================================

    yyjson_doc* const mDoc; yyjson_val* const mRoot; const String mPath;

    JsonDocument(yyjson_doc* doc, yyjson_val* root, String path) noexcept : mDoc(doc), mRoot(root), mPath(path) {}

    friend JsonAny; friend JsonArray; friend JsonObject;
    template <class Type> friend struct JsonTraits;
};

//==============================================================================

class SQEE_API JsonMutAny
{
public: //======================================================

    /// Construct a new element for the given document.
    template <JsonCastableFrom Type>
    JsonMutAny(JsonMutDocument& doc, const Type& arg) noexcept : mVal(JsonTraits<Type>::from(doc, arg).mVal), mDoc(&doc) {}

    /// Access the document that was used to create this element.
    JsonMutDocument& document() noexcept { return *mDoc; }

    /// Build a minified or pretty document string from this element.
    String dump(bool pretty) const noexcept;

protected: //===================================================

    yyjson_mut_val* mVal; JsonMutDocument* mDoc;

    JsonMutAny(yyjson_mut_val* val, JsonMutDocument* doc) noexcept : mVal(val), mDoc(doc) {}

    friend JsonMutArray; friend JsonMutObject; friend JsonMutDocument;
    template <class Type> friend struct JsonTraits;
};

//------------------------------------------------------------------------------

class SQEE_API JsonMutArray final : public JsonMutAny
{
public: //======================================================

    /// Construct a new json array for the given document.
    JsonMutArray(JsonMutDocument& doc) noexcept;

    /// Add a new element to the end of the array. Returns an Any/Array/Object.
    template <JsonCastableFrom Type>
    auto append(const Type& arg) noexcept
    {
        auto result = JsonTraits<Type>::from(*mDoc, arg);
        impl_append_any(result);
        return result;
    }

private: //=====================================================

    void impl_append_any(JsonMutAny any) noexcept;

    template <class Type> friend struct JsonTraits;
};

//------------------------------------------------------------------------------

class SQEE_API JsonMutObject final : public JsonMutAny
{
public: //======================================================

    /// Construct a new json object for the given document.
    JsonMutObject(JsonMutDocument& doc) noexcept;

    /// Add a new element with the given key. Returns an Any/Array/Object.
    template <JsonCastableFrom Type>
    auto append(StringView key, const Type& arg) noexcept
    {
        auto result = JsonTraits<Type>::from(*mDoc, arg);
        impl_append_any(key, result);
        return result;
    }

private: //=====================================================

    void impl_append_any(StringView key, JsonMutAny any) noexcept;

    template <class Type> friend struct JsonTraits;
};

//------------------------------------------------------------------------------

class SQEE_API JsonMutDocument final
{
public: //======================================================

    JsonMutDocument() noexcept;

    ~JsonMutDocument() noexcept;

    /// Set the document's root element. Returns an Any/Array/Object.
    template <JsonCastableFrom Type>
    auto assign(const Type& arg) noexcept
    {
        auto result = JsonTraits<Type>::from(*this, arg);
        impl_assign_any(result);
        return result;
    }

private: //=====================================================

    yyjson_mut_doc* const mDoc; yyjson_mut_val* mRoot = nullptr;

    void impl_assign_any(JsonMutAny any) noexcept;

    friend JsonMutAny; friend JsonMutArray; friend JsonMutObject;
    template <class Type> friend struct JsonTraits;
};

//==============================================================================

namespace views {

/// Casts each value of a JsonArray or JsonObject to the given type.
template <JsonCastableAs Type>
inline const auto json_as = std::views::transform([](auto item) { return std::pair(item.first, item.second.template as<Type>()); });

} // namespace views

//==============================================================================

template <> struct JsonTraits<std::nullptr_t>
{
    inline static std::optional<std::nullptr_t> as(JsonAny json) noexcept
    {
        if (detail::json_check_tag(json.mVal, detail::JSON_TYPE_NULL)) return nullptr;
        return std::nullopt;
    }
    SQEE_API static JsonMutAny from(JsonMutDocument& doc, std::nullptr_t) noexcept;
};

template <> struct JsonTraits<bool>
{
    inline static std::optional<bool> as(JsonAny json) noexcept
    {
        if (detail::json_check_tag(json.mVal, detail::JSON_TYPE_BOOL, detail::JSON_SUBTYPE_FALSE)) return false;
        if (detail::json_check_tag(json.mVal, detail::JSON_TYPE_BOOL, detail::JSON_SUBTYPE_TRUE)) return true;
        return std::nullopt;
    }
    SQEE_API static JsonMutAny from(JsonMutDocument& doc, bool arg) noexcept;
};

template <> struct JsonTraits<int64_t>
{
    inline static std::optional<int64_t> as(JsonAny json) noexcept { return detail::json_as_integer<int64_t>(json.mVal); }
    SQEE_API static JsonMutAny from(JsonMutDocument& doc, int64_t arg) noexcept;
};

template<> struct JsonTraits<int32_t>
{
    inline static std::optional<int32_t> as(JsonAny json) noexcept { return detail::json_as_integer<int32_t>(json.mVal); }
    inline static JsonMutAny from(JsonMutDocument& doc, int32_t arg) noexcept { return JsonTraits<int64_t>::from(doc, int64_t(arg)); }
};

template <> struct JsonTraits<int16_t>
{
    inline static std::optional<int16_t> as(JsonAny json) noexcept { return detail::json_as_integer<int16_t>(json.mVal); }
    inline static JsonMutAny from(JsonMutDocument& doc, int16_t arg) noexcept { return JsonTraits<int64_t>::from(doc, int64_t(arg)); }
};

template <> struct JsonTraits<int8_t>
{
    inline static std::optional<int8_t> as(JsonAny json) noexcept { return detail::json_as_integer<int8_t>(json.mVal); }
    inline static JsonMutAny from(JsonMutDocument& doc, int8_t arg) noexcept { return JsonTraits<int64_t>::from(doc, int64_t(arg)); }
};

template <> struct JsonTraits<uint64_t>
{
    inline static std::optional<uint64_t> as(JsonAny json) noexcept { return detail::json_as_integer<uint64_t>(json.mVal); }
    SQEE_API static JsonMutAny from(JsonMutDocument& doc, uint64_t arg) noexcept;
};

template <> struct JsonTraits<uint32_t>
{
    inline static std::optional<uint32_t> as(JsonAny json) noexcept { return detail::json_as_integer<uint32_t>(json.mVal); }
    inline static JsonMutAny from(JsonMutDocument& doc, uint32_t arg) noexcept { return JsonTraits<uint64_t>::from(doc, uint64_t(arg)); }
};

template <> struct JsonTraits<uint16_t>
{
    inline static std::optional<uint16_t> as(JsonAny json) noexcept { return detail::json_as_integer<uint16_t>(json.mVal); }
    inline static JsonMutAny from(JsonMutDocument& doc, uint16_t arg) noexcept { return JsonTraits<uint64_t>::from(doc, uint64_t(arg)); }
};

template <> struct JsonTraits<uint8_t>
{
    inline static std::optional<uint8_t> as(JsonAny json) noexcept { return detail::json_as_integer<uint8_t>(json.mVal); }
    inline static JsonMutAny from(JsonMutDocument& doc, uint8_t arg) noexcept { return JsonTraits<uint64_t>::from(doc, uint64_t(arg)); }
};

template <> struct JsonTraits<double>
{
    inline static std::optional<double> as(JsonAny json) noexcept
    {
        if (detail::json_check_tag(json.mVal, detail::JSON_TYPE_NUM, detail::JSON_SUBTYPE_REAL))
            return reinterpret_cast<double*>(json.mVal)[1];
        return std::nullopt;
    }
    SQEE_API static JsonMutAny from(JsonMutDocument& doc, double arg) noexcept;
};

template <> struct JsonTraits<float>
{
    inline static std::optional<double> as(JsonAny json) noexcept
    {
        // automatically round or clamp double to float
        if (detail::json_check_tag(json.mVal, detail::JSON_TYPE_NUM, detail::JSON_SUBTYPE_REAL))
            return float(reinterpret_cast<double*>(json.mVal)[1]);
        return std::nullopt;
    }
    inline static JsonMutAny from(JsonMutDocument& doc, float arg) noexcept
    {
        return JsonTraits<double>::from(doc, double(arg));
    }
};

template <> struct JsonTraits<StringView>
{
    inline static std::optional<StringView> as(JsonAny json) noexcept
    {
        if (detail::json_check_tag(json.mVal, detail::JSON_TYPE_STR))
            return StringView(reinterpret_cast<const char**>(json.mVal)[1], detail::json_get_length(json.mVal));
        return std::nullopt;
    }
    SQEE_API static JsonMutAny from(JsonMutDocument& doc, StringView arg) noexcept;
};

template <> struct JsonTraits<String>
{
    inline static std::optional<String> as(JsonAny json) noexcept
    {
        if (detail::json_check_tag(json.mVal, detail::JSON_TYPE_STR))
            return String(reinterpret_cast<const char**>(json.mVal)[1], detail::json_get_length(json.mVal));
        return std::nullopt;
    }
    inline static JsonMutAny from(JsonMutDocument& doc, const String& arg) noexcept
    {
        return JsonTraits<StringView>::from(doc, StringView(arg));
    }
};

//------------------------------------------------------------------------------

template <> struct JsonTraits<JsonAny>
{
    inline static std::optional<JsonAny> as(JsonAny json) noexcept { return json; }
};

template <> struct JsonTraits<JsonArray>
{
    inline static std::optional<JsonArray> as(JsonAny json) noexcept
    {
        if (detail::json_check_tag(json.mVal, detail::JSON_TYPE_ARR)) return JsonArray(json);
        return std::nullopt;
    }
};

template <> struct JsonTraits<JsonObject>
{
    inline static std::optional<JsonObject> as(JsonAny json) noexcept
    {
        if (detail::json_check_tag(json.mVal, detail::JSON_TYPE_OBJ)) return JsonObject(json);
        return std::nullopt;
    }
};

template <> struct JsonTraits<JsonMutAny>
{
    inline static JsonMutAny from(JsonMutDocument&, JsonMutAny arg) noexcept { return arg; }
};

template <> struct JsonTraits<JsonMutArray>
{
    inline static JsonMutArray from(JsonMutDocument&, JsonMutArray arg) noexcept { return arg; }
};

template <> struct JsonTraits<JsonMutObject>
{
    inline static JsonMutObject from(JsonMutDocument&, JsonMutObject arg) noexcept { return arg; }
};

//==============================================================================

template <size_t Capacity>
struct JsonTraits<StackString<Capacity>>
{
    inline static std::optional<StackString<Capacity>> as(JsonAny json) noexcept
    {
        if (auto sv = JsonTraits<StringView>::as(json); sv && sv->length() <= Capacity) return StackString<Capacity>(*sv);
        return std::nullopt;
    }

    inline static JsonMutAny from(JsonMutDocument& doc, const StackString<Capacity>& arg) noexcept
    {
        return JsonTraits<StringView>::from(doc, StringView(arg));
    }
};

//------------------------------------------------------------------------------

template <class Type, size_t Capacity>
struct JsonTraits<StackVector<Type, Capacity>>
{
    inline static std::optional<StackVector<Type, Capacity>> as(JsonAny json) noexcept
    {
        if (auto array = JsonTraits<JsonArray>::as(json); array && array->size() <= Capacity)
        {
            StackVector<Type, Capacity> result;
            for (auto [_, value] : *array)
                if (auto v = JsonTraits<Type>::as(value)) result.emplace_back(*v);
                else return std::nullopt;
            return result;
        }
        return std::nullopt;
    }

    inline static JsonMutArray from(JsonMutDocument& doc, const StackVector<Type, Capacity>& arg) noexcept
    {
        auto array = JsonMutArray(doc);
        for (const auto& v : arg) array.append(arg);
        return array;
    }
};

//------------------------------------------------------------------------------

template <int Size, class Type>
struct JsonTraits<maths::Vector<Size, Type>>
{
    inline static std::optional<maths::Vector<Size, Type>> as(JsonAny json) noexcept
    {
        if (auto array = JsonTraits<JsonArray>::as(json); array && array->size() == Size)
        {
            maths::Vector<Size, Type> result;
            for (int i = 0; i < Size; ++i)
                if (auto v = JsonTraits<Type>::as(*(array->get_safe(i)))) result[i] = *v;
                else return std::nullopt;
            return result;
        }
        return std::nullopt;
    }

    inline static JsonMutArray from(JsonMutDocument& doc, const maths::Vector<Size, Type>& arg) noexcept
    {
        auto array = JsonMutArray(doc);
        for (int i = 0; i < Size; ++i) array.append(arg[i]);
        return array;
    }
};

//------------------------------------------------------------------------------

template <class Type>
struct JsonTraits<maths::Quaternion<Type>>
{
    inline static std::optional<maths::Quaternion<Type>> as(JsonAny json) noexcept
    {
        if (auto array = JsonTraits<JsonArray>::as(json); array && array->size() == 4)
        {
            maths::Quaternion<Type> result;
            for (int i = 0; i < 4; ++i)
                if (auto v = JsonTraits<Type>::as(*(array->get_safe(i)))) result[i] = *v;
                else return std::nullopt;
            return result;
        }
        return std::nullopt;
    }

    inline static JsonMutArray from(JsonMutDocument& doc, const maths::Quaternion<Type>& arg) noexcept
    {
        auto array = JsonMutArray(doc);
        for (int i = 0; i < 4; ++i) array.append(arg[i]);
        return array;
    }
};

//------------------------------------------------------------------------------

template <class Type>
struct JsonTraits<maths::RandomRange<Type>>
{
    inline static std::optional<maths::RandomRange<Type>> as(JsonAny json) noexcept
    {
        if (auto array = JsonTraits<JsonArray>::as(json); array && array->size() == 2)
        {
            maths::RandomRange<Type> result;
            if (auto v = JsonTraits<Type>::as(*(array->get_safe(0)))) result.min = *v;
            else return std::nullopt;
            if (auto v = JsonTraits<Type>::as(*(array->get_safe(1)))) result.max = *v;
            else return std::nullopt;
            return result;
        }
        return std::nullopt;
    }

    inline static JsonMutArray from(JsonMutDocument& doc, const maths::RandomRange<Type>& arg) noexcept
    {
        auto array = JsonMutArray(doc);
        array.append(arg.min); array.append(arg.max);
        return array;
    }
};

//------------------------------------------------------------------------------

template <HasEnumHelper Enum>
struct JsonTraits<Enum>
{
    inline static std::optional<Enum> as(JsonAny json) noexcept
    {
        if (auto sv = JsonTraits<StringView>::as(json)) return enum_from_string_safe<Enum>(*sv);
        return std::nullopt;
    }

    inline static JsonMutAny from(JsonMutDocument& doc, Enum arg) noexcept
    {
        return JsonTraits<StringView>::from(doc, *enum_to_string_safe(arg));
    }
};

} // namespace sq ##############################################################
