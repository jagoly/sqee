#pragma once

#include <sstream>

#include <jsoncpp/json/json.h>

namespace sqe {

Json::Value load_json_file(std::string filePath);

}
