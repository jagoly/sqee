#include <sqee/misc/ResHolder.hpp>

using namespace sq;

string& sq::res::path() {
    static string path;
    return path;
}
