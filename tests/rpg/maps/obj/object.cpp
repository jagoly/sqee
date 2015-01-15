#include <sqee/misc/files.hpp>

#include "../../resbank.hpp"
#include "object.hpp"
#include "model.hpp"
#include "liquid.hpp"
#include "reflector.hpp"
#include "light.hpp"

using namespace sqt;

void ObjectSpec::parse_line(const vector<string>& _line) {
    const string& key = _line[0];

    if (key == "flags") {
        for (uint i = 1; i < _line.size(); i++)
            flagSet.emplace(_line[i]);
        return;
    }

    char c = key[0];
    if (c == 'i' || c == 'f' || c == 's') {
        int len = std::atoi(key.substr(1, 1).c_str());
        const string& name = _line[1];

        if (c == 'i') {
            std::vector<int>& vec = iMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(std::stoi(_line[i+2]));
        } else
        if (c == 'f') {
            std::vector<float>& vec = fMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(std::stof(_line[i+2]));
        } else
        if (c == 's') {
            std::vector<string>& vec = sMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(_line[i+2]);
        }
        return;
    }
}
