#include <fstream>
#include <sstream>

#if defined SQEE_LINUX
#include <dirent.h>
#elif defined SQEE_WINDOWS
#include <windows.h>
#endif

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

using std::ifstream;
using std::stringstream;


bool sq::check_file_exists(const string& _path) {
    return ifstream(_path).good();
}


char sq::get_file_first_char(const string& _path) {
    ifstream src(_path);
    if (!src.is_open()) {
        log_error("Couldn't open file %s", _path);
        return char(-1);
    } return src.get();
}


string sq::get_string_from_file(const string& _path) {
    ifstream src(_path);
    if (!src.is_open()) {
        log_error("Couldn't open file %s", _path);
        return string();
    }

    stringstream sstr;
    sstr << src.rdbuf();
    return sstr.str();
}


vector<uchar> sq::get_bytes_from_file(const string& _path) {
    ifstream src(_path, std::ios::binary | std::ios::ate);
    if (!src.is_open()) {
        log_error("Couldn't open file %s", _path);
        return vector<uchar>();
    }

    vector<uchar> retVec(src.tellg());
    src.seekg(0, src.beg);
    src.read((char*)&retVec[0], retVec.size());
    return retVec;
}


vector<string> sq::tokenise_string(const string& _str, char _dlm) {
    stringstream sstr(_str);
    vector<string> retVec; string item;
    while (std::getline(sstr, item, _dlm))
        retVec.emplace_back(item);
    return retVec;
}


vector<pair<vector<string>, uint>> sq::tokenise_file(const string& _path) {
    ifstream src(_path);
    if (!src.is_open()) {
        log_error("Couldn't open file %s", _path);
        return vector<pair<vector<string>, uint>>();
    }

    uint lineNum = 1u;
    stringstream sstr; sstr << src.rdbuf();
    vector<pair<vector<string>, uint>> retVec;
    for (string& line : tokenise_string(sstr.str(), '\n')) {
        vector<string> tokens = tokenise_string(line, ' ');
        if (!tokens.empty()) retVec.emplace_back(tokens, lineNum);
        lineNum += 1u;
    } return retVec;
}


vector<string> sq::get_files_from_dir(const string& _path) {
    vector<string> retVec;

    #if defined SQEE_LINUX
    DIR* dir; struct dirent *ent;
    if ((dir = opendir(_path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL)
            if (ent->d_type == DT_REG)
                retVec.emplace_back(ent->d_name);
        closedir(dir);
    }
    #elif defined SQEE_WINDOWS
    WIN32_FIND_DATA findData;
    string pattern = _path + "/*";
    HANDLE hFind = FindFirstFile(pattern.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            retVec.push_back(findData.cFileName);
        while (FindNextFile(hFind, &findData));
        FindClose(hFind);
    }
    #endif

    else log_error("Couldn't open directory %s", _path);
    return retVec;
}
