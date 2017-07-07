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


bool sq::check_file_exists(const string& path)
{
    return std::ifstream(path).good();
}


char sq::get_file_first_char(const string& path)
{
    std::ifstream src(path);

    if (src.is_open() == false)
    {
        log_warning("Couldn't open file '%s'", path);
        return char(-1);
    }

    return char(src.get());
}


string sq::get_string_from_file(const string& path)
{
    std::ifstream src(path);

    if (src.is_open() == false)
    {
        log_warning("Couldn't open file '%s'", path);
        return string();
    }

    std::stringstream sstr;
    sstr << src.rdbuf();
    return sstr.str();
}


std::vector<uchar> sq::get_bytes_from_file(const string& path)
{
    using unsigned_ifstream = std::basic_ifstream<uchar>;
    unsigned_ifstream src(path, std::ios::binary | std::ios::ate);
    //std::ifstream src(path, std::ios::binary | std::ios::ate);

    if (src.is_open() == false)
    {
        log_warning("Couldn't open file '%s'", path);
        return {};
    }

    const auto fileSize = src.tellg();

    std::vector<uchar> result;
    result.resize(uint(fileSize));

    src.seekg(0, src.beg);
    src.read(result.data(), fileSize);

    return result;
}


std::vector<string> sq::tokenise_string(const string& _str, char _dlm) {
    std::stringstream sstr(_str);
    std::vector<string> retVec; string item;
    while (std::getline(sstr, item, _dlm))
        if (item.empty() == false)
            retVec.emplace_back(item);
    return retVec;
}


std::vector<std::pair<std::vector<string>, uint>> sq::tokenise_file(const string& _path) {
    std::ifstream src(_path);
    if (src.is_open() == false) {
        log_warning("Couldn't open file %s", _path);
        return {};
    }

    uint lineNum = 1u;
    std::stringstream sstr; sstr << src.rdbuf();
    std::vector<std::pair<std::vector<string>, uint>> retVec;
    for (string& line : tokenise_string(sstr.str(), '\n')) {
        std::vector<string> tokens = tokenise_string(line, ' ');
        if (!tokens.empty()) retVec.emplace_back(tokens, lineNum);
        lineNum += 1u;
    } return retVec;
}


std::vector<string> sq::get_files_from_dir(const string& _path) {
    std::vector<string> retVec;

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

    else log_warning("Couldn't open directory %s", _path);
    return retVec;
}


string sq::file_name_from_path(const string& _path) {
    auto iter = std::find(_path.rbegin(), _path.rend(), '/');
    return string(iter.base(), _path.end());
}


string sq::directory_from_path(const string& _path) {
    auto iter = std::find(_path.rbegin(), _path.rend(), '/');
    return string(_path.begin(), iter.base());
}
