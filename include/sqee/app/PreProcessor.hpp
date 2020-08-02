// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class Program;

//============================================================================//

/// The SQEE GLSL PreProcessor.
class SQEE_API PreProcessor final : private NonCopyable
{
public: //====================================================//

    /// Constructor.
    PreProcessor();

    //--------------------------------------------------------//

    /// Import a header from a file.
    void import_header(String path);

    /// Update a header from a string.
    void update_header(String path, String source);

    //--------------------------------------------------------//

    /// Process a GLSL shader source file.
    String process(StringView path, StringView prelude) const;

    //--------------------------------------------------------//

    /// Load a GLSL vertex shader into a Program.
    void load_vertex(Program& program, StringView path, StringView prelude = "") const;

    /// Load a GLSL geometry shader into a Program.
    void load_geometry(Program& program, StringView path, StringView prelude = "") const;

    /// Load a GLSL fragment shader into a Program.
    void load_fragment(Program& program, StringView path, StringView prelude = "") const;

private: //===================================================//

    struct TokenisedHeader
    {
        String source;
        std::vector<StringView> lines;
    };

    std::unordered_map<String, TokenisedHeader> mHeaders;
};

//============================================================================//

} // namespace sq
