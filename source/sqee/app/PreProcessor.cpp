#include <list>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/app/PreProcessor.hpp>

using namespace sq;

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

void PreProcessor::import_header(const string& path)
{
    const string fullPath = "shaders/" + path + ".glsl";
    const string header = get_string_from_file(fullPath);
    mHeaders[path] = tokenise_string(header, '\n');
}

void PreProcessor::update_header(const string& key, const string& string)
{
    mHeaders[key] = tokenise_string(string, '\n');
}

//============================================================================//

string PreProcessor::process(const string& path, const string& prelude) const
{
    const string fullPath = "shaders/" + path + ".glsl";
    const string fullString = get_string_from_file(fullPath);

    string source = "#version 330 core\n"
                    "#extension GL_ARB_shading_language_420pack : enable\n"
                    "#extension GL_ARB_explicit_uniform_location : enable\n"
                    "#extension GL_ARB_gpu_shader5 : enable\n"
                    "#line 0\n";

    source.reserve(fullString.size()); // might still be reallocated

    source += prelude;

    //--------------------------------------------------------//

    std::vector<std::pair<uint, string>> errorVec;

    const auto base = tokenise_string_view(fullString, '\n');

    //--------------------------------------------------------//

    auto recursive_func = [&](auto&& thisFunc, const std::vector<string_view>& lines) -> void
    {
        for (size_t lineNum = 0u; lineNum < lines.size(); ++lineNum)
        {
            const string_view& line = lines[lineNum];

            if (line.empty() == true)
            {
                source += '\n';
            }
            else if (line.front() == '#')
            {
                const auto tokens = tokenise_string_view(line, ' ');

                if (tokens[0] == "#include")
                {
                    const auto headerName = string(tokens.back());
                    const auto headerFind = mHeaders.find(headerName);

                    if (tokens.size() == 1u)
                        errorVec.emplace_back(lineNum, "#include missing a header path");

                    else if (tokens.size() > 2u)
                        errorVec.emplace_back(lineNum, "#include does not support spaces");

                    else if (headerFind == mHeaders.end())
                        errorVec.emplace_back(lineNum, "#include header has not been imported");

                    else
                    {
                        source += tfm::format("#line 0 // begin '%s'\n", headerName);

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
        string errorLines;
        for (const std::pair<uint, string>& error : errorVec)
            errorLines += tfm::format("\nLine %d: %s", error.first, error.second);

        log_error("Failed to pre-process shader from \"%s\"%s", path, errorLines);
    }

    return source;
}

//============================================================================//

void PreProcessor::load_vertex(Program& program, const string& path, const string& prelude) const
{
    program.load_vertex(this->process(path, prelude), path);
}

void PreProcessor::load_geometry(Program& program, const string& path, const string& prelude) const
{
    program.load_geometry(this->process(path, prelude), path);
}

void PreProcessor::load_fragment(Program& program, const string& path, const string& prelude) const
{
    program.load_fragment(this->process(path, prelude), path);
}
