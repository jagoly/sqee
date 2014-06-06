#include <fstream>

#include <extra/helpers.hpp>

using namespace sqe;

Json::Value sqe::load_json_file(std::string filePath) {
    std::ifstream src(filePath);

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(src, root)) {
        std::cout << "file load failed:" << std::endl << reader.getFormattedErrorMessages();
    }
    src.close();

    return root;
}
