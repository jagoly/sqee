// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <unordered_map>

#include <sqee/gl/Program.hpp>
#include <sqee/misc/Parsing.hpp>

namespace sq {

//============================================================================//

/// The SQEE GLSL PreProcessor.
class SQEE_API PreProcessor final : private NonCopyable
{
public: //====================================================//

    /// Constructor.
    PreProcessor();

    //--------------------------------------------------------//

    /// Import a header from a file.
    void import_header(const String& path);

    /// Update a header from a string.
    void update_header(const String& key, const String& string);

    //--------------------------------------------------------//

    /// Process a GLSL shader source file.
    String process(const String& path, const String& prelude) const;

    //--------------------------------------------------------//

    /// Load a GLSL vertex shader into a Program.
    void load_vertex(Program& program, const String& path, const String& prelude = "") const;

    /// Load a GLSL geometry shader into a Program.
    void load_geometry(Program& program, const String& path, const String& prelude = "") const;

    /// Load a GLSL fragment shader into a Program.
    void load_fragment(Program& program, const String& path, const String& prelude = "") const;

private: //===================================================//

    std::unordered_map<String, TokenisedString> mHeaders;
};

//============================================================================//

} // namespace sq
