#include <cstring> // strcpy

#include <sqee/redist/imgui/imgui.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/macros.hpp>

#include <sqee/app/Window.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/Event.hpp>

#include <sqee/app/GuiSystem.hpp>

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
private: //===================================================//

    Window& window;
    InputDevices& input;

    ImGuiIO& io;
    Context& context;

    //--------------------------------------------------------//

    bool mMousePressed[3] {};
    float mMouseWheel = 0.0f;
    sq::Texture2D mTexture;
    sq::Program mProgram;
    sq::VertexArray mVAO;
    sq::FixedBuffer mVBO, mIBO;

public: //====================================================//

    Implementation(Window& window, InputDevices& inputDevices)
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

        //--------------------------------------------------------//

        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        fontConfig.OversampleH = 2;
        fontConfig.OversampleV = 2;

        std::strcpy(fontConfig.Name, "Ubuntu Regular");
        io.Fonts->AddFontFromMemoryCompressedTTF(data_UbuntuRegular, data_UbuntuRegular_size, 16.f, &fontConfig);

        std::strcpy(fontConfig.Name, "Ubuntu Bold");
        io.Fonts->AddFontFromMemoryCompressedTTF(data_UbuntuBold, data_UbuntuBold_size, 16.f, &fontConfig);

        std::strcpy(fontConfig.Name, "Ubuntu Italic");
        io.Fonts->AddFontFromMemoryCompressedTTF(data_UbuntuItalic, data_UbuntuItalic_size, 16.f, &fontConfig);

        std::strcpy(fontConfig.Name, "Ubuntu Mono Regular");
        io.Fonts->AddFontFromMemoryCompressedTTF(data_UbuntuMonoRegular, data_UbuntuMonoRegular_size, 16.f, &fontConfig);

        //--------------------------------------------------------//

        create_gl_objects();

        //io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
        //io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;
        //io.ClipboardUserData = g_Window;
        //#ifdef _WIN32
        //    io.ImeWindowHandle = glfwGetWin32Window(g_Window);
        //#endif
    }

    //--------------------------------------------------------//

    void create_gl_objects()
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
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        mTexture.set_format(sq::Texture::Format::RGBA8_UN);
        mTexture.set_filter_mode(true);
        mTexture.allocate_storage({uint(width), uint(height)});
        mTexture.load_memory(pixels);

        io.Fonts->TexID = static_cast<void*>(&mTexture);
    }

    //--------------------------------------------------------//

    bool handle_event(Event event)
    {
        SWITCH ( event.type ) {

            //CASE ( Mouse_Scroll )
            //{
            //    if (event->wheel.y > 0)
            //        g_MouseWheel = 1;
            //    if (event->wheel.y < 0)
            //        g_MouseWheel = -1;
            //    return true;
            //}

            CASE ( Mouse_Press )
            {
                const decltype(Event::Data::mouse)& data = event.data.mouse;

                mMousePressed[0] |= (data.button == Mouse_Button::Left);
                mMousePressed[1] |= (data.button == Mouse_Button::Right);
                mMousePressed[2] |= (data.button == Mouse_Button::Middle);

                return true;
            }

            CASE ( Text_Entry )
            {
                const auto code = event.data.text_entry.unicode;

                if (code < 128u && std::isprint(char(code)))
                    io.AddInputCharacter(static_cast<ImWchar>(code));

                return true;
            }

            CASE ( Keyboard_Press, Keyboard_Release )
            {
                const decltype(Event::Data::keyboard)& data = event.data.keyboard;

                io.KeysDown[int(data.key)] = (event.type == Event::Type::Keyboard_Press);

                io.KeyShift = data.shift; io.KeyCtrl = data.ctrl;
                io.KeyAlt = data.alt; io.KeySuper = data.super;

                return true;
            }

            CASE_DEFAULT { return false; }

        } SWITCH_END;
    }

    //--------------------------------------------------------//

    void begin_new_frame(double elapsed)
    {
        io.DeltaTime = float(elapsed);

        io.DisplaySize = Vec2F(window.get_window_size());
        io.MousePos = Vec2F(input.get_cursor_location());

        io.MouseDown[0] = mMousePressed[0] || input.is_pressed(Mouse_Button::Left);
        io.MouseDown[1] = mMousePressed[1] || input.is_pressed(Mouse_Button::Right);
        io.MouseDown[2] = mMousePressed[2] || input.is_pressed(Mouse_Button::Middle);

        mMousePressed[0] = false; mMousePressed[1] = false; mMousePressed[1] = false;

        //io.MouseWheel = g_MouseWheel;
        //g_MouseWheel = 0.0f;

        ImGui::NewFrame();
    }

    //--------------------------------------------------------//

    void render_draw_lists(ImDrawData* drawData)
    {
        const Vec2U vp = context.get_ViewPort();

        context.bind_FrameBuffer_default();

        context.set_state(Context::Blend_Mode::Alpha);
        context.set_state(Context::Cull_Face::Disable);
        context.set_state(Context::Depth_Test::Disable);

        gl::Enable(gl::SCISSOR_TEST);

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

        for (int n = 0; n < drawData->CmdListsCount; ++n)
        {
            const ImDrawList* cmdList = drawData->CmdLists[n];
            const ImDrawIdx* indexBufferOffset = 0u;

            mVBO.update(0u, uint(cmdList->VtxBuffer.Size) * sizeof(ImDrawVert), cmdList->VtxBuffer.Data);
            mIBO.update(0u, uint(cmdList->IdxBuffer.Size) * sizeof(ImDrawIdx), cmdList->IdxBuffer.Data);

            for (int m = 0; m < cmdList->CmdBuffer.Size; ++m)
            {
                const ImDrawCmd* cmd = &cmdList->CmdBuffer[m];

                if (cmd->UserCallback == nullptr)
                {
                    gl::Scissor ( int(cmd->ClipRect.x), int(vp.y - cmd->ClipRect.w),
                                  int(cmd->ClipRect.z - cmd->ClipRect.x),
                                  int(cmd->ClipRect.w - cmd->ClipRect.y) );

                    gl::DrawElements(gl::TRIANGLES, int(cmd->ElemCount), gl::UNSIGNED_SHORT, indexBufferOffset);
                }
                else cmd->UserCallback(cmdList, cmd);

                indexBufferOffset += cmd->ElemCount;
            }
        }

        gl::Disable(gl::SCISSOR_TEST);
    }

    //--------------------------------------------------------//

    ~Implementation()
    {
        ImGui::Shutdown();
    }
};

//============================================================================//

GuiSystem::GuiSystem(Window& window, InputDevices& inputDevices)
{
    impl = std::make_unique<Implementation>(window, inputDevices);
}

GuiSystem::~GuiSystem() = default;

bool GuiSystem::handle_event(Event event)
{
    return impl->handle_event(event);
}

void GuiSystem::begin_new_frame(double elapsed)
{
    impl->begin_new_frame(elapsed);
}

void GuiSystem::render_gui()
{
    ImGui::Render();
    impl->render_draw_lists(ImGui::GetDrawData());
}


//static const char* ImGui_ImplGlfwGL3_GetClipboardText(void* user_data)
//{
//    return glfwGetClipboardString((GLFWwindow*)user_data);
//}

//static void ImGui_ImplGlfwGL3_SetClipboardText(void* user_data, const char* text)
//{
//    glfwSetClipboardString((GLFWwindow*)user_data, text);
//}
