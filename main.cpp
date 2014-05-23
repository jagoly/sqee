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

    sqe::HandlerClose handler1;
    app.attach_handler(&handler1);

    sqe::HandlerDebug handler2;
    app.attach_handler(&handler2);

    TestScene scene1;
    app.append_scene(&scene1);

    sq::Scene scene2;
    app.append_scene(&scene2);

    sq::Entity ent1;
    app.add_entity("ent1", &ent1);

    sq::Component comp1;
    ent1.add_component("comp1", &comp1);

    app.set_size({800, 600});
    app.run();

    return 0;
}
