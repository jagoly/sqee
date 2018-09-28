// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/app/PreProcessor.hpp>

#include <list>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/helpers.hpp>

using namespace sq;
using literals::operator""_fmt_;

//============================================================================//

extern const char data_glsl_misc_screen[];
extern const char data_glsl_funcs_position[];
extern const char data_glsl_funcs_depth[];
extern const char data_glsl_funcs_random[];
extern const char data_glsl_funcs_colour[];
extern const char data_glsl_misc_disks[];

//============================================================================//

PreProcessor::PreProcessor()
{
    mHeaders.emplace("builtin/misc/screen", tokenise_string(data_glsl_misc_screen, '\n'));
    mHeaders.emplace("builtin/funcs/position", tokenise_string(data_glsl_funcs_position, '\n'));
    mHeaders.emplace("builtin/funcs/depth", tokenise_string(data_glsl_funcs_depth, '\n'));
    mHeaders.emplace("builtin/funcs/random", tokenise_string(data_glsl_funcs_random, '\n'));
    mHeaders.emplace("builtin/funcs/colour", tokenise_string(data_glsl_funcs_colour, '\n'));
    mHeaders.emplace("builtin/misc/disks", tokenise_string(data_glsl_misc_disks, '\n'));
}

//============================================================================//

void PreProcessor::import_header(const String& path)
{
    const auto fullPath = build_string("shaders/" + path + ".glsl");
    mHeaders[path] = tokenise_string(get_string_from_file(fullPath), '\n');
}

void PreProcessor::update_header(const String& key, const String& string)
{
    mHeaders[key] = tokenise_string(string, '\n');
}

//============================================================================//

String PreProcessor::process(const String& path, const String& prelude) const
{
    const String fullPath = [&]()
    {
        const bool isAbsolute = ( path.front() == '/' );
        const bool hasExtension = !extension_from_path(path).empty();

        if (!isAbsolute && !hasExtension) return build_string("shaders/", path, ".glsl");
        if (!isAbsolute)                  return build_string("shaders/", path);
        if (!hasExtension)                return build_string(path, ".glsl");
        return path;
    }();

    const String fullString = get_string_from_file(fullPath);

    String source = "#version 330 core\n"
                         "#extension GL_ARB_shading_language_420pack : enable\n"
                         "#extension GL_ARB_explicit_uniform_location : enable\n"
                         "#extension GL_ARB_gpu_shader5 : enable\n"
                         "#line 0\n";

    source.reserve(source.size() + fullString.size()); // might still be reallocated

    source += prelude;

    //--------------------------------------------------------//

    Vector<std::pair<uint, String>> errorVec;

    const auto base = tokenise_string_view(fullString, '\n');

    //--------------------------------------------------------//

    auto recursive_func = [&](auto&& thisFunc, const Vector<StringView>& lines) -> void
    {
        for (size_t lineNum = 0u; lineNum < lines.size(); ++lineNum)
        {
            const StringView& line = lines[lineNum];

            if (line.empty() == true)
            {
                source += '\n';
            }
            else if (line.front() == '#')
            {
                const auto tokens = tokenise_string_view(line, ' ');

                if (tokens[0] == "#include")
                {
                    const auto headerName = String(tokens.back());
                    const auto headerFind = mHeaders.find(headerName);

                    if (tokens.size() == 1u)
                        errorVec.emplace_back(lineNum, "#include missing a header path");

                    else if (tokens.size() > 2u)
                        errorVec.emplace_back(lineNum, "#include does not support spaces");

                    else if (headerFind == mHeaders.end())
                        errorVec.emplace_back(lineNum, "header '%s' has not been imported"_fmt_(headerName));

                    else
                    {
                        source += build_string("#line 0 // begin '", headerName, "'\n");

                        thisFunc(thisFunc, headerFind->second.tokens);
                    }
                }

                else if (tokens[0] == "#version")
                    errorVec.emplace_back(lineNum, "#version is added automatically");

                else if (tokens[0] == "#extension")
                    errorVec.emplace_back(lineNum, "extensions are added automatically");

                else { source += line; source += '\n'; } // #define, #ifdef, etc.
            }

            else { source += line; source += '\n'; }
        }
    };

    recursive_func(recursive_func, base);

    //--------------------------------------------------------//

    if (errorVec.empty() == false)
    {
        String errorLines;
        for (const std::pair<uint, String>& error : errorVec)
            errorLines += "\nLine %d: %s"_fmt_(error.first, error.second);

        log_error("Failed to pre-process shader from \"%s\"%s", path, errorLines);
    }

    return source;
}

//============================================================================//

void PreProcessor::load_vertex(Program& program, const String& path, const String& prelude) const
{
    program.load_vertex(this->process(path, prelude), path);
}

void PreProcessor::load_geometry(Program& program, const String& path, const String& prelude) const
{
    program.load_geometry(this->process(path, prelude), path);
}

void PreProcessor::load_fragment(Program& program, const String& path, const String& prelude) const
{
    program.load_fragment(this->process(path, prelude), path);
}
