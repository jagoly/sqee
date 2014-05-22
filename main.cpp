#include <iostream>

#include <libsqee/libsqee.hpp>
#include <libsqee/extra.hpp>

using namespace std;

class TestScene : public sq::Scene {
public:
    void render() {
        renderTex.clear(sf::Color::Blue);
        renderTex.display();
    }
};

int main()
{
    sq::Application app;

    sqe::HandlerClose handler;
    app.attach_handler(&handler);

    TestScene scene1;
    app.append_scene(&scene1);

    sq::Scene scene2;
    app.append_scene(&scene2);

    app.set_size({800, 600});
    app.run();

    return 0;
}

