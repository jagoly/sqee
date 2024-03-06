#pragma once

#include <sqee/export.hpp>

#ifndef SQEE_STATIC_LIB
    #define VULKAN_HPP_STORAGE_SHARED
    #define VULKAN_HPP_STORAGE_API SQEE_API
#endif

#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>
