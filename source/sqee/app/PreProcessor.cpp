#include <list>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/app/PreProcessor.hpp>

using namespace sq;

//============================================================================//

extern const char data_funcs_depth[];
extern const char data_funcs_random[];
extern const char data_funcs_colours[];
extern const char data_disks_uniform[];
extern const char data_data_screen[];

//============================================================================//

PreProcessor::PreProcessor()
{
    mHeaders.emplace("builtin/funcs/depth", tokenise_string(data_funcs_depth, '\n'));
    mHeaders.emplace("builtin/funcs/random", tokenise_string(data_funcs_random, '\n'));
    mHeaders.emplace("builtin/funcs/colours", tokenise_string(data_funcs_colours, '\n'));
    mHeaders.emplace("builtin/disks/uniform", tokenise_string(data_disks_uniform, '\n'));
    mHeaders.emplace("builtin/data/screen", tokenise_string(data_data_screen, '\n'));
}

//============================================================================//

void PreProcessor::import_header(const string& path)
{
    const string fullPath = "shaders/" + path + ".glsl";
    const string header = get_string_from_file(fullPath);
    mHeaders.emplace(path, tokenise_string(header, '\n'));
}

//============================================================================//

void PreProcessor::update_header(const string& key, const string& string)
{
    mHeaders[key] = tokenise_string(string, '\n');
}

//============================================================================//

void PreProcessor::load(Shader& shader, const string& path, const string& extra) const
{
    const string fullPath = "shaders/" + path + ".glsl";
    const string fileStr = get_string_from_file(fullPath);

    vector<string> lineVec = tokenise_string(fileStr, '\n');
    std::list<string> lines(lineVec.begin(), lineVec.end());

    string source = "#version 420 core\n#line 0\n";
    source.append(extra);

    vector<pair<uint, string>> errorVec;
    uint lineNum = 0u, tokenNum = 0u;

    for (auto it = lines.begin(); it != lines.end(); ++it)
    {
        vector<string> tokens = tokenise_string(*it, ' ');

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

                vector<string> header = mHeaders.at(tokens[1]);
                header.emplace_back("#line " + std::to_string(++lineNum));
                lines.insert(std::next(it), header.begin(), header.end());

                it = lines.erase(std::prev(it));
                tokenNum += header.size();
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
        for (const pair<uint, string>& error : errorVec)
            errorLines += tfm::format("\nLine %d: %s", error.first, error.second);

        log_warning("Failed to process shader from \"%s\"%s", path, errorLines);
    }

    else shader.load(source, path);
}

//============================================================================//

void PreProcessor::operator()(Shader& shader, const string& path, const string& extra) const
{
    load(shader, path, extra);
}
