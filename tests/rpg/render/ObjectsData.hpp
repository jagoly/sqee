#pragma once

#include <map>

#include <sqee/ecs/Entity.hpp>

#include "static/StaticModel.hpp"
#include "static/StaticDecal.hpp"

#include "entity/ModelSimple.hpp"
#include "entity/ModelSkelly.hpp"
#include "entity/DecalBasic.hpp"
#include "entity/LightOrtho.hpp"
#include "entity/LightPoint.hpp"
#include "entity/LightSpot.hpp"

#include "world/Camera.hpp"
#include "world/SkyBox.hpp"
#include "world/Ambient.hpp"
#include "world/SkyLight.hpp"

namespace sqt { namespace render {

struct EntityData {
    unique_ptr<render::ModelSimpleData> modelSimple;
    unique_ptr<render::ModelSkellyData> modelSkelly;
    unique_ptr<render::DecalBasicData> decalBasic;
    unique_ptr<render::LightOrthoData> lightOrtho;
    unique_ptr<render::LightPointData> lightPoint;
    unique_ptr<render::LightSpotData> lightSpot;
};

struct ObjectsData {
    vector<render::StaticModelData> staticModelVec;
    vector<render::StaticDecalData> staticDecalVec;

    std::map<const sq::Entity*, EntityData> entityMap;

    unique_ptr<render::CameraData> cameraData;
    unique_ptr<render::SkyBoxData> skyboxData;
    unique_ptr<render::AmbientData> ambientData;
    unique_ptr<render::SkyLightData> skylightData;
};

}}
