#ifndef STAGE_HPP
#define STAGE_HPP

#include <string>
#include <memory>
#include <map>

#include <libsqee/logic/entity.hpp>

namespace sq {

class Application;

class Stage {
public:
    Stage();

    Application* application;

    double accum = 0.f;
    int tickRate;
    double dt;

    std::map<std::string, std::unique_ptr<Entity>> entityMap;

    virtual void update();
};

}

#endif // STAGE_HPP
