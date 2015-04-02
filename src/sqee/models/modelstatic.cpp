#include "sqee/app/logging.hpp"
#include "sqee/models/modelstatic.hpp"

using namespace sq;

void ModelStatic::setup() {
    if (mesh->mCount != skin->mtrlVec.size())
        log_warning("When setting up ModelStatic, mtrl counts do not match");
}
