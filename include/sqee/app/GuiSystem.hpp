// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/vk/SwapBuffer.hpp>
#include <sqee/vk/VulkTexture.hpp>

#include <sqee/vk/Vulkan.hpp>

namespace sq {

//====== Forward Declarations ================================================//

struct Event;
class InputDevices;
class Window;

//============================================================================//

class SQEE_API GuiSystem final : private NonCopyable
{
public: //====================================================//

    GuiSystem(Window& window, InputDevices& inputDevices);

    ~GuiSystem();

    //--------------------------------------------------------//

    void set_style_widgets_default();
    void set_style_widgets_supertux();

    void set_style_colours_classic();
    void set_style_colours_dark();
    void set_style_colours_light();
    void set_style_colours_supertux();

    //--------------------------------------------------------//

    bool handle_event(Event event);

    void finish_handle_events(bool focus);

    void finish_scene_update(double elapsed);

    void render_gui(vk::CommandBuffer cmdbuf);

    //--------------------------------------------------------//

    void show_imgui_demo();

private: //===================================================//

    void load_ubuntu_fonts();

    void create_objects();
    void create_descriptor_set();
    void create_pipeline();

    Window& window;
    InputDevices& input;

    //--------------------------------------------------------//

    VulkTexture mFontTexture;
    SwapBuffer mVertexBuffer;
    SwapBuffer mIndexBuffer;

    vk::DescriptorSetLayout mDescriptorSetLayout;
    vk::DescriptorSet mDescriptorSet;

    vk::PipelineLayout mPipelineLayout;
    vk::Pipeline mPipeline;
};

//============================================================================//

} // namespace sq
