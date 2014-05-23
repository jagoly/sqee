#include <libsqee/logic/component.hpp>

using namespace sq;

Component::Component() {
    name = "Default Component";
    actionStringSet.emplace("Info");
}

bool Component::operator<(const Component& rhs) const {
    if (name < rhs.name) {
        return true;
    }
    return false;
}

std::string Component::actionInfo() {
    return "This is the Default Component, it does nothing.";
}
