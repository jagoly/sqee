#ifndef SQEE_MSVC

#include "../catch.hpp"
#include "../Common.hpp"

#include <sqee/app/MessageBus.hpp>

//============================================================================//

using std::literals::operator""s;

struct Object : public sq::Receiver<int, string>
{
    using Receiver::Receiver;

    void handle_message(int message) override { intSum += message; }
    void handle_message(string message) override { stringSum += message; }

    int intSum = 0;
    string stringSum = "";
};

//============================================================================//

TEST_CASE("message bus / receiver tests", "[]")
{
    sq::MessageBus mbus;
    Object object(mbus);

    mbus.register_type<float>();
    mbus.register_type<int>();
    mbus.register_type<bool>();
    mbus.register_type<string>();
    mbus.register_type<uchar>();

    mbus.subscribe_all(object);

    mbus.publish(5);
    mbus.publish("hello, "s);
    mbus.publish("world!"s);
    mbus.publish(8);

    REQUIRE(object.intSum == 13);
    REQUIRE(object.stringSum == "hello, world!");
}

#endif // !SQEE_MSVC
