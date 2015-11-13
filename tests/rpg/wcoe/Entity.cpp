#include <sqee/app/Resources.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "Cell.hpp"
#include "World.hpp"
#include "Entity.hpp"

using namespace sqt::wcoe;
namespace maths = sq::maths;

//void ModelComponent::refresh() {
//    if ((mesh = sq::static_Mesh().get(PROP_mesh)) == nullptr)
//        mesh = sq::static_Mesh().add(PROP_mesh, PROP_mesh);

//    if ((skin = sq::static_Skin().get(PROP_skin)) == nullptr)
//        skin = sq::static_Skin().add(PROP_skin, PROP_skin);
//}
