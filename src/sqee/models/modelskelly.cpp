#include "sqee/app/logging.hpp"
#include "sqee/models/modelskelly.hpp"

using namespace sq;

void ModelSkelly::setup() {
    if (mesh->mCount != skin->mtrlVec.size())
        log_warning("When setting up ModelSkelly, mtrl counts do not match");
}
