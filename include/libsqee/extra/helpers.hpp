#pragma once

#include <jsoncpp/json/json.h>

namespace sqe {

Json::Value load_json_file(std::string filePath);

std::string load_from_file(std::string filePath);

}
