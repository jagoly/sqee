#pragma once
#include "forward.hpp"

#include <unordered_map>

namespace sq {

class Preprocessor : NonCopyable {
public:
    void import_header(const string& _path);
    string operator()(const string& _path, const string& _extra = "");

private:
    std::unordered_map<string, string> headerMap;
};

}
