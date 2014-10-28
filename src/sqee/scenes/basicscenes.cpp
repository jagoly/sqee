#include <text/font.hpp>
#include <text/text.hpp>
#include <scenes/basicscenes.hpp>

using namespace sq;

void SceneFPS::render(float _ft) {
    static sq::Font::Ptr font = sq::create_font(SQ_FPSFONT, app.ftLib, 48);
    static sq::TextHandles textHandles;

    static float fps = 60.f;
    fps = fps * 0.9f + 1.f / _ft * 0.1f;
    glm::uvec2 size = app.get_size();
    sq::draw_text(textHandles, font, "FPS: "+std::to_string(int(fps)), {16, size.y - 32}, size);
}
