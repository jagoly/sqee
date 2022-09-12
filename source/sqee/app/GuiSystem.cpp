#include <sqee/app/GuiSystem.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/Window.hpp>
#include <sqee/core/Macros.hpp>
#include <sqee/core/Types.hpp>
#include <sqee/data/BuiltinShaders.hpp>
#include <sqee/data/UbuntuMinimal.hpp>
#include <sqee/maths/Colours.hpp>
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
    style.FramePadding              = { 6.f, 4.f };
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
    colors[ImGuiCol_PopupBg]                = ImVec4(0.15f, 0.15f, 0.15f, 0.90f);
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

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.BackendFlags = ImGuiBackendFlags_HasMouseCursors;

    io.MouseDrawCursor = true;
    io.ConfigWindowsResizeFromEdges = true;

    window.set_cursor_hidden(true);

    load_ubuntu_fonts();

    create_objects();
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

    std::strcpy(fontConfig.Name, "Ubuntu Regular");
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuRegular, sqee_UbuntuRegular_size, 16.f, &fontConfig);

    std::strcpy(fontConfig.Name, "Ubuntu Bold");
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuBold, sqee_UbuntuBold_size, 16.f, &fontConfig);

    std::strcpy(fontConfig.Name, "Ubuntu Italic");
    io.Fonts->AddFontFromMemoryCompressedTTF(sqee_UbuntuItalic, sqee_UbuntuItalic_size, 16.f, &fontConfig);

    std::strcpy(fontConfig.Name, "Ubuntu Mono Regular");
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
    mFontTexture.load_from_memory(pixels, size_t(width * height), config);

    const auto& ctx = VulkanContext::get();

    mDescriptorSetLayout = ctx.create_descriptor_set_layout ({
        vk::DescriptorSetLayoutBinding(0u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment)
    });

    mDescriptorSet = vk_allocate_descriptor_set(ctx, mDescriptorSetLayout);

    vk_update_descriptor_set (
        ctx, mDescriptorSet, DescriptorImageSampler(0u, 0u, mFontTexture.get_descriptor_info())
    );

    ImGui::GetIO().Fonts->TexID = &mDescriptorSet;
}

//============================================================================//

void GuiSystem::create_pipeline()
{
    const auto& ctx = VulkanContext::get();

    mPipelineLayout = ctx.create_pipeline_layout (
        mDescriptorSetLayout, vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0u, sizeof(Vec2F))
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

    constexpr ImGuiKey sqee_to_imgui[enum_count_v<Keyboard_Key>]
    {
        ImGuiKey_0,  // Num_0
        ImGuiKey_1,  // Num_1
        ImGuiKey_2,  // Num_2
        ImGuiKey_3,  // Num_3
        ImGuiKey_4,  // Num_4
        ImGuiKey_5,  // Num_5
        ImGuiKey_6,  // Num_6
        ImGuiKey_7,  // Num_7
        ImGuiKey_8,  // Num_8
        ImGuiKey_9,  // Num_9

        ImGuiKey_A,  // A
        ImGuiKey_B,  // B
        ImGuiKey_C,  // C
        ImGuiKey_D,  // D
        ImGuiKey_E,  // E
        ImGuiKey_F,  // F
        ImGuiKey_G,  // G
        ImGuiKey_H,  // H
        ImGuiKey_I,  // I
        ImGuiKey_J,  // J
        ImGuiKey_K,  // K
        ImGuiKey_L,  // L
        ImGuiKey_M,  // M
        ImGuiKey_N,  // N
        ImGuiKey_O,  // O
        ImGuiKey_P,  // P
        ImGuiKey_Q,  // Q
        ImGuiKey_R,  // R
        ImGuiKey_S,  // S
        ImGuiKey_T,  // T
        ImGuiKey_U,  // U
        ImGuiKey_V,  // V
        ImGuiKey_W,  // W
        ImGuiKey_X,  // X
        ImGuiKey_Y,  // Y
        ImGuiKey_Z,  // Z

        ImGuiKey_GraveAccent,   // Grave
        ImGuiKey_Minus,         // Dash
        ImGuiKey_Period,        // Equal
        ImGuiKey_LeftBracket,   // LeftBracket
        ImGuiKey_RightBracket,  // RightBracket
        ImGuiKey_Backslash,     // BackSlash

        ImGuiKey_Semicolon,   // SemiColon
        ImGuiKey_Apostrophe,  // Apostrophe
        ImGuiKey_Comma,       // Comma
        ImGuiKey_Period,      // Period
        ImGuiKey_Slash,       // Slash

        ImGuiKey_LeftShift,   // Shift_L
        ImGuiKey_RightShift,  // Shift_R
        ImGuiKey_LeftCtrl,    // Control_L
        ImGuiKey_RightCtrl,   // Control_R
        ImGuiKey_LeftAlt,     // Alt_L
        ImGuiKey_RightAlt,    // Alt_R
        ImGuiKey_LeftSuper,   // Super_L
        ImGuiKey_RightSuper,  // Super_R

        ImGuiKey_Menu,       // Menu
        ImGuiKey_Backspace,  // BackSpace
        ImGuiKey_Tab,        // Tab
        ImGuiKey_Enter,      // Return
        ImGuiKey_Space,      // Space
        ImGuiKey_Escape,     // Escape

        ImGuiKey_Insert,    // Insert
        ImGuiKey_Delete,    // Delete
        ImGuiKey_Home,      // Home
        ImGuiKey_End,       // End
        ImGuiKey_PageUp,    // PageUp
        ImGuiKey_PageDown,  // PageDown

        ImGuiKey_LeftArrow,   // Arrow_Left
        ImGuiKey_UpArrow,     // Arrow_Up
        ImGuiKey_RightArrow,  // Arrow_Right
        ImGuiKey_DownArrow,   // Arrow_Down

        ImGuiKey_Keypad0,  // Pad_0
        ImGuiKey_Keypad1,  // Pad_1
        ImGuiKey_Keypad2,  // Pad_2
        ImGuiKey_Keypad3,  // Pad_3
        ImGuiKey_Keypad4,  // Pad_4
        ImGuiKey_Keypad5,  // Pad_5
        ImGuiKey_Keypad6,  // Pad_6
        ImGuiKey_Keypad7,  // Pad_7
        ImGuiKey_Keypad8,  // Pad_8
        ImGuiKey_Keypad9,  // Pad_9

        ImGuiKey_KeypadDecimal,   // Pad_Decimal
        ImGuiKey_KeypadAdd,       // Pad_Plus
        ImGuiKey_KeypadSubtract,  // Pad_Minus
        ImGuiKey_KeypadMultiply,  // Pad_Multiply
        ImGuiKey_KeypadDivide,    // Pad_Divide
        ImGuiKey_KeypadEnter,     // Pad_Return

        ImGuiKey_F1,   // F1
        ImGuiKey_F2,   // F2
        ImGuiKey_F3,   // F3
        ImGuiKey_F4,   // F4
        ImGuiKey_F5,   // F5
        ImGuiKey_F6,   // F6
        ImGuiKey_F7,   // F7
        ImGuiKey_F8,   // F8
        ImGuiKey_F9,   // F9
        ImGuiKey_F10,  // F10
        ImGuiKey_F11,  // F11
        ImGuiKey_F12,  // F12
    };

    const auto add_modifier_events = [&](const auto& data)
    {
        io.AddKeyEvent(ImGuiKey_ModCtrl, data.ctrl);
        io.AddKeyEvent(ImGuiKey_ModShift, data.shift);
        io.AddKeyEvent(ImGuiKey_ModAlt, data.alt);
        io.AddKeyEvent(ImGuiKey_ModSuper, data.super);
    };

    //--------------------------------------------------------//

    SWITCH ( event.type ) {

    CASE ( Window_Unfocus, Window_Focus )
    {
        io.AddFocusEvent(event.type == Event::Type::Window_Focus);

        return false;
    }

    CASE ( Keyboard_Release, Keyboard_Press )
    {
        if (event.data.keyboard.key != Keyboard_Key::Unknown)
        {
            add_modifier_events(event.data.keyboard);
            io.AddKeyEvent(sqee_to_imgui[int(event.data.keyboard.key)], event.type == Event::Type::Keyboard_Press);
        }

        return io.WantCaptureKeyboard;
    }

    CASE ( Keyboard_Repeat )
    {
        // ImGui generates key repeats automatically
        return io.WantCaptureKeyboard;
    }

    CASE ( Mouse_Release, Mouse_Press )
    {
        if (event.data.mouse.button != Mouse_Button::Unknown)
        {
            add_modifier_events(event.data.mouse);
            io.AddMouseButtonEvent(int(event.data.mouse.button), event.type == Event::Type::Mouse_Press);
        }

        return io.WantCaptureMouse;
    }

    CASE ( Mouse_Scroll )
    {
        io.AddMouseWheelEvent(float(event.data.scroll.delta.x), float(event.data.scroll.delta.y));

        return io.WantCaptureMouse;
    }

    CASE ( Text_Entry )
    {
        io.AddInputCharacter(event.data.text.unicode);

        return io.WantTextInput;
    }

    CASE_DEFAULT { return false; }

    } SWITCH_END;
}

//============================================================================//

void GuiSystem::finish_handle_events()
{
    ImGuiIO& io = ImGui::GetIO();

    const auto displaySize = Vec2F(window.get_size());
    io.DisplaySize = ImVec2(displaySize.x, displaySize.y);

    // todo: update mouse movement with events
    const auto mousePos = Vec2F(input.get_cursor_location(false));
    if (mousePos != Vec2F(io.MousePos.x, io.MousePos.y))
        io.AddMousePosEvent(mousePos.x, mousePos.y);

    ImGui::NewFrame();
}

//============================================================================//

void GuiSystem::finish_scene_update(double elapsed)
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Render();

    io.DeltaTime = float(elapsed);
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

    const Vec2F invWindowSize2 = Vec2F(2.f, 2.f) / Vec2F(window.get_size());
    cmdbuf.pushConstants<Vec2F>(mPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0u, invWindowSize2);

    const vk::DescriptorSet* boundTexture = nullptr;

    //--------------------------------------------------------//

    // submit drawing commands using offsets into buffers
    {
        uint32_t vertexOffset = 0u;
        uint32_t indexOffset = 0u;

        for (int n = 0; n < drawData.CmdListsCount; ++n)
        {
            for (const ImDrawCmd& cmd : drawData.CmdLists[n]->CmdBuffer)
            {
                if (cmd.GetTexID() != boundTexture)
                {
                    boundTexture = static_cast<const vk::DescriptorSet*>(cmd.GetTexID());
                    cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0u, *boundTexture, {});
                }

                if (cmd.UserCallback == nullptr)
                {
                    const auto clipX = std::max(int32_t(cmd.ClipRect.x), 0);
                    const auto clipY = std::max(int32_t(cmd.ClipRect.y), 0);
                    const auto clipW = std::min(uint32_t(cmd.ClipRect.z - cmd.ClipRect.x), window.get_size().x);
                    const auto clipH = std::min(uint32_t(cmd.ClipRect.w - cmd.ClipRect.y), window.get_size().y);

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
