#include <fstream>
#include <sstream>

#include <misc/files.hpp>

using namespace sq;

Json::Value sq::get_json_from_file(const string& _filePath) {
    std::ifstream src(_filePath);
    if (!src.is_open()) {
        cout << "ERROR: Couldn't open file \"" << _filePath << "\"" << endl;
        return Json::Value();
    }

    Json::Reader reader; Json::Value root;
    if (!reader.parse(src, root))
        cout << "ERROR: Failed to load json from \"" << _filePath << "\"\n"
             << "-------------------------\n"
             << reader.getFormattedErrorMessages()
             << "-------------------------" << endl;
    return root;
}

string sq::get_string_from_file(const string& _filePath) {
    std::ifstream src(_filePath);
    if (!src.is_open()) {
        cout << "ERROR: Couldn't open file \"" << _filePath << "\"" << endl;
        return string();
    }

    std::stringstream sstr;
    sstr << src.rdbuf();
    return sstr.str();
}

vector<vector<string>> sq::get_words_from_file(const string& _filePath) {
    std::ifstream src(_filePath);
    vector<vector<string>> vec;
    if (!src.is_open()) {
        cout << "ERROR: Couldn't open file \"" << _filePath << "\"" << endl;
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
