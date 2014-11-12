#include <iostream>
#include <fstream>
#include <sstream>

#include <misc/files.hpp>

namespace sq {

Json::Value get_json_from_file(const std::string& _path) {
    std::ifstream src(_path);

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(src, root)) {
        std::cout << "ERROR: Failed to load json from \"" << _path << "\"\n"
                  << "-------------------------\n"
                  << reader.getFormattedErrorMessages()
                  << "-------------------------" << std::endl;
    }
    src.close();

    return root;
}

std::string get_string_from_file(const std::string& _path) {
    std::ifstream src;
    src.open(_path);
    std::stringstream sstr;
    sstr << src.rdbuf();
    src.close();
    return sstr.str();
}

}
