#include <sqee/app/PreProcessor.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

extern const char data_funcs_depth[];
extern const char data_funcs_random[];
extern const char data_funcs_colours[];
extern const char data_disks_uniform[];
extern const char data_blocks_camera[];

const string shader_prelude =
"#version 420 core\n"
"#line 0\n";

PreProcessor::PreProcessor() {
    headerMap.emplace("builtin/funcs/depth", data_funcs_depth);
    headerMap.emplace("builtin/funcs/random", data_funcs_random);
    headerMap.emplace("builtin/funcs/colours", data_funcs_colours);
    headerMap.emplace("builtin/disks/uniform", data_disks_uniform);
    headerMap.emplace("builtin/blocks/camera", data_blocks_camera);
}

void PreProcessor::import_header(const string& _path) {
    headerMap.emplace(_path, get_string_from_file("shaders/"+_path+".glsl"));
}

pair<string, string> PreProcessor::load(const string& _path, const string& _extra) const {
    string path = "shaders/" + _path + ".glsl";
    string fileStr = get_string_from_file(path);
    vector<string> lineVec = tokenise_string(fileStr, '\n');
    list<string> lines(lineVec.begin(), lineVec.end());
    string retStr = shader_prelude + _extra;

    uint lineNum = 0u; uint tokenNum = 0u;
    for (auto it = lines.begin(); it != lines.end(); ++it) {
        vector<string> tokens = tokenise_string(*it, ' ');
        if (tokens.empty() == false) {
            if (tokens[0] == "#version") {
                log_error("Failed to process shader from %s\nline %d: "
                          "#version is added automatically", path, lineNum);
                return pair<string, string>("", path); }
            if (tokens[0] == "#extension") {
                log_error("Failed to process shader from %s\nline %d: "
                          "#extensions are added automatically", path, lineNum);
                return pair<string, string>("", path); }
            if (tokens[0] == "#include") {
                if (tokens.size() != 2u) {
                    log_error("Failed to process shader from %s\nline %d: "
                              "#include does not support spaces", path, lineNum);
                    return pair<string, string>("", path); }
                if (headerMap.count(tokens[1]) == 0u) {
                    log_error("Failed to process shader from %s\nline %d: "
                              "\"%s\" has not been imported", path, lineNum, tokens[1]);
                    return pair<string, string>("", path); }
                const string& header = headerMap.at(tokens[1]);
                vector<string> hTokens = tokenise_string(header, '\n');
                hTokens.emplace_back(tfm::format("#line %d", ++lineNum));
                list<string>::iterator itB = it; std::advance(it, 1);
                lines.insert(it, hTokens.begin(), hTokens.end());
                it = lines.erase(std::prev(itB));
                tokenNum += hTokens.size();
            } else retStr += *it + '\n';
        } else retStr += '\n';

        if (tokenNum == 0u) lineNum += 1u;
        else tokenNum -= 1u;
    }

    return pair<string, string>(retStr, path);
}
