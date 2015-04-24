#pragma once
#include <sqee/forward.hpp>

namespace sq {

class Preprocessor : NonCopyable {
public:
    void import_header(const string& _path);
    string operator()(const string& _path, const string& _extra = "");

private:
    unordered_map<string, string> headerMap;
};

}
