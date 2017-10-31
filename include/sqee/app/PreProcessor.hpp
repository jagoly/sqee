#pragma once

#include <unordered_map>

#include <sqee/misc/Parsing.hpp>

#include <sqee/gl/Program.hpp>

namespace sq {

//============================================================================//

/// The SQEE GLSL PreProcessor.
class PreProcessor final : NonCopyable
{
public: //====================================================//

    /// Constructor.
    PreProcessor();

    //--------------------------------------------------------//

    /// Import a header from a file.
    void import_header(const string& path);

    /// Update a header from a string.
    void update_header(const string& key, const string& string);

    //--------------------------------------------------------//

    /// Process a GLSL shader source file.
    string process(const string& path, const string& prelude) const;

    //--------------------------------------------------------//

    /// Load a GLSL vertex shader into a Program.
    void load_vertex(Program& program, const string& path, const string& prelude = "") const;

    /// Load a GLSL geometry shader into a Program.
    void load_geometry(Program& program, const string& path, const string& prelude = "") const;

    /// Load a GLSL fragment shader into a Program.
    void load_fragment(Program& program, const string& path, const string& prelude = "") const;

private: //===================================================//

    std::unordered_map<string, TokenisedString> mHeaders;
};

//============================================================================//

} // namespace sq
