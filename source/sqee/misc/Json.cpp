#include <sqee/misc/Json.hpp>

#include <sqee/debug/Assert.hpp>

#include <sqee/redist/yyjson.hpp>

#include <cassert>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace sq;

//==============================================================================

inline size_t impl_prettify_string(char* chars, size_t length)
{
    // rewrites yyjson string to compress flat arrays to a single line each
    // this never adds extra chars, so we can just write back to the same string

    if (*chars != '{' && *chars != '[') return length; // root not a container

    const char* pIn = chars;
    char* pOut = chars;

    const char* const pDocEnd = chars + length;

    ++pIn; ++pOut; // don't compact root array

    while (true)
    {
        bool arrayDone = false;
        bool quoted = false;

        while (true)
        {
            assert(pIn < pDocEnd);
            assert(pOut < pDocEnd);
            const char c = *(pOut++) = *(pIn++);
            if (c == '"' && *(pIn-2) != '\\') { quoted = !quoted; continue; }
            if (quoted == false && c == '[') break;
            if (pIn == pDocEnd) break;
        }
        if (pIn == pDocEnd) break;

        if (*pIn == ']')
        {
            assert(pIn < pDocEnd);
            assert(pOut < pDocEnd);
            *(pOut++) = ']'; ++pIn;
            if (pIn == pDocEnd) break;
            continue;
        }

        const char* pLook = ++pIn;
        while (true)
        {
            const char c = *(pLook++);
            if (c == '"' && *(pLook-2) != '\\') { quoted = !quoted; continue; }
            if (quoted == false && c == ']') break;
            if (quoted == false && (c == '[' || c == '{')) { arrayDone = true; break; }
            assert(pLook < pDocEnd);
        }
        if (arrayDone == true)
        {
            do *(pOut++) = *pIn; while (++pIn != pLook);
            if (pIn == pDocEnd) break;
            continue;
        }

        while (true)
        {
            *(pOut++) = ' '; // space before/between/after items
            while (*(++pIn) == ' '); // skip over indent
            while (true)
            {
                assert(pIn < pDocEnd);
                assert(pOut < pDocEnd);
                const char c = *(pIn++);
                if (c == '\n') break;
                *(pOut++) = c;
                if (c == '"' && *(pIn-2) != '\\') { quoted = !quoted; continue; }
                if (quoted == false && c == ']') { arrayDone = true; break; }
            }
            if (arrayDone == true) break;
        }
        if (pIn == pDocEnd) break;
    }

    return size_t(pOut - chars);
}

//==============================================================================

String JsonAny::dump(bool pretty) const noexcept
{
    const auto flags = pretty ? YYJSON_WRITE_PRETTY_TWO_SPACES : YYJSON_WRITE_NOFLAG;

    size_t length; yyjson_write_err error;
    const auto chars = std::unique_ptr<char, decltype(&std::free)>(
        yyjson_val_write_opts(mVal, flags, nullptr, &length, &error), &std::free
    );
    SQASSERT(chars != nullptr, error.msg);

    return String(chars.get(), pretty ? impl_prettify_string(chars.get(), length) : length);
}

String JsonAny::pointer() const noexcept
{
    // todo: support various escapes needed for actual json pointers

    struct IndexOrKey
    {
        union { size_t index; yyjson_val* key; }; bool isKey;
        IndexOrKey(size_t index) : index(index), isKey(false) {}
        IndexOrKey(yyjson_val* key) : key(key), isKey(true) {}
    };

    std::vector<IndexOrKey> pathStack;

    const auto inner = [this, &pathStack](auto inner, yyjson_val* val) -> bool
    {
        if (val == mVal)
        {
            return true;
        }
        else if (unsafe_yyjson_is_arr(val))
        {
            for (yyjson_arr_iter iter = yyjson_arr_iter_with(val); yyjson_val* child = yyjson_arr_iter_next(&iter);)
            {
                pathStack.emplace_back(iter.idx-1);
                if (inner(inner, child)) return true;
                pathStack.pop_back();
            }
        }
        else if (unsafe_yyjson_is_obj(val))
        {
            for (yyjson_obj_iter iter = yyjson_obj_iter_with(val); yyjson_val* key = yyjson_obj_iter_next(&iter);)
            {
                pathStack.emplace_back(key);
                if (inner(inner, key+1)) return true;
                pathStack.pop_back();
            }
        }
        return false;
    };

    const bool found = inner(inner, mDoc->mRoot);
    SQASSERT(found == true, "could not find val in doc");

    fmt::memory_buffer buffer;

    for (size_t i = 0u; i < pathStack.size(); ++i)
    {
        buffer.push_back('/');

        const IndexOrKey& entry = pathStack[i];

        if (entry.isKey == false)
            fmt::format_to(fmt::appender(buffer), "{}", entry.index);
        else
            buffer.append(unsafe_yyjson_get_str(entry.key), unsafe_yyjson_get_str(entry.key) + unsafe_yyjson_get_len(entry.key));
    }

    if (buffer.size() == 0u) buffer.push_back('#');

    return String(buffer.data(), buffer.size());
}

void JsonAny::impl_throw_as(const char* typeName) const
{
    throw std::runtime_error(fmt::format("{} -> {} -> as {}", mDoc->mPath, pointer(), typeName));
}

void JsonAny::impl_throw_with_context(fmt::string_view fstr, fmt::format_args args) const
{
    fmt::memory_buffer buffer;

    fmt::format_to(fmt::appender(buffer), "{} -> {} -> ", mDoc->mPath, pointer());

    if (reinterpret_cast<const unsigned long long&>(args) == 0) buffer.append(fstr);
    else fmt::vformat_to(fmt::appender(buffer), fstr, args);

    throw std::runtime_error(String(buffer.data(), buffer.size()));
}

//==============================================================================

std::optional<JsonAny> JsonArray::get_safe(size_t index) const noexcept
{
    if (yyjson_val* val = yyjson_arr_get(mVal, index)) return JsonAny(val, mDoc);
    return std::nullopt;
}

JsonArray::IndexValue JsonArray::Iterator::operator*() const noexcept
{
    assert(iterCur != nullptr);
    return { iterIdx, JsonAny(iterCur, array->mDoc) };
}

JsonArray::Iterator& JsonArray::Iterator::operator++() noexcept
{
    const size_t iterMax = array->size();
    assert(iterIdx < iterMax);
    iterCur = (++iterIdx < iterMax) ? unsafe_yyjson_get_next(iterCur) : nullptr;
    return *this;
}

void JsonArray::impl_throw_get(size_t index) const
{
    throw std::runtime_error(fmt::format("{} -> {} -> get {}", mDoc->mPath, pointer(), index));
}

//==============================================================================

std::optional<JsonAny> JsonObject::get_safe(StringView key) const noexcept
{
    if (yyjson_val* val = yyjson_obj_getn(mVal, key.data(), key.length())) return JsonAny(val, mDoc);
    return std::nullopt;
}

JsonObject::KeyValue JsonObject::Iterator::operator*() const noexcept
{
    assert(iterCur != nullptr);
    return { StringView(unsafe_yyjson_get_str(iterCur), unsafe_yyjson_get_len(iterCur)), JsonAny(iterCur+1, object->mDoc) };
}

JsonObject::Iterator& JsonObject::Iterator::operator++() noexcept
{
    const size_t iterMax = object->size();
    assert(iterIdx < iterMax);
    iterCur = (++iterIdx < iterMax) ? unsafe_yyjson_get_next(iterCur+1) : nullptr;
    return *this;
}

void JsonObject::impl_throw_get(StringView key) const
{
    throw std::runtime_error(fmt::format("{} -> {} -> get \"{}\"", mDoc->mPath, pointer(), key));
}

//==============================================================================

JsonDocument::~JsonDocument() noexcept
{
    assert(mDoc != nullptr);
    yyjson_doc_free(mDoc);
}

tl::expected<JsonDocument, StringView> JsonDocument::parse_file_safe(String path) noexcept
{
    yyjson_read_err error;
    if (auto doc = yyjson_read_file(path.c_str(), YYJSON_READ_INSITU, nullptr, &error))
        return JsonDocument(doc, doc->root, std::move(path));

    return tl::unexpected(StringView(error.msg));
}

JsonDocument JsonDocument::parse_file(String path)
{
    yyjson_read_err error;
    if (auto doc = yyjson_read_file(path.c_str(), YYJSON_READ_INSITU, nullptr, &error))
        return JsonDocument(doc, doc->root, std::move(path));

    throw std::runtime_error(fmt::format("{} -> JsonDocument parse_file: {}", path, error.msg));
}

tl::expected<JsonDocument, StringView> JsonDocument::parse_string_safe(StringView source, String fakePath) noexcept
{
    yyjson_read_err error;
    if (auto doc = yyjson_read_opts(const_cast<char*>(source.data()), source.length(), 0, nullptr, &error))
        return JsonDocument(doc, doc->root, std::move(fakePath));

    return tl::unexpected(StringView(error.msg));
}

JsonDocument JsonDocument::parse_string(StringView source, String fakePath)
{
    yyjson_read_err error;
    if (auto doc = yyjson_read_opts(const_cast<char*>(source.data()), source.length(), 0, nullptr, &error))
        return JsonDocument(doc, doc->root, std::move(fakePath));

    throw std::runtime_error(fmt::format("{} -> JsonDocument parse_string: {}", fakePath, error.msg));
}

//==============================================================================

String JsonMutAny::dump(bool pretty) const noexcept
{
    const auto flags = pretty ? YYJSON_WRITE_PRETTY_TWO_SPACES : YYJSON_WRITE_NOFLAG;

    size_t length; yyjson_write_err error;
    const auto chars = std::unique_ptr<char, decltype(&std::free)>(
        yyjson_mut_val_write_opts(mVal, flags, nullptr, &length, &error), &std::free
    );
    SQASSERT(chars != nullptr, error.msg);

    return String(chars.get(), pretty ? impl_prettify_string(chars.get(), length) : length);
}

//==============================================================================

JsonMutArray::JsonMutArray(JsonMutDocument& doc) noexcept : JsonMutAny(yyjson_mut_arr(doc.mDoc), &doc)
{
    assert(mVal != nullptr);
}

void JsonMutArray::impl_append_any(JsonMutAny any) noexcept
{
    [[maybe_unused]] bool success = yyjson_mut_arr_append(mVal, any.mVal);
    assert(success == true);
}

//==============================================================================

JsonMutObject::JsonMutObject(JsonMutDocument& doc) noexcept : JsonMutAny(yyjson_mut_obj(doc.mDoc), &doc)
{
    assert(mVal != nullptr);
}

void JsonMutObject::impl_append_any(StringView key, JsonMutAny any) noexcept
{
    yyjson_mut_val* keyVal = yyjson_mut_strncpy(mDoc->mDoc, key.data(), key.length());
    [[maybe_unused]] bool success = yyjson_mut_obj_add(mVal, keyVal, any.mVal);
    assert(success == true);
}

//==============================================================================

JsonMutDocument::JsonMutDocument() noexcept : mDoc(yyjson_mut_doc_new(nullptr)) {}

JsonMutDocument::~JsonMutDocument() noexcept { yyjson_mut_doc_free(mDoc); }

void JsonMutDocument::impl_assign_any(JsonMutAny any) noexcept
{
    assert(mRoot == nullptr && any.mDoc == this);
    yyjson_mut_doc_set_root(mDoc, any.mVal);
}

//==============================================================================

JsonMutAny JsonTraits<std::nullptr_t>::from(JsonMutDocument& doc, std::nullptr_t) noexcept
{
    yyjson_mut_val* val = yyjson_mut_null(doc.mDoc); assert(val);
    return JsonMutAny(val, &doc);
}

JsonMutAny JsonTraits<bool>::from(JsonMutDocument& doc, bool arg) noexcept
{
    yyjson_mut_val* val = yyjson_mut_bool(doc.mDoc, arg); assert(val);
    return JsonMutAny(val, &doc);
}

JsonMutAny JsonTraits<int64_t>::from(JsonMutDocument& doc, int64_t arg) noexcept
{
    yyjson_mut_val* val = yyjson_mut_sint(doc.mDoc, arg); assert(val);
    return JsonMutAny(val, &doc);
}

JsonMutAny JsonTraits<uint64_t>::from(JsonMutDocument& doc, uint64_t arg) noexcept
{
    yyjson_mut_val* val = yyjson_mut_uint(doc.mDoc, arg); assert(val);
    return JsonMutAny(val, &doc);
}

JsonMutAny JsonTraits<double>::from(JsonMutDocument& doc, double arg) noexcept
{
    yyjson_mut_val* val = yyjson_mut_real(doc.mDoc, arg); assert(val);
    return JsonMutAny(val, &doc);
}

JsonMutAny JsonTraits<StringView>::from(JsonMutDocument& doc, StringView arg) noexcept
{
    yyjson_mut_val* val = yyjson_mut_strn(doc.mDoc, arg.data(), arg.length()); assert(val);
    return JsonMutAny(val, &doc);
}
