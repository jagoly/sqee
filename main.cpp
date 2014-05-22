#include <iostream>

#include <application.hpp>

#include "simplehandlers.cpp"

using namespace std;

int main()
{
    sq::Application app;
    app.attach_handler(sqe::HandlerClose());

    app.run();

    return 0;
}

