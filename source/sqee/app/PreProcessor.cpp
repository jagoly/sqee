#include <sqee/app/PreProcessor.hpp>

#include <sqee/core/Utilities.hpp>
#include <sqee/data/ShaderHeaders.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Program.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Parsing.hpp>

#include <fmt/format.h>

using namespace fmt::literals;

using namespace sq;

//============================================================================//

static constexpr const StringView SHADER_PRELUDE =
"#version 330 core\n"
"#extension GL_ARB_shading_language_420pack : enable\n"
"#extension GL_ARB_explicit_uniform_location : enable\n"
"#extension GL_ARB_gpu_shader5 : enable\n";

//============================================================================//

PreProcessor::PreProcessor()
{
    update_header("builtin/misc/screen", sqee_glsl_misc_screen);
    update_header("builtin/funcs/position", sqee_glsl_funcs_position);
    update_header("builtin/funcs/depth", sqee_glsl_funcs_depth);
    update_header("builtin/funcs/random", sqee_glsl_funcs_random);
    update_header("builtin/funcs/colour", sqee_glsl_funcs_colour);
    update_header("builtin/misc/disks", sqee_glsl_misc_disks);
}

//============================================================================//

void PreProcessor::import_header(String path)
{
    const auto fullPath = build_string("shaders/", path, ".glsl");
    TokenisedHeader& header = mHeaders[std::move(path)];
    header.source = get_string_from_file(fullPath);
    header.lines = tokenise_string_lines(header.source);
}

void PreProcessor::update_header(String path, String source)
{
    TokenisedHeader& header = mHeaders[std::move(path)];
    header.source = std::move(source);
    header.lines = tokenise_string_lines(header.source);
}

//============================================================================//

String PreProcessor::process(StringView path, StringView prelude) const
{
    const auto fullPath = compute_resource_path("shaders/", path, {"glsl"});
    if (fullPath == std::nullopt) log_error("Failed to find shader '{}'", path);

    const String source = get_string_from_file(*fullPath);

    String result;

    // just a starting size, may still be reallocated due to headers
    result.reserve(SHADER_PRELUDE.size() + prelude.size() + source.size());

    result += SHADER_PRELUDE;
    result += prelude;

    const auto sourceLines = tokenise_string_lines(source);

    String errors;

    //--------------------------------------------------------//

    auto recursive_func = [this](auto thisFunc, const auto& lines, String& result, String& errors) -> void
    {
        result += "#line 1\n";

        for (size_t lineIndex = 0u; lineIndex != lines.size(); ++lineIndex)
        {
            const size_t lineNum = lineIndex + 1u;
            const StringView line = lines[lineIndex];

            // keep empty lines around for readability in tools like renderdoc
            if (line.empty() == true) result += '\n';

            else if (line.front() == '#')
            {
                const auto tokens = tokenise_string(line, ' ');

                if (tokens[0] == "#include")
                {
                    if (tokens.size() == 1u)
                        errors += "\nLine {}: #include missing a header path"_format(lineNum);

                    else if (tokens.size() > 2u)
                        errors += "\nLine {}: #include does not support spaces"_format(lineNum);

                    else
                    {
                        const auto headerName = String(tokens.back());

                        if (const auto findIter = mHeaders.find(headerName); findIter != mHeaders.end())
                        {
                            thisFunc(thisFunc, findIter->second.lines, result, errors);
                            result += "#line {}\n"_format(lineNum+1u);
                        }
                        else errors += "\nLine {}: header '{}' has not been imported"_format(lineNum, headerName);
                    }
                }

                else if (tokens[0] == "#version")
                    errors += "\nLine {}: #version is added automatically"_format(lineNum);

                else if (tokens[0] == "#extension")
                    errors += "\nLine {}: extensions are added automatically"_format(lineNum);

                else if (tokens[0] == "#line")
                    errors += "\nLine {}: #line is reserved for preprocessing"_format(lineNum);

                else { result += line; result += '\n'; } // #define, #ifdef, etc.
            }

            else { result += line; result += '\n'; }
        }
    };

    //--------------------------------------------------------//

    recursive_func(recursive_func, sourceLines, result, errors);

    if (errors.empty() == false)
        log_error_multiline("Failed to pre-process shader from '{}'{}", path, errors);

    return result;
}

//============================================================================//

void PreProcessor::load_vertex(Program& program, StringView path, StringView prelude) const
{
    program.load_vertex(process(path, prelude), path);
}

void PreProcessor::load_geometry(Program& program, StringView path, StringView prelude) const
{
    program.load_geometry(process(path, prelude), path);
}

void PreProcessor::load_fragment(Program& program, StringView path, StringView prelude) const
{
    program.load_fragment(process(path, prelude), path);
}
