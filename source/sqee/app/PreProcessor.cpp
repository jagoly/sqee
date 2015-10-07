#include <list>

#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/app/PreProcessor.hpp>

using namespace sq;

extern const char data_funcs_depth[];
extern const char data_funcs_random[];
extern const char data_funcs_colours[];
extern const char data_disks_uniform[];
extern const char data_blocks_camera[];
extern const char data_data_screen[];

const string shader_prelude =
"#version 420 core\n"
"#line 0\n";

PreProcessor::PreProcessor() {
    headerMap.emplace("builtin/funcs/depth", tokenise_string(data_funcs_depth, '\n'));
    headerMap.emplace("builtin/funcs/random", tokenise_string(data_funcs_random, '\n'));
    headerMap.emplace("builtin/funcs/colours", tokenise_string(data_funcs_colours, '\n'));
    headerMap.emplace("builtin/disks/uniform", tokenise_string(data_disks_uniform, '\n'));
    headerMap.emplace("builtin/blocks/camera", tokenise_string(data_blocks_camera, '\n'));
    headerMap.emplace("builtin/data/screen", tokenise_string(data_data_screen, '\n'));
}

void PreProcessor::import_header(const string& _path) {
    string headerStr = get_string_from_file("shaders/"+_path+".glsl");
    headerMap.emplace(_path, tokenise_string(headerStr, '\n'));
}

void PreProcessor::load(Shader& _shader, const string& _path, const string& _extra) const {
    string path = "shaders/" + _path + ".glsl";
    string fileStr = get_string_from_file(path);
    vector<string> lineVec = tokenise_string(fileStr, '\n');
    std::list<string> lines(lineVec.begin(), lineVec.end());

    string pSource = shader_prelude + _extra;
    vector<pair<uint, string>> errorVec;
    uint lineNum = 0u, tokenNum = 0u;

    for (auto it = lines.begin(); it != lines.end(); ++it) {
        vector<string> tokens = tokenise_string(*it, ' ');
        if (tokens.empty() == false) {
            if (tokens[0] == "#version")
                errorVec.emplace_back(lineNum, "#version is added automatically");
            if (tokens[0] == "#extension")
                errorVec.emplace_back(lineNum, "extensions are added automatically");
            if (tokens[0] == "#include") {
                if (tokens.size() == 1u)
                    errorVec.emplace_back(lineNum, "#include missing a header path");
                if (tokens.size() > 2u)
                    errorVec.emplace_back(lineNum, "#include does not support spaces");
                if (headerMap.count(tokens[1]) == 0u)
                    errorVec.emplace_back(lineNum, "#include header has not been imported");
                vector<string> hTokens = headerMap.at(tokens[1]);
                hTokens.emplace_back("#line " + std::to_string(++lineNum));
                std::list<string>::iterator itB = it; std::advance(it, 1);
                lines.insert(it, hTokens.begin(), hTokens.end());
                it = lines.erase(std::prev(itB));
                tokenNum += hTokens.size();
            } else pSource += *it + '\n';
        } else pSource += '\n';

        if (tokenNum == 0u) lineNum += 1u;
        else tokenNum -= 1u;
    }

    if (errorVec.empty() == false) { string errorLines;
        for (const pair<uint, string>& error : errorVec)
            errorLines += tfm::format("\nLine %d: %s", error.first, error.second);
        log_error("Failed to process shader from \"%s\"%s", path, errorLines);
    } else _shader.load(pSource, path);
}

void PreProcessor::operator()(Shader& _shader, const string& _path, const string& _extra) const {
    load(_shader, _path, _extra);
}
