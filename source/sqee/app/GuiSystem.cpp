#include <sqee/app/GuiSystem.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/Window.hpp>
#include <sqee/core/Macros.hpp>
#include <sqee/core/Types.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/maths/Colours.hpp>

#include <sqee/data/UbuntuMinimal.hpp>
#include <sqee/data/BuiltinShaders.hpp>

#include <sqee/vk/Helpers.hpp>
#include <sqee/vk/VulkanContext.hpp>

#include <dearimgui/imgui.h>

using namespace sq;

//============================================================================//

static void impl_make_colors_linear(ImVec4* colors)
{
    for (int i = 0; i != ImGuiCol_COUNT; ++i)
    {
        colors[i].x = maths::srgb_to_linear(colors[i].x);
        colors[i].y = maths::srgb_to_linear(colors[i].y);
        colors[i].z = maths::srgb_to_linear(colors[i].z);
    }
}

//============================================================================//

void GuiSystem::set_style_widgets_default()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha                     = 1.f;
    style.WindowPadding             = { 8.f, 8.f };
    style.WindowRounding            = 0.f;
    style.WindowBorderSize          = 1.f;
    style.WindowMinSize             = { 32.f, 32.f };
    style.WindowTitleAlign          = { 0.f, 0.5f };
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
    style.CellPadding               = { 4.f, 2.f };
    style.TouchExtraPadding         = { 0.f, 0.f };
    style.IndentSpacing             = 21.f;
    style.ColumnsMinSpacing         = 6.f;
    style.ScrollbarSize             = 14.f;
    style.ScrollbarRounding         = 9.f;
    style.GrabMinSize               = 10.f;
    style.GrabRounding              = 0.f;
    style.LogSliderDeadzone         = 4.f;
    style.TabRounding               = 4.f;
    style.TabBorderSize             = 0.f;
    style.TabMinWidthForCloseButton = 0.f;
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

    style.Alpha                     = 1.f;
    style.WindowPadding             = { 6.f, 6.f };
    style.WindowRounding            = 8.f;
    style.WindowBorderSize          = 0.f;
    style.WindowMinSize             = { 32.f, 32.f };
    style.WindowTitleAlign          = { 0.f, 0.5f };
    style.WindowMenuButtonPosition  = ImGuiDir_Left;
    style.ChildRounding             = 4.f;
    style.ChildBorderSize           = 0.f;
    style.PopupRounding             = 0.f;
    style.PopupBorderSize           = 1.f;
    style.FramePadding              = { 6.f, 6.f };
    style.FrameRounding             = 4.f;
    style.FrameBorderSize           = 0.f;
    style.ItemSpacing               = { 6.f, 6.f };
    style.ItemInnerSpacing          = { 6.f, 6.f };
    style.CellPadding               = { 4.f, 4.f };
    style.TouchExtraPadding         = { 0.f, 0.f };
    style.IndentSpacing             = 20.f;
    style.ColumnsMinSpacing         = 6.f;
    style.ScrollbarSize             = 16.f;
    style.ScrollbarRounding         = 8.f;
    style.GrabMinSize               = 8.f;
    style.GrabRounding              = 4.f;
    style.LogSliderDeadzone         = 4.f;
    style.TabRounding               = 4.f;
    style.TabBorderSize             = 0.f;
    style.TabMinWidthForCloseButton = 0.f;
    style.ColorButtonPosition       = ImGuiDir_Right;
    style.ButtonTextAlign           = { 0.5f, 0.5f };
    style.SelectableTextAlign       = { 0.f, 0.f };
    style.DisplayWindowPadding      = { 20.f, 20.f };
    style.DisplaySafeAreaPadding    = { 4.f, 4.f };
    style.MouseCursorScale          = 1.f;
}

//============================================================================//

void GuiSystem::set_style_colours_classic()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsClassic(&style);
    impl_make_colors_linear(style.Colors);
}

void GuiSystem::set_style_colours_dark()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark(&style);
    impl_make_colors_linear(style.Colors);
}

void GuiSystem::set_style_colours_light()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsLight(&style);
    impl_make_colors_linear(style.Colors);
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
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.40f, 0.30f, 0.20f, 0.40f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.80f, 0.80f, 0.80f, 0.80f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.01f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.01f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.30f, 0.50f, 0.70f, 0.40f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    // todo: because of correct alpha blending, transparent colours now appear too bright
    impl_make_colors_linear(colors);
}

//============================================================================//

GuiSystem::GuiSystem(Window& window, InputDevices& inputDevices)
    : window(window), input(inputDevices)
{
    ImGui::SetCurrentContext(ImGui::CreateContext());

    ImGuiIO& io = ImGui::GetIO();

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

    load_ubuntu_fonts();

    create_objects();
    create_descriptor_set();
    create_pipeline();
}

//============================================================================//

GuiSystem::~GuiSystem()
{
    const auto& ctx = VulkanContext::get();

    ctx.device.free(ctx.descriptorPool, mDescriptorSet);
    ctx.device.destroy(mDescriptorSetLayout);

    ctx.device.destroy(mPipeline);
    ctx.device.destroy(mPipelineLayout);
}

//============================================================================//

void GuiSystem::load_ubuntu_fonts()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;

    const size_t maxNameLen = sizeof(ImFontConfig::Name);

    fontConfig.Name[StringView("Ubuntu Regular").copy(fontConfig.Name, maxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuRegular, sqee_UbuntuRegular_size, 16.f, &fontConfig);

    fontConfig.Name[StringView("Ubuntu Bold").copy(fontConfig.Name, maxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuBold, sqee_UbuntuBold_size, 16.f, &fontConfig);

    fontConfig.Name[StringView("Ubuntu Italic").copy(fontConfig.Name, maxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuItalic, sqee_UbuntuItalic_size, 16.f, &fontConfig);

    fontConfig.Name[StringView("Ubuntu Mono Regular").copy(fontConfig.Name, maxNameLen-1)] = '\0';
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuMonoRegular, sqee_UbuntuMonoRegular_size, 16.f, &fontConfig);
}

//============================================================================//

void GuiSystem::create_objects()
{
    mVertexBuffer.initialise(65536u * sizeof(ImDrawVert), vk::BufferUsageFlagBits::eVertexBuffer);
    mIndexBuffer.initialise(65536u * sizeof(ImDrawIdx), vk::BufferUsageFlagBits::eIndexBuffer);

    uchar* pixels; int width, height;
    ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    Texture::Config config;
    config.format = vk::Format::eR8Unorm;
    config.wrapX = config.wrapY = config.wrapZ = vk::SamplerAddressMode::eRepeat;
    config.swizzle.r = config.swizzle.g = config.swizzle.b = vk::ComponentSwizzle::eOne;
    config.swizzle.a = vk::ComponentSwizzle::eR;
    config.filter = Texture::FilterMode::Linear;
    config.mipmaps = Texture::MipmapsMode::Disable;
    config.size = Vec3U(width, height, 1u);
    config.mipLevels = 1u;

    mFontTexture.initialise_2D(config);
    mFontTexture.load_from_memory(pixels, 0u, 0u, config);
}

//============================================================================//

void GuiSystem::create_descriptor_set()
{
    const auto& ctx = VulkanContext::get();

    mDescriptorSetLayout = ctx.create_descriptor_set_layout ({
        vk::DescriptorSetLayoutBinding(0u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment)
    });

    mDescriptorSet = vk_allocate_descriptor_set(ctx, mDescriptorSetLayout);

    vk_update_descriptor_set (
        ctx, mDescriptorSet, DescriptorImageSampler(0u, 0u, mFontTexture.get_descriptor_info())
    );
}

//============================================================================//

void GuiSystem::create_pipeline()
{
    const auto& ctx = VulkanContext::get();

    mPipelineLayout = ctx.create_pipeline_layout (
        mDescriptorSetLayout, vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0u, sizeof(Mat4F))
    );

    // load shaders and create graphics pipeline
    {
        const auto shaderModules = ShaderModules (
            ctx, std::pair(sqee_GuiSystemVertSpv, sizeof(sqee_GuiSystemVertSpv)),
            {}, std::pair(sqee_GuiSystemFragSpv, sizeof(sqee_GuiSystemFragSpv))
        );

        const auto vertexBindingDescriptions = std::array {
            vk::VertexInputBindingDescription { 0u, sizeof(ImDrawVert), vk::VertexInputRate::eVertex }
        };

        const auto vertexAttributeDescriptions = std::array {
            vk::VertexInputAttributeDescription { 0u, 0u, vk::Format::eR32G32Sfloat, 0u },
            vk::VertexInputAttributeDescription { 1u, 0u, vk::Format::eR32G32Sfloat, 8u },
            vk::VertexInputAttributeDescription { 2u, 0u, vk::Format::eR8G8B8A8Unorm, 16u }
        };

        mPipeline = sq::vk_create_graphics_pipeline (
            ctx, mPipelineLayout, window.get_render_pass(), 0u, shaderModules.stages,
            vk::PipelineVertexInputStateCreateInfo {
                {}, vertexBindingDescriptions, vertexAttributeDescriptions
            },
            vk::PipelineInputAssemblyStateCreateInfo {
                {}, vk::PrimitiveTopology::eTriangleList, false
            },
            vk::PipelineRasterizationStateCreateInfo {
                {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone,
                vk::FrontFace::eClockwise, false, 0.f, false, 0.f, 1.f
            },
            vk::PipelineMultisampleStateCreateInfo {
                {}, vk::SampleCountFlagBits::e1, false, 0.f, nullptr, false, false
            },
            vk::PipelineDepthStencilStateCreateInfo {
                {}, false, false, {}, false, false, {}, {}, 0.f, 0.f
            },
            { 1u, nullptr }, { 1u, nullptr },
            vk::PipelineColorBlendAttachmentState {
                true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd,
                vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlags(0b1111)
            },
            { vk::DynamicState::eViewport, vk::DynamicState::eScissor }
        );
    }
}

//============================================================================//

bool GuiSystem::handle_event(Event event)
{
    ImGuiIO& io = ImGui::GetIO();

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
            const auto& data = event.data.keyboard;

            io.KeysDown[int(data.key)] = (event.type == Event::Type::Keyboard_Press);

            io.KeyShift = data.shift; io.KeyCtrl = data.ctrl;
            io.KeyAlt = data.alt; io.KeySuper = data.super;

            return io.WantCaptureKeyboard;
        }

        CASE_DEFAULT { return false; }

    } SWITCH_END;

    return false;
}

//============================================================================//

void GuiSystem::finish_handle_events(bool focus)
{
    ImGuiIO& io = ImGui::GetIO();

    const auto displaySize = Vec2F(window.get_size());
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

    ImGui::NewFrame();
}

//============================================================================//

void GuiSystem::finish_scene_update(double elapsed)
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Render();

    io.DeltaTime = float(elapsed);
    io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = false;
    io.MouseWheel = 0.f;
    io.KeyShift = io.KeyCtrl = io.KeyAlt = io.KeySuper = false;
}

//============================================================================//

void GuiSystem::render_gui(vk::CommandBuffer cmdbuf)
{
    const auto& drawData = *ImGui::GetDrawData();

    //--------------------------------------------------------//

    // copy all vertices/indices into same pair of buffers
    {
        ImDrawVert* vertexPtr = reinterpret_cast<ImDrawVert*>(mVertexBuffer.swap_map());
        ImDrawIdx* indexPtr = reinterpret_cast<ImDrawIdx*>(mIndexBuffer.swap_map());

        for (int n = 0; n < drawData.CmdListsCount; ++n)
        {
            const auto& vertexBuffer = drawData.CmdLists[n]->VtxBuffer;
            std::memcpy(vertexPtr, vertexBuffer.Data, vertexBuffer.size() * sizeof(ImDrawVert));
            vertexPtr += vertexBuffer.size();

            const auto& indexBuffer = drawData.CmdLists[n]->IdxBuffer;
            std::memcpy(indexPtr, indexBuffer.Data, indexBuffer.size() * sizeof(ImDrawIdx));
            indexPtr += indexBuffer.size();
        }
    }

    //--------------------------------------------------------//

    cmdbuf.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);

    cmdbuf.setViewport(0u, vk::Viewport(0.f, 0.f, float(window.get_size().x), float(window.get_size().y), 0.f, 1.f));

    cmdbuf.bindVertexBuffers(0u, mVertexBuffer.front(), size_t(0u));
    cmdbuf.bindIndexBuffer(mIndexBuffer.front(), 0u, vk::IndexType::eUint16);

    cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0u, mDescriptorSet, {});

    const auto orthoMatrix = Mat4F (
        { 2.f / float(window.get_size().x), 0.f, 0.f, 0.f },
        { 0.f, 2.f / float(window.get_size().y), 0.f, 0.f },
        { 0.f, 0.f, 1.f, 0.f },
        { -1.f, -1.f, 0.f, 1.f }
    );

    cmdbuf.pushConstants<Mat4F>(mPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0u, orthoMatrix);

    //--------------------------------------------------------//

    // subbmit drawing commands using offsets into buffers
    {
        uint32_t vertexOffset = 0u;
        uint32_t indexOffset = 0u;

        for (int n = 0; n < drawData.CmdListsCount; ++n)
        {
            for (const ImDrawCmd& cmd : drawData.CmdLists[n]->CmdBuffer)
            {
                if (cmd.UserCallback == nullptr)
                {
                    const auto clipX = int32_t(cmd.ClipRect.x);
                    const auto clipY = int32_t(cmd.ClipRect.y);
                    const auto clipW = uint32_t(cmd.ClipRect.z - cmd.ClipRect.x);
                    const auto clipH = uint32_t(cmd.ClipRect.w - cmd.ClipRect.y);

                    cmdbuf.setScissor(0u, vk::Rect2D({clipX, clipY}, {clipW, clipH}));
                    cmdbuf.drawIndexed(cmd.ElemCount, 1u, indexOffset+cmd.IdxOffset, vertexOffset+cmd.VtxOffset, 0u);
                }
                else cmd.UserCallback(drawData.CmdLists[n], &cmd);
            }

            vertexOffset += drawData.CmdLists[n]->VtxBuffer.size();
            indexOffset += drawData.CmdLists[n]->IdxBuffer.size();
        }
    }
}

//============================================================================//

void GuiSystem::show_imgui_demo() { ImGui::ShowDemoWindow(); }
