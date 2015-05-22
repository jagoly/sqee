#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include "../cell.hpp"
#include "../world.hpp"
#include "decal.hpp"

using namespace sqt::wcoe;

Decal::Decal(const string& _name, Cell* _cell)
    : Object(ObjType::Decal, _name, _cell) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("matrix", 16);
    ubo->reserve("invMat", 16);
    ubo->reserve("d_n_s", 4);
    ubo->create();
}

void Decal::load_from_spec(const ObjSpec& _spec) {
    DAT_position = glm::make_vec3(_spec.fMap.at("position").data());
    DAT_normal   = glm::make_vec3(_spec.fMap.at("normal").data());
    DAT_tangent  = glm::make_vec3(_spec.fMap.at("tangent").data());
    DAT_scale    = glm::make_vec3(_spec.fMap.at("scale").data());
    if (_spec.sMap.count("diff")) DAT_dPath = _spec.sMap.at("diff")[0];
    if (_spec.sMap.count("norm")) DAT_nPath = _spec.sMap.at("norm")[0];
    if (_spec.sMap.count("spec")) DAT_sPath = _spec.sMap.at("spec")[0];
}

void Decal::refresh() {
    if (DAT_dPath.empty() == false) {
        const string name = "decals/" + DAT_dPath + "_d";
        if (!(texDiff = sq::res::tex2D().get(name))) {
            texDiff = sq::res::tex2D().add(name);
            texDiff->create(gl::RGBA, gl::RGBA8, 4);
            texDiff->set_preset(sq::Texture2D::M_C());
            texDiff->buffer_file(name);
            texDiff->gen_mipmap();
        }
    } else texDiff = nullptr;

    if (DAT_nPath.empty() == false) {
        const string name = "decals/" + DAT_nPath + "_n";
        if (!(texNorm = sq::res::tex2D().get(name))) {
            texNorm = sq::res::tex2D().add(name);
            texNorm->create(gl::RGBA, gl::RGBA8, 4);
            texNorm->set_preset(sq::Texture2D::M_C());
            texNorm->buffer_file(name);
            texNorm->gen_mipmap();
        }
    } else texNorm = nullptr;

    if (DAT_sPath.empty() == false) {
        const string name = "decals/" + DAT_sPath + "_s";
        if (!(texSpec = sq::res::tex2D().get(name))) {
            texSpec = sq::res::tex2D().add(name);
            texSpec->create(gl::RGBA, gl::RGBA8, 4);
            texSpec->set_preset(sq::Texture2D::M_C());
            texSpec->buffer_file(name);
            texSpec->gen_mipmap();
        }
    } else texSpec = nullptr;

    vec3 position = DAT_position + cell->DAT_position;
    vec3 normal = glm::normalize(DAT_normal);
    vec3 tangent = glm::normalize(DAT_tangent);
    vec3 binormal = glm::normalize(cross(normal, tangent));
    matrix = glm::translate(mat4(), position);
    matrix *= mat4(mat3(binormal, tangent, normal));
    matrix = glm::scale(matrix, DAT_scale);
    invMat = glm::inverse(matrix);
    bbox = sq::make_BoundBox(matrix, {0,0,0}, {1,1,1}, 0.87f);
    ivec3 dns = {bool(texDiff), bool(texNorm), bool(texSpec)};

    ubo->bind(1);
    ubo->update("matrix", &matrix);
    ubo->update("invMat", &invMat);
    ubo->update("d_n_s", &dns);
}

void Decal::tick() {}

void Decal::calc(double _accum) {}
