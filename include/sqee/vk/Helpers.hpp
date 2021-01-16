#pragma once

#include <sqee/export.hpp>

#include <sqee/vk/VulkContext.hpp>

namespace sq {

SQEE_API std::tuple<vk::Buffer, vk::DeviceMemory> vk_create_buffer
(const VulkContext& ctx, size_t size, vk::BufferUsageFlags usage, bool host);

SQEE_API std::tuple<vk::Image, vk::DeviceMemory> vk_create_image
(const VulkContext& ctx, Vec2U size, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, bool host);

SQEE_API void vk_copy_buffer
(const VulkContext& ctx, vk::Buffer srcBuffer, vk::Buffer dstBuffer, size_t size);

//SQEE_API void vk_transition_image_layout
//(const VulkContext& ctx, vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

SQEE_API void vk_transfer_buffer_to_image
(const VulkContext& ctx, vk::Buffer srcBuffer, vk::Image dstImage, Vec2U size);

} // namespace sq
