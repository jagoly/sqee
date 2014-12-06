#include "../../resbank.hpp"
#include "liquid.hpp"

using namespace sqt::wld;

Liquid::Liquid(const ObjectSpec& _spec) : Object(_spec) {
    ubo.reserve("reflMat", 16);
    ubo.reserve("wSmooth", 1);
    ubo.reserve("thickness", 1);
    ubo.reserve("flowOffset", 2);
    ubo.reserve("translation", 3);
    ubo.reserve("normProg", 1);
    ubo.reserve("colour", 3);
    ubo.reserve("normA", 1);
    ubo.reserve("normB", 1);

    ubo.create();

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);

    float points[18];
    float txcrds[12];

    glm::vec3 pos = {0.f, 0.f, 0.f};
    if (_spec.fMap.count("pos"))
        pos = glm::make_vec3(_spec.fMap.at("pos").data());

    glm::vec2 size = {0.f, 0.f};
    if (_spec.fMap.count("size"))
        size = glm::make_vec2(_spec.fMap.at("size").data());

    if (_spec.fMap.count("smooth"))
        smooth = _spec.fMap.at("smooth")[0];

    if (_spec.fMap.count("scale"))
        scale = _spec.fMap.at("scale")[0];

    if (_spec.fMap.count("flow"))
        flow = glm::make_vec2(_spec.fMap.at("flow").data());

    if (_spec.fMap.count("colour"))
        colour = glm::make_vec3(_spec.fMap.at("colour").data());

    if (_spec.fMap.count("thickness"))
        thickness = _spec.fMap.at("thickness")[0];

    string texDir = _spec.sMap.at("texDir")[0];


    float tX = size.x / scale;
    float tY = size.y / scale;

    points[5*3+0] = points[1*3+0] = size.x;
    txcrds[5*2+0] = txcrds[1*2+0] = tX;
    points[5*3+1] = points[1*3+1] = size.y;
    txcrds[5*2+1] = txcrds[1*2+1] = tY;

    points[4*3+0] = points[2*3+0] = 0.f;
    txcrds[4*2+0] = txcrds[2*2+0] = 0.f;
    points[4*3+1] = points[2*3+1] = 0.f;
    txcrds[4*2+1] = txcrds[2*2+1] = 0.f;

    points[3*3+0] = 0.f;
    txcrds[3*2+0] = 0.f;
    points[3*3+1] = size.y;
    txcrds[3*2+1] = tY;

    points[0*3+0] = size.x;
    txcrds[0*2+0] = tX;
    points[0*3+1] = 0.f;
    txcrds[0*2+1] = 0.f;

    points[2] = points[5] = points[8] = points[11] = points[14] = points[17] = 0.f;

    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, 18 * sizeof(float), points, gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);

    gl::GenBuffers(1, &vboTc);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTc);
    gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(float), txcrds, gl::STATIC_DRAW);
    gl::VertexAttribPointer(1, 2, gl::FLOAT, false, 0, nullptr);

    glm::mat4 reflMat = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 2.f*pos.z, 1
    };

    if (!(normArray = resBank().texH.get("waternorms_"+texDir))) {
        normArray = resBank().texH.add("waternorms_"+texDir);
        normArray->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::Texture::Preset::L_R);
        normArray->resize({1024, 1024, 4});
        normArray->buffer_file("static/water/"+texDir+"/0", 0);
        normArray->buffer_file("static/water/"+texDir+"/1", 1);
        normArray->buffer_file("static/water/"+texDir+"/2", 2);
        normArray->buffer_file("static/water/"+texDir+"/3", 3);
    }

    update_ubo();
    ubo.update("reflMat", &reflMat);
    ubo.update("translation", &pos);
}

void Liquid::update_ubo() {
    ubo.bind(2);
    ubo.update("wSmooth", &smooth);
    ubo.update("thickness", &thickness);
    ubo.update("colour", &colour);
}

void Liquid::bind_ubo() {
    ubo.bind(2);
    ubo.update("flowOffset", &flowOffs);
    ubo.update("normA", &normInfo.x);
    ubo.update("normB", &normInfo.y);
    ubo.update("normProg", &normInfo.z);
}

void Liquid::draw() {
    normArray->bind(gl::TEXTURE3);
    gl::BindVertexArray(vao);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
}

void Liquid::tick() {
    flowOffsA = flowOffsB;
    flowOffsB.x += flow.x / scale;
    flowOffsB.y += flow.y / scale;

    static int ticker = 191;
    if (++ticker == 192) ticker = 0;
    int ind = ticker / 48;
    if      (ind == 0) { normInfo.x = 0; normInfo.y = 1; }
    else if (ind == 1) { normInfo.x = 1; normInfo.y = 2; }
    else if (ind == 2) { normInfo.x = 2; normInfo.y = 3; }
    else if (ind == 3) { normInfo.x = 3; normInfo.y = 0; }

    normOffsA = normOffsB;
    uchar rem = ticker % 48;
    if (!rem) { normOffsA = 0.f; normOffsB = 1.f / 48.f; }
    else normOffsB += 1.f / 48.f;
}

void Liquid::calc(double _accum) {
    const double dt = 1.d / 24.d;
    flowOffs = glm::mix(flowOffsA, flowOffsB, _accum / dt);
    normInfo.z = glm::mix(normOffsA, normOffsB, _accum / dt);

}
