#pragma once

#include <sqee/app/Application.hpp>

namespace sqt {

class RpgApp final : public sq::Application {
public:
    RpgApp();

    void eval_test_init();

    void update_options();
    bool handle(sf::Event _event);
};

}
