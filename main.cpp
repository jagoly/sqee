#include <iostream>

#include <libsqee/tests/soko.hpp>

int main()
{
    sqt::TestApp app;

    app.set_size({800, 600});

    app.run();

    return 0;
}
