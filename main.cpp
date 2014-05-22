#include <iostream>

#include <libsqee/libsqee.hpp>
#include <simplehandlers.hpp>

using namespace std;

int main()
{
    sq::Application app;

    sqe::HandlerClose handler;
    app.attach_handler(&handler);

    sf::Event ev;
    handler.handle(ev);

    app.run();

    return 0;
}

