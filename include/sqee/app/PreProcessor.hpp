#pragma once
#include <sqee/forward.hpp>
#include <unordered_map>

namespace sq {

/// The SQEE GLSL PreProcessor
class PreProcessor final : NonCopyable {
public:
    PreProcessor();

    void import_header(const string& _path);
    void load(Shader& _shader, const string& _path, const string& _extra = "") const;
    void operator()(Shader& _shader, const string& _path, const string& _extra = "") const;

private:
    std::unordered_map<string, vector<string>> headerMap;
};

}
