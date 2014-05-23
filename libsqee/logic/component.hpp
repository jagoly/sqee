#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <set>
#include <string>

namespace sq {

class Component {
public:
    Component();
    std::string name;

    std::set<std::string> actionStringSet;

    virtual std::string actionInfo();
    bool operator<(const Component&) const;
};

}

#endif // COMPONENT_HPP
