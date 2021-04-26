#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq {

//============================================================================//

/// Structure containing a renderpass plus metadata.
struct PassConfig
{
    vk::RenderPass renderPass;
    vk::SampleCountFlagBits samples;
    Vec2U viewport;
};

using PassConfigMap = std::map<TinyString, PassConfig>;

//============================================================================//

} // namespace sq
