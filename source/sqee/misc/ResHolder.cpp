#include <sqee/misc/ResHolder.hpp>

string& sq::res::path() {
    static string path;
    return path;
}
