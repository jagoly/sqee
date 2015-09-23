#pragma once
#include <sqee/forward.hpp>

namespace sq {

/// The SQEE GLSL PreProcessor
class PreProcessor final : NonCopyable {
public:
    PreProcessor();

    void import_header(const string& _path);
    pair<string, string> load(const string& _path, const string& _extra = "") const;

private:
    unordered_map<string, string> headerMap;
};

}
