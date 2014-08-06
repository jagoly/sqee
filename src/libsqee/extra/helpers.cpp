#include <fstream>
#include <sstream>

#include <extra/helpers.hpp>

namespace sq {

Json::Value load_json_file(std::string filePath) {
    std::ifstream src(filePath);

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(src, root)) {
        std::cout << "file load failed:" << std::endl << reader.getFormattedErrorMessages();
    }
    src.close();

    return root;
}

std::string load_from_file(std::string filePath) {
    std::ifstream src;
    src.open(filePath);
    std::stringstream sstr;
    sstr << src.rdbuf();
    src.close();
    return sstr.str();
}

}
