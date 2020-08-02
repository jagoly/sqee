#ifndef SQEE_MSVC

#include "../catch.hpp"
#include "../Common.hpp"

#include <sqee/app/MessageBus.hpp>

//============================================================================//

struct ObjectA
{
    ObjectA(sq::MessageBus& mbus) : intSender(mbus.get_message_source<int>())
    {
        receivers.subscribe(mbus);

        // binding option one: macro
        SQEE_MB_BIND_METHOD(receivers, int, handle_message);
        SQEE_MB_BIND_METHOD(receivers, std::string, handle_message);
    }

    sq::MessageReceiver<int, std::string> receivers;

    sq::MessageSource<int>& intSender;

    void handle_message(const int& message) { intSum += message; }
    void handle_message(const std::string& message) { stringSum += message; }

    int intSum = 0;
    std::string stringSum = "";
};

void foo(std::function<void(const int&)>&&) {}
void foo(std::function<void(const std::string&)>&&) {}

struct ObjectB
{
    ObjectB(sq::MessageBus& mbus) : stringSender(mbus.get_message_source<std::string>())
    {
        receivers.subscribe(mbus);

        // binding option two: lambda
        receivers.assign<int>([this](const auto& msg) { handle_message(msg); });
        receivers.assign<std::string>([this](const auto& msg) { handle_message(msg); });
    }

    sq::MessageReceiver<int, std::string> receivers;

    sq::MessageSource<std::string>& stringSender;

    void handle_message(const int& message) { intSum += message; }
    void handle_message(const std::string& message) { stringSum += message; }

    int intSum = 0;
    std::string stringSum = "";
};

//============================================================================//

TEST_CASE("message bus / receiver tests", "[]")
{
    sq::MessageBus mbus;

    mbus.add_message_source<int>();
    mbus.add_message_source<std::string>();
    mbus.add_message_source<float>();

    ObjectA objectA(mbus);
    ObjectB objectB(mbus);

    REQUIRE(objectA.intSum == 0);
    REQUIRE(objectA.stringSum == "");
    REQUIRE(objectB.intSum == 0);
    REQUIRE(objectB.stringSum == "");

    objectA.intSender.publish(4);
    objectB.stringSender.publish("hello, ");
    objectB.stringSender.publish("world!");

    REQUIRE(objectA.intSum == 4);
    REQUIRE(objectA.stringSum == "hello, world!");
    REQUIRE(objectB.intSum == 4);
    REQUIRE(objectB.stringSum == "hello, world!");
}

#endif // !SQEE_MSVC
