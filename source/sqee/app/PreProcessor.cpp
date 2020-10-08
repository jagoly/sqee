#include <sqee/app/PreProcessor.hpp>

#include <sqee/core/Utilities.hpp>
#include <sqee/data/ShaderHeaders.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Program.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Parsing.hpp>
#include <sqee/core/Algorithms.hpp>

#include <fmt/format.h>

using namespace sq;

//============================================================================//

constexpr const char PRELUDE_VERSION[] = "#version 450 core\n";

constexpr const char PRELUDE_VERTEX[] = "#define SHADER_STAGE_VERTEX\n";
//constexpr const char PRELUDE_GEOMETRY[] = "#define SHADER_STAGE_GEOMETRY\n";
constexpr const char PRELUDE_FRAGMENT[] = "#define SHADER_STAGE_FRAGMENT\n";

PreProcessor::PreProcessor()
{
    import_header("builtin/misc/screen", sqee_glsl_misc_screen);
    import_header("builtin/funcs/position", sqee_glsl_funcs_position);
    import_header("builtin/funcs/depth", sqee_glsl_funcs_depth);
    import_header("builtin/funcs/random", sqee_glsl_funcs_random);
    import_header("builtin/funcs/colour", sqee_glsl_funcs_colour);
    import_header("builtin/misc/disks", sqee_glsl_misc_disks);
}

//============================================================================//

void PreProcessor::import_header(const String& path)
{
    TokenisedHeader& header = mHeaders[path];

    header.source = get_string_from_file(build_string("shaders/", path, ".glsl"));
    header.lines = tokenise_string_lines(header.source);
}

void PreProcessor::import_header(const String& path, String source)
{
    TokenisedHeader& header = mHeaders[path];

    header.source = std::move(source);
    header.lines = tokenise_string_lines(header.source);
}

//============================================================================//

String PreProcessor::process(const String& path, const OptionMap& options, bool prelude) const
{
    const auto source = try_get_string_from_file(path);

    if (source.has_value() == false)
        log_error("Failed to find shader '{}'", path);

    String result; // includes will still cause a reallocation
    if (prelude == true)
    {
        result.reserve(sizeof(PRELUDE_VERSION) - 1u + source->size());
        result = PRELUDE_VERSION;
    }
    else result.reserve(source->size());

    std::vector<StringView> tokens;
    std::vector<OptionMap::const_iterator> usedOptions;

    StringView file = path; // changes for headers
    String errors; // print errors afterwards

    //--------------------------------------------------------//

    auto recursive_func = [&](auto thisFunc, const std::vector<StringView>& srcLines) -> void
    {
        size_t lineIndex = 0u;

        const auto write_error_message = [&](StringView msg, const auto&... args)
        {
            fmt::format_to(std::back_inserter(errors), "\n{}:{}: ", file, lineIndex + 1u);
            fmt::format_to(std::back_inserter(errors), msg, args...);
        };

        // start of a shader after prelude is added, or start of a header
        result += "#line 1\n";

        for (lineIndex = 0u; lineIndex != srcLines.size(); ++lineIndex)
        {
            const StringView srcLine = srcLines[lineIndex];

            if (srcLine[srcLine.find_first_not_of(' ')] == '#')
            {
                // don't include newline character in tokens
                tokenise_string(srcLine.substr(0u, srcLine.size() - 1u), ' ', tokens);

                if (tokens[0] == "#include")
                {
                    // include paths cannot contain spaces
                    if (tokens.size() == 2u)
                    {
                        const auto iter = mHeaders.find(String(tokens[1]));
                        if (iter != mHeaders.end())
                        {
                            file = tokens[1];
                            thisFunc(thisFunc, iter->second.lines);
                            file = path;
                        }
                        else write_error_message("invalid header '{}'", tokens[1]);
                    }
                    else write_error_message("#include given wrong number of arguments");
                }

                else if (tokens[0] == "#option")
                {
                    // option values cannot contain spaces
                    if (tokens.size() == 3u)
                    {
                        const auto iter = options.find(tokens[1]);
                        if (iter != options.end())
                        {
                            fmt::format_to(std::back_inserter(result), "#define {} {}\n", iter->first, iter->second);
                            if (algo::find(usedOptions, iter) == usedOptions.end())
                                usedOptions.push_back(iter);
                            else write_error_message("duplicate option '{}'", tokens[1]);
                        }
                        else fmt::format_to(std::back_inserter(result), "#define {} {}\n", tokens[1], tokens[2]);
                    }
                    else write_error_message("#option given wrong number of arguments");
                }

                else if (tokens[0] == "#version")
                    write_error_message("glsl version is added automatically");

                else if (tokens[0] == "#extension")
                    write_error_message("glsl extensions are added automatically");

                else result += srcLine; // #define, #ifdef, #line, etc.
            }

            else result += srcLine; // all other lines
        }
    };

    recursive_func(recursive_func, tokenise_string_lines(*source));

    //--------------------------------------------------------//

    // if any of the given options do not exist in the shader, that is an error
    if (usedOptions.size() < options.size())
        for (auto iter = options.begin(); iter != options.end(); ++iter)
            if (algo::find(usedOptions, iter) == usedOptions.end())
                fmt::format_to(std::back_inserter(errors), "\ninvalid option '{}'", iter->first);

    if (errors.empty() == false)
        log_error_multiline("Failed to process shader from '{}'{}", path, errors);

    return result;
}

//============================================================================//

void PreProcessor::load_vertex(Program& program, const String& path, const OptionMap& options) const
{
    program.load_shader(ShaderStage::Vertex, process(path, options, true), path);
}

void PreProcessor::load_geometry(Program& program, const String& path, const OptionMap& options) const
{
    program.load_shader(ShaderStage::Geometry, process(path, options, true), path);
}

void PreProcessor::load_fragment(Program& program, const String& path, const OptionMap& options) const
{
    program.load_shader(ShaderStage::Fragment, process(path, options, true), path);
}

//============================================================================//

void PreProcessor::load_super_shader(Program& program, const String& path, const OptionMap& options) const
{
    const String processed = process(path, options, false);

    // todo: super shaders should support other stage combinations
    program.load_shader(ShaderStage::Vertex, build_string(PRELUDE_VERSION, PRELUDE_VERTEX, processed), path);
    program.load_shader(ShaderStage::Fragment, build_string(PRELUDE_VERSION, PRELUDE_FRAGMENT, processed), path);

    program.link_program_stages();
}
