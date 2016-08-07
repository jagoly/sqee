#include <sqee/app/Resources.hpp>

namespace sq {

string& static_path() { static string path; return path; }

}
