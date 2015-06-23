#pragma once
#include <sqee/forward.hpp>

namespace sq {

class PreProcessor : NonCopyable {
public:
    void import_header(const string& _path);
    string load(const string& _path, const string& _extra = "");

private:
    unordered_map<string, string> headerMap;
};

}
