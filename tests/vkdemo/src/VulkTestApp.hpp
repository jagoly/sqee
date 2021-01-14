#pragma once

#include <sqee/app/Application.hpp>

#include <sqee/vk/VulkWindow.hpp>

using namespace sq::coretypes;

namespace sqt {

//============================================================================//

class VulkTestApp : public sq::Application
{

public: //====================================================//

    ~VulkTestApp();

private: //===================================================//

    void initialise(std::vector<std::string> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void create_demo_pipeline();

    void populate_command_buffer();

    void render(double elapsed);

    void handle_window_resize();

    //--------------------------------------------------------//

    std::unique_ptr<sq::VulkWindow> mWindow;

    vk::PipelineLayout mPipelineLayout;
    vk::Pipeline mPipeline;

    double mTimeAccum = 0.0;
    uint mFramesAccum = 0u;
};

} // namespace sqt
