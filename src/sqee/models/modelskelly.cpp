#include "app/logging.hpp"
#include "models/modelskelly.hpp"

using namespace sq;

void ModelSkelly::setup() {
    if (mesh->mCount != skin->mtrlVec.size())
        log_warning("When setting up ModelSkelly, mtrl counts do not match");
}
