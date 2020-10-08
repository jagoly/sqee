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

    using OptionMap = std::map<SmallString, SmallString>;

    //--------------------------------------------------------//

    PreProcessor(); ///< Constructor.

    /// Add or update a header from a file.
    void import_header(const String& path);

    /// Add or update a header with a string.
    void import_header(const String& path, String source);

    //--------------------------------------------------------//

    /// Process any kind of GLSL shader source file.
    String process(const String& path, const OptionMap& options, bool prelude) const;

    /// Load a GLSL vertex shader into a program.
    void load_vertex(Program& program, const String& path, const OptionMap& options) const;

    /// Load a GLSL geometry shader into a program.
    void load_geometry(Program& program, const String& path, const OptionMap& options) const;

    /// Load a GLSL fragment shader into a program.
    void load_fragment(Program& program, const String& path, const OptionMap& options) const;

    /// Load a GLSL super shader into a Program.
    void load_super_shader(Program& program, const String& path, const OptionMap& options) const;

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
