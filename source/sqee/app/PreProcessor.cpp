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
    const string fileStr = get_string_from_file(fullPath);

    std::vector<string> lineVec = tokenise_string(fileStr, '\n');
    std::list<string> lines(lineVec.begin(), lineVec.end());

    string source = "#version 330 core\n"
                    "#extension GL_ARB_shading_language_420pack : enable\n"
                    "#extension GL_ARB_explicit_uniform_location : enable\n"
                    "#extension GL_ARB_gpu_shader5 : enable\n"
                    "#line 0\n";

    source.append(prelude);

    std::vector<std::pair<uint, string>> errorVec;
    uint lineNum = 0u, tokenNum = 0u;

    for (auto it = lines.begin(); it != lines.end(); ++it)
    {
        std::vector<string> tokens = tokenise_string(*it, ' ');

        if (tokens.empty() == false)
        {
            if (tokens[0] == "#version")
                errorVec.emplace_back(lineNum, "#version is added automatically");

            if (tokens[0] == "#extension")
                errorVec.emplace_back(lineNum, "extensions are added automatically");

            if (errorVec.empty() == false) break;

            if (tokens[0] == "#include")
            {
                if (tokens.size() == 1u)
                    errorVec.emplace_back(lineNum, "#include missing a header path");

                if (tokens.size() > 2u)
                    errorVec.emplace_back(lineNum, "#include does not support spaces");

                if (mHeaders.count(tokens[1]) == 0u)
                    errorVec.emplace_back(lineNum, "#include header has not been imported");

                if (errorVec.empty() == false) break;

                std::vector<string> header = mHeaders.at(tokens[1]);
                header.emplace_back("#line " + std::to_string(++lineNum));
                lines.insert(std::next(it), header.begin(), header.end());

                it = lines.erase(std::prev(it));
                tokenNum += uint(header.size());
            }

            else source += *it + '\n';
        }

        else source += '\n';

        if (tokenNum == 0u) ++lineNum;
        else --tokenNum;
    }

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
