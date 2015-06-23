#include <dirent.h>
#include <fstream>
#include <sstream>

#include <sqee/app/Logging.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

bool sq::check_file_exists(const string& _path) {
    std::ifstream infile(_path);
    return infile.good();
}

char sq::get_file_first_byte(const string& _path) {
    std::ifstream src(_path);
    if (!src.is_open()) {
        log_error("Couldn't open file %s", _path);
        return -1;
    }
    return src.get();
}

Json::Value sq::get_json_from_file(const string& _path) {
    std::ifstream src(_path);
    if (!src.is_open()) {
        log_error("Couldn't open file %s", _path);
        return Json::Value();
    }

    Json::Reader reader; Json::Value root;
    if (!reader.parse(src, root)) {
        log_error("Failed to load json from %s\n%s\n", _path, reader.getFormattedErrorMessages());
        return Json::Value();
    }

    return root;
}

string sq::get_string_from_file(const string& _path) {
    std::ifstream src(_path);
    if (!src.is_open()) {
        log_error("Couldn't open file %s", _path);
        return string();
    }

    std::stringstream sstr;
    sstr << src.rdbuf();
    return sstr.str();
}

vector<char> sq::get_bytes_from_file(const string& _path) {
    std::ifstream src(_path, std::ios::binary | std::ios::ate);
    if (!src.is_open()) {
        log_error("Couldn't open file %s", _path);
        return vector<char>();
    }

    vector<char> retVec(src.tellg());
    src.seekg(0, src.beg);
    src.read((char*)&retVec[0], retVec.size());

    return retVec;
}

vector<vector<string>> sq::get_words_from_file(const string& _path) {
    vector<vector<string>> retVec;
    std::ifstream src(_path);
    if (!src.is_open()) {
        log_error("Couldn't open file %s", _path);
        return retVec;
    }

    string line; string word;
    while (std::getline(src, line, '\n')) {
        retVec.emplace_back();
        std::stringstream sstr(line);
        while (std::getline(sstr, word, ' '))
            retVec.back().emplace_back(word);
        if (retVec.back().empty()) retVec.pop_back();
    } return retVec;
}

vector<string> sq::get_files_from_dir(const string& _path) {
    vector<string> retVec;
    DIR* dir; struct dirent *ent;
    if ((dir = opendir(_path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL)
            if (ent->d_type == DT_REG)
                retVec.emplace_back(ent->d_name);
        closedir(dir);
    } else log_error("Couldn't open directory %s", _path);
    return retVec;
}
