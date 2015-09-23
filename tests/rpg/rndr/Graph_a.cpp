#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/objects/ModelSimple.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/PointLight.hpp"
#include "../wcoe/objects/SpotLight.hpp"
#include "../wcoe/objects/Reflector.hpp"
#include "../wcoe/objects/Emitter.hpp"
#include "../wcoe/objects/Liquid.hpp"
#include "../wcoe/objects/Decal.hpp"
#include "Graph.hpp"

#include <sqee/debug/Logging.hpp>

using namespace sqt::rndr;

IrrVolume::IrrVolume() {
    texEnvSHM.reset(new sq::Texture2D());
    texEnvSDV.reset(new sq::Texture2D());
    texEnvSHM->create(gl::RGB, gl::RGB8, 3, false);
    texEnvSDV->create(gl::RGB, gl::RGB8, 3, false);
    texEnvSHM->set_preset(sq::Texture::NearestClamp());
    texEnvSDV->set_preset(sq::Texture::NearestClamp());

    //uint probeNum = 15u * 9u * 10u;
    uint probeNum = 7u * 3u * 3u;
    uint szWidth = std::min(probeNum, 32u);
    uint szHeight = (probeNum + 31u) / 32u;

    texEnvSHM->allocate_storage(uvec2(szWidth*16u, szHeight));
    texEnvSDV->allocate_storage(uvec2(szWidth*26u, szHeight));

    uint index = 0u;
    for (uint z = 0u; z < 3u; ++z) {
        for (uint y = 0u; y < 3u; ++y) {
            for (uint x = 0u; x < 7u; ++x) {
                Probe& probe = tree[index];
                fvec3 pos((x + 0.5f) / 7.f * 32.f - 16.f,
                          (y + 0.5f) / 3.f * 14.f - 7.f,
                          (z + 0.5f) / 3.f * 16.f - 0.f);
                probe.position = pos; index += 1u;

                array<fmat4, 6> viewMatArr = {
                    glm::lookAt(pos, pos+fvec3(0, +1, 0), fvec3(0, 0, +1)),
                    glm::lookAt(pos, pos+fvec3(+1, 0, 0), fvec3(0, 0, +1)),
                    glm::lookAt(pos, pos+fvec3(0, -1, 0), fvec3(0, 0, +1)),
                    glm::lookAt(pos, pos+fvec3(-1, 0, 0), fvec3(0, 0, +1)),
                    glm::lookAt(pos, pos+fvec3(0, 0, +1), fvec3(0, -1, 0)),
                    glm::lookAt(pos, pos+fvec3(0, 0, -1), fvec3(0, +1, 0))
                };

                fmat4 projMat = glm::perspective(glm::radians(90.f), 1.f, 0.02f, 100.f);

                probe.matArr = {
                    projMat * viewMatArr[0], projMat * viewMatArr[1],
                    projMat * viewMatArr[2], projMat * viewMatArr[3],
                    projMat * viewMatArr[4], projMat * viewMatArr[5] };

                array<fmat4, 6> invViewArr = {
                    glm::inverse(viewMatArr[0]), glm::inverse(viewMatArr[1]),
                    glm::inverse(viewMatArr[2]), glm::inverse(viewMatArr[3]),
                    glm::inverse(viewMatArr[3]), glm::inverse(viewMatArr[5]) };

                array<fmat4, 6> trnViewArr = {
                    glm::transpose(viewMatArr[0]), glm::transpose(viewMatArr[1]),
                    glm::transpose(viewMatArr[2]), glm::transpose(viewMatArr[3]),
                    glm::transpose(viewMatArr[3]), glm::transpose(viewMatArr[5]) };

                probe.ubo.reset(new sq::UniformBuffer());
                probe.ubo->reserve("viewMatArr", 96u);
                probe.ubo->reserve("invViewArr", 96u);
                probe.ubo->reserve("trnViewArr", 96u);
                probe.ubo->create();

                probe.ubo->update("viewMatArr", viewMatArr.data());
                probe.ubo->update("invViewArr", invViewArr.data());
                probe.ubo->update("trnViewArr", trnViewArr.data());
            }
        }
    }
}

void Graph::refresh_IrrVolTree() {
    sq::log_info("refreshing irrd tree");

    // Create GL Resources /////

    irrVolume.texDiff.reset(new sq::Texture2DArray());
    irrVolume.texSurf.reset(new sq::Texture2DArray());
    irrVolume.texDpSt.reset(new sq::Texture2DArray());
    irrVolume.texHdr.reset(new sq::Texture2DArray());

    irrVolume.texDiff->create(gl::RGB, gl::RGB8, 3, false);
    irrVolume.texSurf->create(gl::RGB, gl::RGB8, 3, false);
    irrVolume.texDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1, false);
    irrVolume.texHdr->create(gl::RGB, gl::RGB16F, 3, false);

    irrVolume.texDiff->set_preset(sq::Texture::NearestClamp());
    irrVolume.texSurf->set_preset(sq::Texture::NearestClamp());
    irrVolume.texDpSt->set_preset(sq::Texture::NearestClamp());
    irrVolume.texHdr->set_preset(sq::Texture::NearestClamp());

    uint probeNum = 7u * 3u * 3u;
    uint szHeight = 32u * std::min(probeNum, 64u);
    uint szDepth = (probeNum + 63u) / 64u;

    irrVolume.texDiff->allocate_storage(uvec3(192u, szHeight, szDepth));
    irrVolume.texSurf->allocate_storage(uvec3(192u, szHeight, szDepth));
    irrVolume.texDpSt->allocate_storage(uvec3(192u, szHeight, szDepth));
    irrVolume.texHdr->allocate_storage(uvec3(192u, szHeight, szDepth));

    //uchar clr[3] = {127, 127, 127};
    //gl::ClearTexImage(irrVolume.texDiff->tex, 0, gl::RGB, gl::UNSIGNED_BYTE, clr);
    //gl::ClearTexImage(texSurf.tex, 0, gl::RGB, gl::UNSIGNED_BYTE, nullptr);
    //gl::ClearTexImage(texDpSt.tex, 0, gl::DEPTH_STENCIL, gl::UNSIGNED_INT_24_8, nullptr);
    //gl::ClearTexImage(texDefr.tex, 0, gl::RGB, gl::UNSIGNED_BYTE, nullptr);

    sq::FrameBuffer fboA, fboB; pipeline->bind();
    irrVolume.texDiff->bind(gl::TEXTURE3);
    irrVolume.texSurf->bind(gl::TEXTURE4);
    irrVolume.texDpSt->bind(gl::TEXTURE5);

    // Render Models /////

    //gl::StencilMask(0b0010);
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::BLEND_OFF(); sq::STENCIL_OFF();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
    //sq::STENCIL_OFF(); sq::STENCIL_REPLACE();
    //gl::StencilFunc(gl::ALWAYS, 0b0010, 0b0000);

    for (uint layer = 0u; layer < szDepth; ++layer) {
        fboA.revert_to_defaults();
        fboA.attach(gl::DEPTH_STENCIL_ATTACHMENT, *irrVolume.texDpSt, layer);
        fboA.attach(gl::COLOR_ATTACHMENT0, *irrVolume.texDiff, layer);
        fboA.attach(gl::COLOR_ATTACHMENT1, *irrVolume.texSurf, layer);
        fboA.draw_bufs(); sq::CLEAR_COLOR_DEPTH_STENC();

        fboB.revert_to_defaults();
        fboB.attach(gl::DEPTH_STENCIL_ATTACHMENT, *irrVolume.texDpSt, layer);
        fboB.attach(gl::COLOR_ATTACHMENT0, *irrVolume.texHdr, layer);
        fboB.draw_bufs(); sq::CLEAR_COLOR();

        for (uint row = 0u; row < 64u; ++row) {
            uint index = layer * 64u + row;
            if (index == probeNum) break;

            const IrrVolume::Probe& probe = irrVolume.tree[index];
            probe.ubo->bind(2u);

            for (uint face = 0u; face < 6u; ++face) {
                gl::Viewport(face*32, row*32, 32, 32);

                sq::log_only("rendering %d: %d, %d, %d", index, layer, row, face);

                fboA.use();

                pipeline->use_shader(*VS.irrd_gbuf_simple);
                pipeline->use_shader(*FS.irrd_gbuf_model);

                gl::StencilMask(0b0010);
                sq::BLEND_OFF();
                sq::DEPTH_ON(); sq::DEPTH_WRITE();
                sq::CULLFACE_ON(); sq::CULLFACE_BACK();
                sq::STENCIL_ON(); sq::STENCIL_REPLACE();
                gl::StencilFunc(gl::EQUAL, 0b0010, 0b0000);

                for (const auto& rptr : reflectorList) {
                    const wcoe::Reflector& rflct = *rptr.lock();
                    if (rflct.PROP_probes == false) continue;
                    fmat4 matrix = probe.matArr[face] * rflct.matrix;
                    VS.irrd_gbuf_simple->set_mat<fmat4>("matrix", matrix);
                    fmat3 normMat = sq::make_normMat(probe.matArr[face] * rflct.matrix);
                    VS.irrd_gbuf_simple->set_mat<fmat3>("normMat", normMat);
                    sq::FRONTFACE(rflct.negScale); rflct.mesh->bind_vao();
                    for (uint i = 0u; i < rflct.mesh->mtrlCount; i++) {
                        FS.irrd_gbuf_model->set_vec<ivec3>("d_n_s", rflct.skin->mtrlVec[i].glDNS);
                        rflct.skin->bind_textures(i, 1, 0, 0); rflct.mesh->draw_ibo(i);
                    }
                }

                for (const auto& mptr : modelSimpleList) {
                    const wcoe::ModelSimple& model = *mptr.lock();
                    if (model.PROP_probes == false) continue;
                    fmat4 matrix = probe.matArr[face] * model.matrix;
                    VS.irrd_gbuf_simple->set_mat<fmat4>("matrix", matrix);
                    fmat3 normMat = sq::make_normMat(probe.matArr[face] * model.matrix);
                    VS.irrd_gbuf_simple->set_mat<fmat3>("normMat", normMat);
                    sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
                    for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                        FS.irrd_gbuf_model->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS);
                        model.skin->bind_textures(i, 1, 0, 0); model.mesh->draw_ibo(i);
                    }
                }

                for (const auto& mptr : modelSkellyList) {
                    const wcoe::ModelSkelly& model = *mptr.lock();
                    if (model.PROP_probes == false) continue;
                    fmat4 matrix = probe.matArr[face] * model.matrix;
                    VS.irrd_gbuf_simple->set_mat<fmat4>("matrix", matrix);
                    fmat3 normMat = sq::make_normMat(probe.matArr[face] * model.matrix);
                    VS.irrd_gbuf_simple->set_mat<fmat3>("normMat", normMat);
                    sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
                    for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                        FS.irrd_gbuf_model->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS);
                        model.skin->bind_textures(i, 1, 0, 0); model.mesh->draw_ibo(i);
                    }
                }


                fboB.use();


                // Render Skybox /////

                if (world->skybox.PROP_enabled) {
                    pipeline->use_shader(*VS.irrd_defr_skybox);
                    pipeline->use_shader(*FS.irrd_defr_skybox);
                    VS.irrd_defr_skybox->set_sca<uint>("faceInd", face);
                    world->skybox.tex->bind(gl::TEXTURE0);
                    world->skybox.ubo->bind(1u);

                    sq::STENCIL_KEEP();
                    gl::StencilFunc(gl::EQUAL, 0b0000, 0b0010);
                    sq::draw_screen_quad();
                }


                // Render Skylight /////

                if (world->skylight.PROP_enabled) {
                    pipeline->use_shader(*VS.gnrc_screen);
                    pipeline->use_shader(*FS.irrd_defr_skylight);
                    FS.irrd_defr_skylight->set_sca<uint>("faceInd", face);
                    world->skylight.texDepthB->bind(gl::TEXTURE6);
                    world->skylight.ubo->bind(1u);

                    sq::BLEND_ON(); sq::BLEND_ONEONE();
                    gl::StencilFunc(gl::EQUAL, 0b0010, 0b0010);
                    sq::STENCIL_KEEP(); sq::draw_screen_quad();
                }

            }
        }
    }


    sq::FRONTFACE(false);
    sq::DEPTH_OFF(); sq::CULLFACE_OFF();
    irrVolume.texHdr->bind(gl::TEXTURE0);
    pipeline->use_shader(*VS.gnrc_screen);

    // Render Spherical Harmonics /////

    fboA.revert_to_defaults();
    pipeline->use_shader(*FS.irrd_harmonics_shm);
    fboA.attach(gl::COLOR_ATTACHMENT0, *irrVolume.texEnvSHM);
    sq::VIEWPORT(irrVolume.texEnvSHM->get_size());
    fboA.draw_bufs(); sq::CLEAR_COLOR();
    sq::draw_screen_quad();

    // Render Sphere Subdivisions /////

    fboA.revert_to_defaults();
    pipeline->use_shader(*FS.irrd_harmonics_sdv);
    fboA.attach(gl::COLOR_ATTACHMENT0, *irrVolume.texEnvSDV);
    sq::VIEWPORT(irrVolume.texEnvSDV->get_size());
    fboA.draw_bufs(); sq::CLEAR_COLOR();
    sq::draw_screen_quad();
}

void Graph::render_envspheres() {
    pipeline->use_shader(*VS.dbug_envsphere);
    sq::Shader* fragShader = INFO.irrd ? FS.dbug_envsphere_sdv : FS.dbug_envsphere_shm;
    sq::Texture2D* texture = (INFO.irrd ? irrVolume.texEnvSDV : irrVolume.texEnvSHM).get();
    pipeline->use_shader(*fragShader); texture->bind(gl::TEXTURE0);

    sq::BLEND_OFF(); sq::DEPTH_ON();
    sq::DEPTH_WRITE(); sq::CLEAR_DEPTH();
    sq::CULLFACE_BACK(); sq::CULLFACE_ON();

    uint index = 0u;
    for (const IrrVolume::Probe& probe : irrVolume.tree) {
        VS.dbug_envsphere->set_vec<fvec3>("trans", probe.position);
        fragShader->set_sca<uint>("probe", index);
        sq::draw_volume_sphr(); ++index;
    }
}

//////////////////////////

Graph::Graph(const sq::Settings& _settings, sq::Camera* _camera)
    : settings(_settings), camera(_camera) {

    // Particles /////
    gl::GenVertexArrays(1, &partVAO);
    gl::GenBuffers(1, &partVBO);
    gl::GenBuffers(1, &partIBO);

    gl::BindVertexArray(partVAO);
    gl::BindBuffer(gl::ARRAY_BUFFER, partVBO);
    gl::VertexAttribPointer(0, 4, gl::FLOAT, false, 32, (void*)(0));
    gl::VertexAttribPointer(1, 4, gl::FLOAT, false, 32, (void*)(16));
    gl::EnableVertexAttribArray(0); gl::EnableVertexAttribArray(1);
}

void Graph::update() {
    modelSimpleList.remove_if(wptr_expired<wcoe::ModelSimple>);
    modelSkellyList.remove_if(wptr_expired<wcoe::ModelSkelly>);
    pointLightList.remove_if(wptr_expired<wcoe::PointLight>);
    spotLightList.remove_if(wptr_expired<wcoe::SpotLight>);
    reflectorList.remove_if(wptr_expired<wcoe::Reflector>);
    emitterList.remove_if(wptr_expired<wcoe::Emitter>);
    liquidList.remove_if(wptr_expired<wcoe::Liquid>);
    decalList.remove_if(wptr_expired<wcoe::Decal>);

    modelSimpleList.sort([this](ModelSimpleList::value_type& a, ModelSimpleList::value_type& b) {
        return glm::distance(camera->pos, a.lock()->bbox.sphere.origin)
             < glm::distance(camera->pos, b.lock()->bbox.sphere.origin);
    });

    modelSkellyList.sort([this](ModelSkellyList::value_type& a, ModelSkellyList::value_type& b) {
        return glm::distance(camera->pos, a.lock()->sphere.origin)
             < glm::distance(camera->pos, b.lock()->sphere.origin);
    });

    pointLightList.sort([this](PointLightList::value_type& a, PointLightList::value_type& b) {
        return glm::distance(camera->pos, a.lock()->sphere.origin)
             < glm::distance(camera->pos, b.lock()->sphere.origin);
    });

    spotLightList.sort([this](SpotLightList::value_type& a, SpotLightList::value_type& b) {
        return glm::distance(camera->pos, a.lock()->frus.sphere.origin)
             < glm::distance(camera->pos, b.lock()->frus.sphere.origin);
    });

    reflectorList.sort([this](ReflectorList::value_type& a, ReflectorList::value_type& b) {
        return glm::distance(camera->pos, a.lock()->bbox.sphere.origin)
             < glm::distance(camera->pos, b.lock()->bbox.sphere.origin);
    });

    emitterList.sort([this](EmitterList::value_type& a, EmitterList::value_type& b) {
        return glm::distance(camera->pos, a.lock()->position)
             < glm::distance(camera->pos, b.lock()->position);
    });

//    liquidList.sort([this] (LiquidList::value_type& a, LiquidList::value_type& b) {
//       return glm::distance(camera->pos, vec3(a.lock()->bbox.sphere.origin))
//            < glm::distance(camera->pos, vec3(b.lock()->bbox.sphere.origin));
//    });

    decalList.sort([this](DecalList::value_type& a, DecalList::value_type& b) {
        return glm::distance(camera->pos, a.lock()->bbox.sphere.origin)
             < glm::distance(camera->pos, b.lock()->bbox.sphere.origin);
    });
}


void Graph::reload_lists() {
    modelSimpleList.clear();
    modelSkellyList.clear();
    pointLightList.clear();
    spotLightList.clear();
    reflectorList.clear();
    emitterList.clear();
    liquidList.clear();
    decalList.clear();

    for (const auto& model : world->filtered<wcoe::ModelSimple>())
        modelSimpleList.emplace_back(model);
    for (const auto& model : world->filtered<wcoe::ModelSkelly>())
        modelSkellyList.emplace_back(model);
    for (const auto& light : world->filtered<wcoe::PointLight>())
        pointLightList.emplace_back(light);
    for (const auto& light : world->filtered<wcoe::SpotLight>())
        spotLightList.emplace_back(light);
    for (const auto& rflct : world->filtered<wcoe::Reflector>())
        reflectorList.emplace_back(rflct);
    for (const auto& emitr : world->filtered<wcoe::Emitter>())
        emitterList.emplace_back(emitr);
    for (const auto& liqid : world->filtered<wcoe::Liquid>())
        liquidList.emplace_back(liqid);
    for (const auto& decal : world->filtered<wcoe::Decal>())
        decalList.emplace_back(decal);

    update_settings();
}


void Graph::update_settings() {
}
