// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/app/GuiSystem.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/Window.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/gl/Program.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/misc/Algorithms.hpp>

#include <sqee/redist/gl_loader.hpp>
#include <sqee/redist/imgui/imgui.hpp>

#include <sqee/macros.hpp>

using namespace sq;

//============================================================================//

extern const int32_t data_UbuntuRegular_size;
extern const uint64_t data_UbuntuRegular[];

extern const int32_t data_UbuntuBold_size;
extern const uint64_t data_UbuntuBold[];

extern const int32_t data_UbuntuItalic_size;
extern const uint64_t data_UbuntuItalic[];

extern const int32_t data_UbuntuMonoRegular_size;
extern const uint64_t data_UbuntuMonoRegular[];

//============================================================================//

namespace { // anonymous

//============================================================================//

const char vertexSource[] = R"glsl(

#version 330 core
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_gpu_shader5 : enable

layout(location=0) in vec2 v_Position;
layout(location=1) in vec2 v_TexCoord;
layout(location=2) in vec4 v_Colour;

layout(location=0) uniform mat4 u_Matrix;

out vec2 texcrd;
out vec4 colour;

void main()
{
    texcrd = v_TexCoord;
    colour = v_Colour;

    gl_Position = u_Matrix * vec4(v_Position, 0.f, 1.f);
}

)glsl";

//============================================================================//

const char fragmentSource[] = R"glsl(

#version 330 core
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_gpu_shader5 : enable

layout(binding=0) uniform sampler2D tex_Atlas;

in vec2 texcrd;
in vec4 colour;

out vec4 frag_Colour;

void main()
{
    frag_Colour = colour * texture(tex_Atlas, texcrd);
}

)glsl";

//============================================================================//

} // anonymous namespace

//============================================================================//

class GuiSystem::Implementation final
{
public: //====================================================//

    Implementation(Window& window, InputDevices& inputDevices);

    ~Implementation() = default;

    //--------------------------------------------------------//

    void create_fonts();

    void create_gl_objects();

    //--------------------------------------------------------//

    bool handle_event(Event event);

    void finish_handle_events();

    void finish_scene_update(double elapsed);

    void render_gui();

private: //===================================================//

    Window& window;
    InputDevices& input;

    ImGuiIO& io;
    Context& context;

    //--------------------------------------------------------//

    sq::Texture2D mTexture;
    sq::Program mProgram;

    sq::VertexArray mVAO;
    sq::FixedBuffer mVBO, mIBO;
};

//============================================================================//

GuiSystem::Implementation::Implementation(Window& window, InputDevices& inputDevices)
    : window(window), input(inputDevices)
    , io(ImGui::GetIO()), context(Context::get())
{
    io.KeyMap[ImGuiKey_Tab]         = int(Keyboard_Key::Tab);
    io.KeyMap[ImGuiKey_LeftArrow]   = int(Keyboard_Key::Arrow_Left);
    io.KeyMap[ImGuiKey_RightArrow]  = int(Keyboard_Key::Arrow_Right);
    io.KeyMap[ImGuiKey_UpArrow]     = int(Keyboard_Key::Arrow_Up);
    io.KeyMap[ImGuiKey_DownArrow]   = int(Keyboard_Key::Arrow_Down);
    io.KeyMap[ImGuiKey_PageUp]      = int(Keyboard_Key::PageUp);
    io.KeyMap[ImGuiKey_PageDown]    = int(Keyboard_Key::PageDown);
    io.KeyMap[ImGuiKey_Home]        = int(Keyboard_Key::Home);
    io.KeyMap[ImGuiKey_End]         = int(Keyboard_Key::End);
    io.KeyMap[ImGuiKey_Delete]      = int(Keyboard_Key::Delete);
    io.KeyMap[ImGuiKey_Backspace]   = int(Keyboard_Key::BackSpace);
    io.KeyMap[ImGuiKey_Enter]       = int(Keyboard_Key::Return);
    io.KeyMap[ImGuiKey_Escape]      = int(Keyboard_Key::Escape);
    io.KeyMap[ImGuiKey_A]           = int(Keyboard_Key::A);
    io.KeyMap[ImGuiKey_C]           = int(Keyboard_Key::C);
    io.KeyMap[ImGuiKey_V]           = int(Keyboard_Key::V);
    io.KeyMap[ImGuiKey_X]           = int(Keyboard_Key::X);
    io.KeyMap[ImGuiKey_Y]           = int(Keyboard_Key::Y);
    io.KeyMap[ImGuiKey_Z]           = int(Keyboard_Key::Z);

    auto& style = imgui::GetStyle();

    style.Alpha              = 1.0f;         // default: 1.0
    style.WindowPadding      = { 6, 6 };     // default: 8, 8
    style.WindowRounding     = 8;            // default: 7
    style.WindowBorderSize   = 0;            // default: 0
    style.WindowMinSize      = { 32, 32 };   // default: 32, 32
    style.WindowTitleAlign   = { 0.0, 0.5 }; // default: 0.0, 0.5
    style.ChildRounding      = 4;            // default: 0
    style.ChildBorderSize    = 0;            // default: 0
    style.PopupRounding      = 0;            // default: 0
    style.PopupBorderSize    = 1;            // default: 1
    style.FramePadding       = { 6, 6 };     // default: 4, 3
    style.FrameRounding      = 4;            // default: 0
    style.FrameBorderSize    = 0;            // default: 0
    style.ItemSpacing        = { 6, 6 };     // default: 8, 4
    style.ItemInnerSpacing   = { 6, 6 };     // default: 4, 4
    style.TouchExtraPadding  = { 0, 0 };     // default: 0, 0
    style.IndentSpacing      = 20;           // default: 21
    style.ColumnsMinSpacing  = 6;            // default: 6
    style.ScrollbarSize      = 16;           // default: 16
    style.ScrollbarRounding  = 8;            // default: 9
    style.GrabMinSize        = 8;            // default: 10
    style.GrabRounding       = 4;            // default: 0
    style.ButtonTextAlign    = { 0.5, 0.5 }; // default: 0.5, 0.5
    style.AntiAliasedLines   = true;         // default: true
    style.AntiAliasedFill    = true;         // default: true

    style.Colors[ImGuiCol_Text]                  = { 1.00f, 1.00f, 1.00f, 1.00f }; // 1.00f, 1.00f, 1.00f, 1.00f
    style.Colors[ImGuiCol_TextDisabled]          = { 0.50f, 0.50f, 0.50f, 1.00f }; // 0.50f, 0.50f, 0.50f, 1.00f
    style.Colors[ImGuiCol_WindowBg]              = { 0.10f, 0.10f, 0.10f, 0.75f }; // 0.06f, 0.06f, 0.06f, 0.94f
    style.Colors[ImGuiCol_ChildBg]               = { 0.00f, 0.00f, 0.00f, 0.00f }; // 1.00f, 1.00f, 1.00f, 0.00f
    style.Colors[ImGuiCol_PopupBg]               = { 0.15f, 0.15f, 0.15f, 0.75f }; // 0.08f, 0.08f, 0.08f, 0.94f
    style.Colors[ImGuiCol_Border]                = { 0.80f, 0.80f, 0.80f, 0.50f }; // 0.43f, 0.43f, 0.50f, 0.50f
    style.Colors[ImGuiCol_BorderShadow]          = { 0.00f, 0.00f, 0.00f, 0.00f }; // 0.00f, 0.00f, 0.00f, 0.00f
    style.Colors[ImGuiCol_FrameBg]               = { 0.70f, 0.50f, 0.40f, 0.40f }; // 0.16f, 0.29f, 0.48f, 0.54f
    style.Colors[ImGuiCol_FrameBgHovered]        = { 0.80f, 0.60f, 0.50f, 0.40f }; // 0.26f, 0.59f, 0.98f, 0.40f
    style.Colors[ImGuiCol_FrameBgActive]         = { 0.60f, 0.40f, 0.30f, 0.40f }; // 0.26f, 0.59f, 0.98f, 0.67f
    style.Colors[ImGuiCol_TitleBg]               = { 0.30f, 0.30f, 0.30f, 0.60f }; // 0.04f, 0.04f, 0.04f, 1.00f
    style.Colors[ImGuiCol_TitleBgActive]         = { 0.25f, 0.25f, 0.25f, 0.80f }; // 0.16f, 0.29f, 0.48f, 1.00f
    style.Colors[ImGuiCol_TitleBgCollapsed]      = { 0.35f, 0.35f, 0.35f, 0.40f }; // 0.00f, 0.00f, 0.00f, 0.51f
    style.Colors[ImGuiCol_MenuBarBg]             = { 0.40f, 0.40f, 0.40f, 0.40f }; // 0.14f, 0.14f, 0.14f, 1.00f
    style.Colors[ImGuiCol_ScrollbarBg]           = { 0.40f, 0.40f, 0.40f, 0.40f }; // 0.02f, 0.02f, 0.02f, 0.53f
    style.Colors[ImGuiCol_ScrollbarGrab]         = { 0.60f, 0.60f, 0.60f, 0.40f }; // 0.31f, 0.31f, 0.31f, 1.00f
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = { 0.70f, 0.70f, 0.70f, 0.40f }; // 0.41f, 0.41f, 0.41f, 1.00f
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = { 0.50f, 0.50f, 0.50f, 0.40f }; // 0.51f, 0.51f, 0.51f, 1.00f
    style.Colors[ImGuiCol_CheckMark]             = { 0.90f, 0.90f, 0.90f, 0.50f }; // 0.26f, 0.59f, 0.98f, 1.00f
    style.Colors[ImGuiCol_SliderGrab]            = { 1.00f, 1.00f, 1.00f, 0.30f }; // 0.24f, 0.52f, 0.88f, 1.00f
    style.Colors[ImGuiCol_SliderGrabActive]      = { 0.80f, 0.50f, 0.50f, 1.00f }; // 0.26f, 0.59f, 0.98f, 1.00f
    style.Colors[ImGuiCol_Button]                = { 0.80f, 0.50f, 0.30f, 0.60f }; // 0.26f, 0.59f, 0.98f, 0.40f
    style.Colors[ImGuiCol_ButtonHovered]         = { 0.90f, 0.60f, 0.40f, 0.60f }; // 0.26f, 0.59f, 0.98f, 1.00f
    style.Colors[ImGuiCol_ButtonActive]          = { 0.70f, 0.40f, 0.20f, 0.60f }; // 0.06f, 0.53f, 0.98f, 1.00f
    style.Colors[ImGuiCol_Header]                = { 0.90f, 0.50f, 0.20f, 0.60f }; // 0.26f, 0.59f, 0.98f, 0.31f
    style.Colors[ImGuiCol_HeaderHovered]         = { 1.00f, 0.60f, 0.30f, 0.60f }; // 0.26f, 0.59f, 0.98f, 0.80f
    style.Colors[ImGuiCol_HeaderActive]          = { 0.80f, 0.40f, 0.10f, 0.60f }; // 0.26f, 0.59f, 0.98f, 1.00f
    style.Colors[ImGuiCol_Separator]             = { 0.80f, 0.80f, 0.80f, 0.50f }; // 0.43f, 0.43f, 0.50f, 0.50f
    style.Colors[ImGuiCol_SeparatorHovered]      = { 0.90f, 0.90f, 0.90f, 0.50f }; // 0.10f, 0.40f, 0.75f, 0.78f
    style.Colors[ImGuiCol_SeparatorActive]       = { 0.70f, 0.70f, 0.70f, 0.50f }; // 0.10f, 0.40f, 0.75f, 1.00f
    style.Colors[ImGuiCol_ResizeGrip]            = { 0.80f, 0.80f, 0.80f, 0.30f }; // 0.26f, 0.59f, 0.98f, 0.25f
    style.Colors[ImGuiCol_ResizeGripHovered]     = { 0.80f, 0.80f, 0.80f, 0.40f }; // 0.26f, 0.59f, 0.98f, 0.67f
    style.Colors[ImGuiCol_ResizeGripActive]      = { 0.80f, 0.80f, 0.80f, 0.45f }; // 0.26f, 0.59f, 0.98f, 0.95f
    style.Colors[ImGuiCol_PlotLines]             = { 0.61f, 0.61f, 0.61f, 1.00f }; // 0.61f, 0.61f, 0.61f, 1.00f
    style.Colors[ImGuiCol_PlotLinesHovered]      = { 1.00f, 0.43f, 0.35f, 1.00f }; // 1.00f, 0.43f, 0.35f, 1.00f
    style.Colors[ImGuiCol_PlotHistogram]         = { 0.90f, 0.70f, 0.00f, 1.00f }; // 0.90f, 0.70f, 0.00f, 1.00f
    style.Colors[ImGuiCol_PlotHistogramHovered]  = { 1.00f, 0.60f, 0.00f, 1.00f }; // 1.00f, 0.60f, 0.00f, 1.00f
    style.Colors[ImGuiCol_TextSelectedBg]        = { 0.30f, 0.50f, 0.70f, 0.40f }; // 0.26f, 0.59f, 0.98f, 0.35f
    style.Colors[ImGuiCol_DragDropTarget]        = { 1.00f, 1.00f, 0.00f, 0.90f }; // 1.00f, 1.00f, 0.00f, 0.90f
    style.Colors[ImGuiCol_NavHighlight]          = { 0.26f, 0.59f, 0.98f, 1.00f }; // 0.26f, 0.59f, 0.98f, 1.00f
    style.Colors[ImGuiCol_NavWindowingHighlight] = { 1.00f, 1.00f, 1.00f, 0.70f }; // 1.00f, 1.00f, 1.00f, 0.70f
    style.Colors[ImGuiCol_NavWindowingDimBg]     = { 0.80f, 0.80f, 0.80f, 0.20f }; // 0.80f, 0.80f, 0.80f, 0.20f
    style.Colors[ImGuiCol_ModalWindowDimBg]      = { 0.80f, 0.80f, 0.80f, 0.35f }; // 0.80f, 0.80f, 0.80f, 0.35f

    //--------------------------------------------------------//

    create_fonts();

    create_gl_objects();
}

//============================================================================//

void GuiSystem::Implementation::create_fonts()
{
    ImFontConfig fontConfig;

    fontConfig.FontDataOwnedByAtlas = false;

    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;

    constexpr const size_t MaxNameLen = sizeof(ImFontConfig::Name);

    fontConfig.Name[StringView("Ubuntu Regular").copy(fontConfig.Name, MaxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(data_UbuntuRegular, data_UbuntuRegular_size, 16.f, &fontConfig);

    fontConfig.Name[StringView("Ubuntu Bold").copy(fontConfig.Name, MaxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(data_UbuntuBold, data_UbuntuBold_size, 16.f, &fontConfig);

    fontConfig.Name[StringView("Ubuntu Italic").copy(fontConfig.Name, MaxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(data_UbuntuItalic, data_UbuntuItalic_size, 16.f, &fontConfig);

    fontConfig.Name[StringView("Ubuntu Mono Regular").copy(fontConfig.Name, MaxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(data_UbuntuMonoRegular, data_UbuntuMonoRegular_size, 16.f, &fontConfig);
}

//============================================================================//

void GuiSystem::Implementation::create_gl_objects()
{
    mProgram.load_vertex(vertexSource);
    mProgram.load_fragment(fragmentSource);
    mProgram.link_program_stages();

    mVBO.allocate_dynamic(65536u * 20u, nullptr);
    mIBO.allocate_dynamic(65536u * 2u, nullptr);

    mVAO.set_vertex_buffer(mVBO, 20u);
    mVAO.set_index_buffer(mIBO);

    mVAO.add_float_attribute(0u, 2u, gl::FLOAT, false, 0u);
    mVAO.add_float_attribute(1u, 2u, gl::FLOAT, false, 8u);
    mVAO.add_float_attribute(2u, 4u, gl::UNSIGNED_BYTE, true, 16u);

    uchar* pixels; int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    mTexture.set_format(sq::Texture::Format::R8_UN);
    mTexture.set_swizzle_mode('1', '1', '1', 'R');
    mTexture.set_filter_mode(true);
    mTexture.allocate_storage({uint(width), uint(height)});
    mTexture.load_memory(pixels);

    io.Fonts->TexID = static_cast<void*>(&mTexture);
}

//============================================================================//

bool GuiSystem::Implementation::handle_event(Event event)
{
    SWITCH ( event.type ) {

        CASE ( Mouse_Scroll )
        {
            io.MouseWheel += event.data.scroll.delta;

            return io.WantCaptureMouse;
        }

        CASE ( Mouse_Press )
        {
            const Mouse_Button button = event.data.mouse.button;

            io.MouseDown[0] |= (button == Mouse_Button::Left);
            io.MouseDown[1] |= (button == Mouse_Button::Right);
            io.MouseDown[2] |= (button == Mouse_Button::Middle);

            return io.WantCaptureMouse;
        }

        CASE ( Text_Entry )
        {
            const uint32_t code = event.data.text.unicode;

            if (code < 128u && std::isprint(char(code)))
                io.AddInputCharacter(static_cast<ImWchar>(code));

            return io.WantTextInput;
        }

        CASE ( Keyboard_Press, Keyboard_Release )
        {
            const decltype(Event::Data::keyboard)& data = event.data.keyboard;

            io.KeysDown[int(data.key)] = (event.type == Event::Type::Keyboard_Press);

            io.KeyShift = data.shift; io.KeyCtrl = data.ctrl;
            io.KeyAlt = data.alt; io.KeySuper = data.super;

            return io.WantCaptureKeyboard;
        }

        CASE_DEFAULT { return false; }

    } SWITCH_END;

    //--------------------------------------------------------//

    return false;
}

//============================================================================//

void GuiSystem::Implementation::finish_handle_events()
{
    io.DisplaySize = Vec2F(window.get_window_size());
    io.MousePos = Vec2F(input.get_cursor_location());

    io.MouseDown[0] |= input.is_pressed(Mouse_Button::Left);
    io.MouseDown[1] |= input.is_pressed(Mouse_Button::Right);
    io.MouseDown[2] |= input.is_pressed(Mouse_Button::Middle);

    io.KeyShift |= input.is_pressed(Keyboard_Key::Shift_L);
    io.KeyShift |= input.is_pressed(Keyboard_Key::Shift_R);

    io.KeyCtrl |= input.is_pressed(Keyboard_Key::Control_L);
    io.KeyCtrl |= input.is_pressed(Keyboard_Key::Control_R);

    io.KeyAlt |= input.is_pressed(Keyboard_Key::Alt_L);
    io.KeyAlt |= input.is_pressed(Keyboard_Key::Alt_R);

    io.KeySuper |= input.is_pressed(Keyboard_Key::Super_L);
    io.KeySuper |= input.is_pressed(Keyboard_Key::Super_R);

    //--------------------------------------------------------//

    imgui::NewFrame();
}

//============================================================================//

void GuiSystem::Implementation::finish_scene_update(double elapsed)
{
    imgui::Render();

    //--------------------------------------------------------//

    io.DeltaTime = float(elapsed);

    io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = false;

    io.MouseWheel = 0.f;

    io.KeyShift = io.KeyCtrl = io.KeyAlt = io.KeySuper = false;
}

//============================================================================//

void GuiSystem::Implementation::render_gui()
{
    SQASSERT(imgui::GetDrawData(), "imgui NewFrame() -> Render() sequence probably wrong");

    //--------------------------------------------------------//

    const ImDrawData& drawData = *imgui::GetDrawData();

    const Vec2U vp = context.get_ViewPort();

    //--------------------------------------------------------//

    context.bind_FrameBuffer_default();

    context.set_state(Context::Scissor_Test::Enable);

    context.set_state(Context::Blend_Mode::Alpha);
    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);

    const Mat4F orthoMatrix
    {
        { +2.0f / vp.x, 0.0f, 0.0f, 0.0f },
        { 0.0f, -2.0f / vp.y, 0.0f, 0.0f },
        { 0.0f, 0.0f, -1.0f, 0.0f },
        { -1.0f, +1.0f, 0.0f, +1.0f },
    };

    mProgram.update(0, orthoMatrix);

    context.bind_Program(mProgram);
    context.bind_Texture(mTexture, 0u);
    context.bind_VertexArray(mVAO);

    //--------------------------------------------------------//

    for (int n = 0; n < drawData.CmdListsCount; ++n)
    {
        const ImDrawList& cmdList = *drawData.CmdLists[n];
        const ImDrawIdx* indexBufferOffset = 0;

        mVBO.update(0u, uint(cmdList.VtxBuffer.Size) * sizeof(ImDrawVert), cmdList.VtxBuffer.Data);
        mIBO.update(0u, uint(cmdList.IdxBuffer.Size) * sizeof(ImDrawIdx), cmdList.IdxBuffer.Data);

        for (const ImDrawCmd& cmd : cmdList.CmdBuffer)
        {
            if (cmd.UserCallback == nullptr)
            {
                const uint clipX = uint(cmd.ClipRect.x);
                const uint clipY = uint(vp.y - cmd.ClipRect.w);
                const uint clipW = uint(cmd.ClipRect.z - cmd.ClipRect.x);
                const uint clipH = uint(cmd.ClipRect.w - cmd.ClipRect.y);

                context.set_Scissor_Params({clipX, clipY, clipW, clipH});

                gl::DrawElements(gl::TRIANGLES, int(cmd.ElemCount), gl::UNSIGNED_SHORT, indexBufferOffset);
            }
            else cmd.UserCallback(&cmdList, &cmd);

            std::advance(indexBufferOffset, cmd.ElemCount);
        }
    }

    context.set_state(Context::Scissor_Test::Disable);
}

//============================================================================//

GuiSystem*& GuiSystem::impl_get_ptr_ref()
{
    static GuiSystem* instance = nullptr;
    return instance;
}

void GuiSystem::construct(Window& window, InputDevices& inputDevices)
{
    auto& instance = GuiSystem::impl_get_ptr_ref();

    SQASSERT(instance == nullptr, "GuiSystem::construct() already called");

    imgui::CreateContext();

    instance = new GuiSystem();

    instance->impl = std::make_unique<Implementation>(window, inputDevices);
}

void GuiSystem::destruct()
{
    auto& instance = GuiSystem::impl_get_ptr_ref();

    SQASSERT(instance != nullptr, "GuiSystem::destruct() already called");

    delete instance;

    imgui::DestroyContext(); // todo: imgui probably can't be re-started after this
}

//============================================================================//

bool GuiSystem::handle_event(Event event) { return impl->handle_event(event); }

void GuiSystem::finish_handle_events() { impl->finish_handle_events(); }

void GuiSystem::finish_scene_update(double elapsed) { impl->finish_scene_update(elapsed); }

void GuiSystem::render_gui() { impl->render_gui(); }

void GuiSystem::show_imgui_demo() { imgui::ShowDemoWindow(); }

//============================================================================//

void GuiSystem::enable_widget(GuiWidget& widget)
{
    const auto iter = algo::find(mWidgets, &widget);
    if (iter == mWidgets.end()) mWidgets.push_front(&widget);
    else SQASSERT(false, "");
}

void GuiSystem::disable_widget(GuiWidget& widget)
{
    const auto iter = algo::find(mWidgets, &widget);

    if (iter != mWidgets.end())
    {
        // in case we erase current widget
        if (iter == mWidgetIter) --mWidgetIter;
        mWidgets.erase(iter);
    }

    else SQASSERT(false, "");
}

//============================================================================//

void GuiSystem::draw_widgets()
{
    for (mWidgetIter = mWidgets.begin(); mWidgetIter != mWidgets.end(); ++mWidgetIter)
    {
        const GuiWidget* widget = *mWidgetIter;
        if (widget->func != nullptr) widget->func();
    }
}
