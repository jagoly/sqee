#include <fstream>
#include <sstream>

#include "app/logging.hpp"
#include "misc/files.hpp"

using namespace sq;

Json::Value sq::get_json_from_file(const string& _filePath) {
    std::ifstream src(_filePath);
    if (!src.is_open()) {
        log_error("Couldn't open file $0", _filePath);
        return Json::Value();
    }

    Json::Reader reader; Json::Value root;
    if (!reader.parse(src, root)) {
        log_error("Failed to load json from $0$L$1$L", _filePath, reader.getFormattedErrorMessages());
        return Json::Value();
    }

    return root;
}

string sq::get_string_from_file(const string& _filePath) {
    std::ifstream src(_filePath);
    if (!src.is_open()) {
        log_error("Couldn't open file $0", _filePath);
        return string();
    }

    std::stringstream sstr;
    sstr << src.rdbuf();
    return sstr.str();
}

std::vector<std::vector<string>> sq::get_words_from_file(const string& _filePath) {
    std::ifstream src(_filePath);
    std::vector<std::vector<string>> vec;
    if (!src.is_open()) {
        log_error("Couldn't open file $0", _filePath);
        return vec;
    }

    string line; string word;
    while (std::getline(src, line, '\n')) {
        vec.emplace_back();
        std::stringstream sstr(line);
        while (std::getline(sstr, word, ' '))
            vec.back().emplace_back(word);
        if (vec.back().empty()) vec.pop_back();
    }
    return vec;
}
