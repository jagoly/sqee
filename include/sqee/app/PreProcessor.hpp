#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

// Forward Declarations /////
namespace sq { class Shader; }

namespace sq {

/// The SQEE GLSL PreProcessor
class PreProcessor final : NonCopyable {
public:
    PreProcessor();

    void import_header(const string& _path);
    void update_header(const string& _key, const string& _string);
    void load(Shader& _shader, const string& _path, const string& _extra = "") const;
    void operator()(Shader& _shader, const string& _path, const string& _extra = "") const;

private:
    std::unordered_map<string, vector<string>> headerMap;
};

}
