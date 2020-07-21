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

#include <sqee/data/UbuntuMinimal.hpp>

#include <sqee/macros.hpp>

using namespace sq;

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

GuiSystem* gGuiSystemPtr = nullptr;

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

    auto& style = ImGui::GetStyle();

    // all of these values are floats, but any values written as ints here represent pixels
    DISABLE_WARNING_NARROWING_CONSTRUCTOR;

    style.Alpha                    = 1.0;            // 1.0
    style.WindowPadding            = { 6, 6 };       // 8, 8
    style.WindowRounding           = 8;              // 7.0
    style.WindowBorderSize         = 0;              // 1
    style.WindowMinSize            = { 32, 32 };     // 32, 32
    style.WindowTitleAlign         = { 0.0, 0.5 };   // 0.0, 0.5
    style.WindowMenuButtonPosition = ImGuiDir_Left;  // ImGuiDir_Left
    style.ChildRounding            = 4;              // 0
    style.ChildBorderSize          = 0;              // 1
    style.PopupRounding            = 0;              // 0
    style.PopupBorderSize          = 1;              // 1
    style.FramePadding             = { 6, 6 };       // 4, 3
    style.FrameRounding            = 4;              // 0
    style.FrameBorderSize          = 0;              // 0
    style.ItemSpacing              = { 6, 6 };       // 8, 4
    style.ItemInnerSpacing         = { 6, 6 };       // 4, 4
    style.TouchExtraPadding        = { 0, 0 };       // 0, 0
    style.IndentSpacing            = 20;             // 21
    style.ColumnsMinSpacing        = 6;              // 6
    style.ScrollbarSize            = 16;             // 14
    style.ScrollbarRounding        = 8;              // 9
    style.GrabMinSize              = 8;              // 10
    style.GrabRounding             = 4;              // 0
    style.TabRounding              = 4;              // 4
    style.TabBorderSize            = 0;              // 0
    style.ColorButtonPosition      = ImGuiDir_Right; // ImGuiDir_Right
    style.ButtonTextAlign          = { 0.5, 0.5 }; // 0.5, 0.5
    style.SelectableTextAlign      = { 0.0, 0.0 };   // 0.0, 0.0
    style.DisplayWindowPadding     = { 20, 20 };     // 19, 19
    style.DisplaySafeAreaPadding   = { 4, 4 };       // 3, 3
    style.MouseCursorScale         = 1.0;            // 1.0
    style.AntiAliasedLines         = true;           // true
    style.AntiAliasedFill          = true;           // true
    style.CurveTessellationTol     = 1.25;           // 1.25

    //--------------------------------------------------------//

    const auto get_style_colour = [](ImGuiCol_ colour) -> ImVec4
    {
        // by using a switch, we get warnings when ImGui adds a new enum

        switch (colour) {

        case ImGuiCol_Text:                  return { 1.0,  1.0,  1.0,  1.0  }; // 1.00, 1.00, 1.00, 1.00
        case ImGuiCol_TextDisabled:          return { 0.5,  0.5,  0.5,  1.0  }; // 0.50, 0.50, 0.50, 1.00
        case ImGuiCol_WindowBg:              return { 0.1,  0.1,  0.1,  0.75 }; // 0.06, 0.06, 0.06, 0.94
        case ImGuiCol_ChildBg:               return { 0.0,  0.0,  0.0,  0.0  }; // 1.00, 1.00, 1.00, 0.00
        case ImGuiCol_PopupBg:               return { 0.15, 0.15, 0.15, 0.75 }; // 0.08, 0.08, 0.08, 0.94
        case ImGuiCol_Border:                return { 0.8,  0.8,  0.8,  0.5  }; // 0.43, 0.43, 0.50, 0.50
        case ImGuiCol_BorderShadow:          return { 0.0,  0.0,  0.0,  0.0  }; // 0.00, 0.00, 0.00, 0.00
        case ImGuiCol_FrameBg:               return { 0.7,  0.5,  0.4,  0.4  }; // 0.16, 0.29, 0.48, 0.54
        case ImGuiCol_FrameBgHovered:        return { 0.8,  0.6,  0.5,  0.4  }; // 0.26, 0.59, 0.98, 0.40
        case ImGuiCol_FrameBgActive:         return { 0.6,  0.4,  0.3,  0.4  }; // 0.26, 0.59, 0.98, 0.67
        case ImGuiCol_TitleBg:               return { 0.2,  0.2,  0.2,  1.0  }; // 0.04, 0.04, 0.04, 1.00
        case ImGuiCol_TitleBgActive:         return { 0.3,  0.2,  0.3,  1.0  }; // 0.16, 0.29, 0.48, 1.00
        case ImGuiCol_TitleBgCollapsed:      return { 0.3,  0.3,  0.3,  0.4  }; // 0.00, 0.00, 0.00, 0.51
        case ImGuiCol_MenuBarBg:             return { 0.2,  0.2,  0.2,  1.0  }; // 0.14, 0.14, 0.14, 1.00
        case ImGuiCol_ScrollbarBg:           return { 0.4,  0.4,  0.4,  0.4  }; // 0.02, 0.02, 0.02, 0.53
        case ImGuiCol_ScrollbarGrab:         return { 0.6,  0.6,  0.6,  0.4  }; // 0.31, 0.31, 0.31, 1.00
        case ImGuiCol_ScrollbarGrabHovered:  return { 0.7,  0.7,  0.7,  0.4  }; // 0.41, 0.41, 0.41, 1.00
        case ImGuiCol_ScrollbarGrabActive:   return { 0.5,  0.5,  0.5,  0.4  }; // 0.51, 0.51, 0.51, 1.00
        case ImGuiCol_CheckMark:             return { 0.9,  0.9,  0.9,  0.5  }; // 0.26, 0.59, 0.98, 1.00
        case ImGuiCol_SliderGrab:            return { 1.0,  1.0,  1.0,  0.3  }; // 0.24, 0.52, 0.88, 1.00
        case ImGuiCol_SliderGrabActive:      return { 0.8,  0.5,  0.5,  1.0  }; // 0.26, 0.59, 0.98, 1.00
        case ImGuiCol_Button:                return { 0.8,  0.5,  0.3,  0.6  }; // 0.26, 0.59, 0.98, 0.40
        case ImGuiCol_ButtonHovered:         return { 0.9,  0.6,  0.4,  0.6  }; // 0.26, 0.59, 0.98, 1.00
        case ImGuiCol_ButtonActive:          return { 0.7,  0.4,  0.2,  0.6  }; // 0.06, 0.53, 0.98, 1.00
        case ImGuiCol_Header:                return { 0.9,  0.5,  0.2,  0.6  }; // 0.26, 0.59, 0.98, 0.31
        case ImGuiCol_HeaderHovered:         return { 1.0,  0.6,  0.3,  0.6  }; // 0.26, 0.59, 0.98, 0.80
        case ImGuiCol_HeaderActive:          return { 0.8,  0.4,  0.1,  0.6  }; // 0.26, 0.59, 0.98, 1.00
        case ImGuiCol_Separator:             return { 0.8,  0.8,  0.8,  0.5  }; // 0.43, 0.43, 0.50, 0.50
        case ImGuiCol_SeparatorHovered:      return { 0.9,  0.9,  0.9,  0.5  }; // 0.10, 0.40, 0.75, 0.78
        case ImGuiCol_SeparatorActive:       return { 0.7,  0.7,  0.7,  0.5  }; // 0.10, 0.40, 0.75, 1.00
        case ImGuiCol_ResizeGrip:            return { 0.8,  0.8,  0.8,  0.3  }; // 0.26, 0.59, 0.98, 0.25
        case ImGuiCol_ResizeGripHovered:     return { 0.8,  0.8,  0.8,  0.4  }; // 0.26, 0.59, 0.98, 0.67
        case ImGuiCol_ResizeGripActive:      return { 0.8,  0.8,  0.8,  0.5  }; // 0.26, 0.59, 0.98, 0.95
        case ImGuiCol_Tab:                   return { 0.9,  0.5 , 0.2,  0.6  }; // 0.18, 0.35, 0.58, 0.86
        case ImGuiCol_TabHovered:            return { 1.0,  0.6,  0.3,  0.6  }; // 0.26, 0.59, 0.98, 0.80
        case ImGuiCol_TabActive:             return { 0.8,  0.4,  0.1,  0.6  }; // 0.20, 0.41, 0.68, 1.00
        case ImGuiCol_TabUnfocused:          return { 0.7,  0.5,  0.35, 0.6  }; // 0.07, 0.10, 0.15, 0.97
        case ImGuiCol_TabUnfocusedActive:    return { 0.6,  0.4,  0.25, 0.6  }; // 0.14, 0.26, 0.42, 1.00
        case ImGuiCol_DockingPreview:        return { 0.8,  0.4,  0.1,  0.5  }; // 0.26, 0.59, 0.98, 0.70
        case ImGuiCol_DockingEmptyBg:        return { 0.1,  0.1,  0.1,  0.75 }; // 0.20, 0.20, 0.20, 1.00
        case ImGuiCol_PlotLines:             return { 0.61, 0.61, 0.61, 1.0  }; // 0.61, 0.61, 0.61, 1.00
        case ImGuiCol_PlotLinesHovered:      return { 1.0,  0.43, 0.35, 1.0  }; // 1.00, 0.43, 0.35, 1.00
        case ImGuiCol_PlotHistogram:         return { 0.9,  0.7,  0.0,  1.0  }; // 0.90, 0.70, 0.00, 1.00
        case ImGuiCol_PlotHistogramHovered:  return { 1.0,  0.6,  0.0,  1.0  }; // 1.00, 0.60, 0.00, 1.00
        case ImGuiCol_TextSelectedBg:        return { 0.3,  0.5,  0.7,  0.4  }; // 0.26, 0.59, 0.98, 0.35
        case ImGuiCol_DragDropTarget:        return { 1.0,  1.0,  0.0,  0.9  }; // 1.00, 1.00, 0.00, 0.90
        case ImGuiCol_NavHighlight:          return { 0.26, 0.59, 0.98, 1.0  }; // 0.26, 0.59, 0.98, 1.00
        case ImGuiCol_NavWindowingHighlight: return { 1.0,  1.0,  1.0,  0.7  }; // 1.00, 1.00, 1.00, 0.70
        case ImGuiCol_NavWindowingDimBg:     return { 0.8,  0.8,  0.8,  0.2  }; // 0.80, 0.80, 0.80, 0.20
        case ImGuiCol_ModalWindowDimBg:      return { 0.8,  0.8,  0.8,  0.35 }; // 0.80, 0.80, 0.80, 0.35

        case ImGuiCol_COUNT: {}

        } // switch (colour)

        return {};
    };

    ENABLE_WARNING_NARROWING_CONSTRUCTOR;

    for (int i = 0; i < ImGuiCol_COUNT; ++i)
        style.Colors[i] = get_style_colour(ImGuiCol_(i));

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
    io.DisplaySize = Vec2F(window.get_window_size());
    io.MousePos = Vec2F(input.get_cursor_location(false));

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

//============================================================================//

void GuiSystem::enable_widget(GuiWidget& widget)
{
    const auto iter = algo::find(mWidgets, &widget);

    if (iter == mWidgets.end())
        mWidgets.push_front(&widget);

    else SQASSERT(false, "");
}

void GuiSystem::disable_widget(GuiWidget& widget)
{
    const auto iter = algo::find(mWidgets, &widget);

    if (iter != mWidgets.end())
        *iter = nullptr;

    else SQASSERT(false, "");
}

//============================================================================//

void GuiSystem::draw_widgets()
{
    for (mWidgetIter = mWidgets.begin(); mWidgetIter != mWidgets.end(); ++mWidgetIter)
    {
        if (*mWidgetIter == nullptr)
            mWidgetIter = mWidgets.erase(mWidgetIter);

        else if ((*mWidgetIter)->func != nullptr)
            (*mWidgetIter)->func();
    }
}
