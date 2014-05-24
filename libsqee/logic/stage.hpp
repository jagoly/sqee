#ifndef STAGE_HPP
#define STAGE_HPP

#include <string>
#include <memory>
#include <map>

#include <boost/variant.hpp>

#include <libsqee/logic/entity.hpp>

namespace sq {

class Application;

class Stage {
public:
    Stage();

    Application* application;

    typedef boost::variant<int, float, std::string> State;
    std::map<std::string, State> stateMap;
    std::map<std::string, std::unique_ptr<Entity>> entityMap;

    virtual void update();
};

}

#endif // STAGE_HPP
