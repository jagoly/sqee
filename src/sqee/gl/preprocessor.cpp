#include "sqee/app/logging.hpp"
#include "sqee/misc/files.hpp"
#include "sqee/gl/preprocessor.hpp"

using namespace sq;

void PreProcessor::import_header(const string& _path) {
   #ifdef SQEE_DEBUG
    log_info("Importing shader from %s", _path);
   #endif
    headerMap.emplace(_path, get_string_from_file("shaders/"+_path+".glsl"));
}

string PreProcessor::load(const string& _path, const string& _extra) {
   #ifdef SQEE_DEBUG
    log_info("Preprocessing shader from %s", _path);
   #endif

    string retStr = get_string_from_file("shaders/"+_path+".glsl");

    auto lineStart = retStr.find("#version");
    lineStart = retStr.find('\n', lineStart)+1;

    retStr.insert(lineStart, _extra+'\n');
    lineStart += _extra.size()+1;

    while (true) {
        lineStart = retStr.find("#include");
        if (lineStart == string::npos) break;

        auto q1 = retStr.find('\"', lineStart) + 1;
        auto q2 = retStr.find('\"', q1) - q1;
        string incName = retStr.substr(q1, q2);

        retStr.erase(lineStart, q1-lineStart+q2+1);
        retStr.insert(lineStart, headerMap.at(incName));
    }

    return retStr;
}
