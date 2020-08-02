#include <sqee/app/GuiSystem.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/Window.hpp>
#include <sqee/core/Macros.hpp>
#include <sqee/core/Types.hpp>
#include <sqee/data/UbuntuMinimal.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/Program.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/redist/gl_loader.hpp>

#include <dearimgui/imgui.h>

using namespace sq;

//============================================================================//

static constexpr const StringView VERTEX_SHADER_SOURCE = R"glsl(

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

static constexpr const StringView FRAGMENT_SHADER_SOURCE = R"glsl(

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

static GuiSystem* gGuiSystemPtr = nullptr;

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

    void finish_handle_events(bool focus);

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

void GuiSystem::set_style_widgets_default()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha                     = 1.f;
    style.WindowPadding             = { 8.f, 8.f };
    style.WindowRounding            = 7.f;
    style.WindowBorderSize          = 1.f;
    style.WindowMinSize             = { 32.f, 32.f };
    style.WindowTitleAlign          = { 0.f, 0.f };
    style.WindowMenuButtonPosition  = ImGuiDir_Left;
    style.ChildRounding             = 0.f;
    style.ChildBorderSize           = 1.f;
    style.PopupRounding             = 0.f;
    style.PopupBorderSize           = 1.f;
    style.FramePadding              = { 4.f, 3.f };
    style.FrameRounding             = 0.f;
    style.FrameBorderSize           = 0.f;
    style.ItemSpacing               = { 8.f, 4.f };
    style.ItemInnerSpacing          = { 4.f, 4.f };
    style.TouchExtraPadding         = { 0.f, 0.f };
    style.IndentSpacing             = 21.f;
    style.ColumnsMinSpacing         = 6.f;
    style.ScrollbarSize             = 14.f;
    style.ScrollbarRounding         = 9.f;
    style.GrabMinSize               = 10.f;
    style.GrabRounding              = 0.f;
    style.TabRounding               = 4.f;
    style.TabBorderSize             = 0.f;
    style.ColorButtonPosition       = ImGuiDir_Right;
    style.ButtonTextAlign           = { 0.5f, 0.5f };
    style.SelectableTextAlign       = { 0.f, 0.f };
    style.DisplayWindowPadding      = { 19.f, 19.f };
    style.DisplaySafeAreaPadding    = { 3.f, 3.f };
    style.MouseCursorScale          = 1.f;
}

void GuiSystem::set_style_widgets_supertux()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha                    = 1.f;
    style.WindowPadding            = { 6.f, 6.f };
    style.WindowRounding           = 8.f;
    style.WindowBorderSize         = 0.f;
    style.WindowMinSize            = { 32.f, 32.f };
    style.WindowTitleAlign         = { 0.f, 0.5f };
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding            = 4.f;
    style.ChildBorderSize          = 0.f;
    style.PopupRounding            = 0.f;
    style.PopupBorderSize          = 1.f;
    style.FramePadding             = { 6.f, 6.f };
    style.FrameRounding            = 4.f;
    style.FrameBorderSize          = 0.f;
    style.ItemSpacing              = { 6.f, 6.f };
    style.ItemInnerSpacing         = { 6.f, 6.f };
    style.TouchExtraPadding        = { 0.f, 0.f };
    style.IndentSpacing            = 20.f;
    style.ColumnsMinSpacing        = 6.f;
    style.ScrollbarSize            = 16.f;
    style.ScrollbarRounding        = 8.f;
    style.GrabMinSize              = 8.f;
    style.GrabRounding             = 4.f;
    style.TabRounding              = 4.f;
    style.TabBorderSize            = 0.f;
    style.ColorButtonPosition      = ImGuiDir_Right;
    style.ButtonTextAlign          = { 0.5f, 0.5f };
    style.SelectableTextAlign      = { 0.f, 0.f };
    style.DisplayWindowPadding     = { 20.f, 20.f };
    style.DisplaySafeAreaPadding   = { 4.f, 4.f };
    style.MouseCursorScale         = 1.f;
}

//============================================================================//

void GuiSystem::set_style_colours_classic()
{
    ImGui::StyleColorsClassic(&ImGui::GetStyle());
}

void GuiSystem::set_style_colours_dark()
{
    ImGui::StyleColorsDark(&ImGui::GetStyle());
}

void GuiSystem::set_style_colours_light()
{
    ImGui::StyleColorsLight(&ImGui::GetStyle());
}

void GuiSystem::set_style_colours_supertux()
{
    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 0.75f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.15f, 0.15f, 0.15f, 0.75f);
    colors[ImGuiCol_Border]                 = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.70f, 0.50f, 0.40f, 0.40f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.80f, 0.60f, 0.50f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.60f, 0.40f, 0.30f, 0.40f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.30f, 0.20f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.30f, 0.30f, 0.30f, 0.40f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.60f, 0.60f, 0.60f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.50f, 0.50f, 0.50f, 0.40f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.80f, 0.50f, 0.30f, 0.60f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.90f, 0.60f, 0.40f, 0.60f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.70f, 0.40f, 0.20f, 0.60f);
    colors[ImGuiCol_Header]                 = ImVec4(0.90f, 0.50f, 0.20f, 0.60f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(1.00f, 0.60f, 0.30f, 0.60f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.80f, 0.40f, 0.10f, 0.60f);
    colors[ImGuiCol_Separator]              = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.70f, 0.70f, 0.70f, 0.50f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.80f, 0.80f, 0.80f, 0.40f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.90f, 0.50f, 0.20f, 0.60f);
    colors[ImGuiCol_TabHovered]             = ImVec4(1.00f, 0.60f, 0.30f, 0.60f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.80f, 0.40f, 0.10f, 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.70f, 0.50f, 0.35f, 0.60f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.60f, 0.40f, 0.25f, 0.60f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.80f, 0.40f, 0.10f, 0.50f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.10f, 0.10f, 0.10f, 0.75f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.30f, 0.50f, 0.70f, 0.40f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

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
    io.KeyMap[ImGuiKey_Insert]      = int(Keyboard_Key::Insert);
    io.KeyMap[ImGuiKey_Delete]      = int(Keyboard_Key::Delete);
    io.KeyMap[ImGuiKey_Backspace]   = int(Keyboard_Key::BackSpace);
    io.KeyMap[ImGuiKey_Space]       = int(Keyboard_Key::Space);
    io.KeyMap[ImGuiKey_Enter]       = int(Keyboard_Key::Return);
    io.KeyMap[ImGuiKey_Escape]      = int(Keyboard_Key::Escape);
    //io.KeyMap[ImGuiKey_KeyPadEnter] = int(Keyboard_Key::);
    io.KeyMap[ImGuiKey_A]           = int(Keyboard_Key::A);
    io.KeyMap[ImGuiKey_C]           = int(Keyboard_Key::C);
    io.KeyMap[ImGuiKey_V]           = int(Keyboard_Key::V);
    io.KeyMap[ImGuiKey_X]           = int(Keyboard_Key::X);
    io.KeyMap[ImGuiKey_Y]           = int(Keyboard_Key::Y);
    io.KeyMap[ImGuiKey_Z]           = int(Keyboard_Key::Z);

    //--------------------------------------------------------//

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.BackendFlags = ImGuiBackendFlags_HasMouseCursors;

    io.MouseDrawCursor = true;
    io.ConfigWindowsResizeFromEdges = true;

    window.set_cursor_hidden(true);

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
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuRegular, sqee_UbuntuRegular_size, 16.f, &fontConfig);

    fontConfig.Name[StringView("Ubuntu Bold").copy(fontConfig.Name, MaxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuBold, sqee_UbuntuBold_size, 16.f, &fontConfig);

    fontConfig.Name[StringView("Ubuntu Italic").copy(fontConfig.Name, MaxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuItalic, sqee_UbuntuItalic_size, 16.f, &fontConfig);

    fontConfig.Name[StringView("Ubuntu Mono Regular").copy(fontConfig.Name, MaxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuMonoRegular, sqee_UbuntuMonoRegular_size, 16.f, &fontConfig);
}

//============================================================================//

void GuiSystem::Implementation::create_gl_objects()
{
    mProgram.load_vertex(VERTEX_SHADER_SOURCE);
    mProgram.load_fragment(FRAGMENT_SHADER_SOURCE);
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
            if (event.data.scroll.wheel == Mouse_Wheel::Vertical)
                io.MouseWheel += event.data.scroll.delta;

            if (event.data.scroll.wheel == Mouse_Wheel::Horizontal)
                io.MouseWheelH += event.data.scroll.delta;

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

void GuiSystem::Implementation::finish_handle_events(bool focus)
{
    const auto displaySize = Vec2F(window.get_window_size());
    io.DisplaySize = ImVec2(displaySize.x, displaySize.y);

    const auto mousePos = Vec2F(input.get_cursor_location(false));
    io.MousePos = ImVec2(mousePos.x, mousePos.y);

    if (focus == true)
    {
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
    }

    //--------------------------------------------------------//

    ImGui::NewFrame();
}

//============================================================================//

void GuiSystem::Implementation::finish_scene_update(double elapsed)
{
    ImGui::Render();

    //--------------------------------------------------------//

    io.DeltaTime = float(elapsed);

    io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = false;

    io.MouseWheel = 0.f;

    io.KeyShift = io.KeyCtrl = io.KeyAlt = io.KeySuper = false;
}

//============================================================================//

void GuiSystem::Implementation::render_gui()
{
    SQASSERT(ImGui::GetDrawData(), "imgui NewFrame() -> Render() sequence probably wrong");

    //--------------------------------------------------------//

    const ImDrawData& drawData = *ImGui::GetDrawData();

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

GuiSystem& GuiSystem::get()
{
    SQASSERT(gGuiSystemPtr != nullptr, "GuiSystem not constructed");
    return *gGuiSystemPtr;
}

void GuiSystem::construct(Window& window, InputDevices& inputDevices)
{
    SQASSERT(gGuiSystemPtr == nullptr, "GuiSystem::construct() already called");

    ImGui::CreateContext();

    gGuiSystemPtr = new GuiSystem();

    gGuiSystemPtr->impl = std::make_unique<Implementation>(window, inputDevices);
}

void GuiSystem::destruct()
{
    SQASSERT(gGuiSystemPtr != nullptr, "GuiSystem::destruct() already called");

    delete gGuiSystemPtr;

    ImGui::DestroyContext(); // todo: imgui probably can't be re-started after this
}

//============================================================================//

bool GuiSystem::handle_event(Event event) { return impl->handle_event(event); }

void GuiSystem::finish_handle_events(bool focus) { impl->finish_handle_events(focus); }

void GuiSystem::finish_scene_update(double elapsed) { impl->finish_scene_update(elapsed); }

void GuiSystem::render_gui() { impl->render_gui(); }

void GuiSystem::show_imgui_demo() { ImGui::ShowDemoWindow(); }
