#pragma once

#include <sqee/setup.hpp>

#include <sqee/objects/Texture.hpp>
#include <sqee/vk/SwapBuffer.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq { //###############################################################

struct Event;
class InputDevices;
class Window;

//==============================================================================

class SQEE_API GuiSystem
{
public: //======================================================

    GuiSystem(Window& window, InputDevices& inputDevices);

    SQEE_COPY_DELETE(GuiSystem)
    SQEE_MOVE_DELETE(GuiSystem)

    ~GuiSystem();

    //----------------------------------------------------------

    void set_style_widgets_default();
    void set_style_widgets_supertux();

    void set_style_colours_classic();
    void set_style_colours_dark();
    void set_style_colours_light();
    void set_style_colours_supertux();

    //----------------------------------------------------------

    bool handle_event(Event event);

    void finish_handle_events();

    void finish_scene_update(double elapsed);

    void render_gui(vk::CommandBuffer cmdbuf);

    //----------------------------------------------------------

    /// Contains a single CombinedImageSampler.
    vk::DescriptorSetLayout get_descriptor_set_layout() const { return mDescriptorSetLayout; }

    //----------------------------------------------------------

    void show_imgui_demo();

private: //=====================================================

    void load_ubuntu_fonts();

    void create_objects();
    void create_pipeline();

    Window& window;
    InputDevices& input;

    //----------------------------------------------------------

    Texture mFontTexture;
    SwapBuffer mVertexBuffer;
    SwapBuffer mIndexBuffer;

    vk::DescriptorSetLayout mDescriptorSetLayout;
    vk::DescriptorSet mDescriptorSet;

    vk::PipelineLayout mPipelineLayout;
    vk::Pipeline mPipeline;
};

} // namespace sq ##############################################################
