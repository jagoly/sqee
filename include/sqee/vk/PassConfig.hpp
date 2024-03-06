#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/TypeAliases.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq { //###############################################################

/// A set of constants to freeze when compiling a Pipeline.
///
/// IDs to use in shaders for sq::Pipeline:
///  0 - 3: defined automatically
///  4 - 9: defined by PassConfig::constants
///  >= 10: defined by Pipeline JSON
///
/// For vk::Pipelines created manually, any IDs may be used.
///
struct SpecialisationConstants final
{
    template <class... Args>
    SpecialisationConstants(Args... args)
    {
        if constexpr (sizeof...(args) != 0u)
            append(args...);
    }

    template <class Value, class... Args>
    void append(uint id, Value value, Args... args)
    {
        const uint offset = uint(bytes.size());

        bytes.insert(bytes.end(), reinterpret_cast<const std::byte*>(&value), reinterpret_cast<const std::byte*>(&value) + sizeof(Value));
        entries.emplace_back(id, offset, sizeof(Value));

        if constexpr (sizeof...(args) != 0u) append(args...);
        else info = { uint(entries.size()), entries.data(), bytes.size(), bytes.data() };
    }

    template <class... Args>
    void append(const SpecialisationConstants& other, Args... args)
    {
        const uint offset = uint(bytes.size());

        bytes.insert(bytes.end(), other.bytes.begin(), other.bytes.end());

        for (const auto& entry : other.entries)
            entries.emplace_back(entry.constantID, offset + entry.offset, entry.size);

        if constexpr (sizeof...(args) != 0u) append(args...);
        else info = { uint(entries.size()), entries.data(), bytes.size(), bytes.data() };
    }

    std::vector<std::byte> bytes;
    std::vector<vk::SpecializationMapEntry> entries;

    vk::SpecializationInfo info;
};

//==============================================================================

/// Information about a RenderPass, for use by Pipelines.
struct PassConfig final
{
    vk::RenderPass renderPass;
    uint subpass;
    vk::SampleCountFlagBits samples;
    vk::StencilOpState stencil;
    Vec2U viewport;

    vk::PipelineLayout pipelineLayout;

    SpecialisationConstants constants;
};

using PassConfigMap = std::map<TinyString, PassConfig>;

} // namespace sq ##############################################################
