#pragma once

#include <unordered_map>

#include <sqee/gl/Shaders.hpp>

namespace sq {

//============================================================================//

/// The SQEE GLSL PreProcessor
class PreProcessor final : NonCopyable
{
public:

    //========================================================//

    /// Constructor
    PreProcessor();

    //========================================================//

    void import_header(const string& path);
    void update_header(const string& key, const string& string);

    void load(Shader& shader, const string& path, const string& extra = "") const;
    void operator()(Shader& shader, const string& path, const string& extra = "") const;

private:

    //========================================================//

    std::unordered_map<string, vector<string>> mHeaders;
};

//============================================================================//

} // namespace sq
