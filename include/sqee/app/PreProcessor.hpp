#pragma once
#include <sqee/forward.hpp>

namespace sq {

class PreProcessor : NonCopyable {
public:
    PreProcessor();

    void import_header(const string& _path);
    pair<string, string> load(const string& _path, const string& _extra = "");

    unordered_map<string, string> headerMap;
};

}
