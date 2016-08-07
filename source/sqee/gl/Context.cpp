#include <sqee/gl/Context.hpp>

using namespace sq;

Context& Context::get() {
    static Context instance;
    return instance;
}
