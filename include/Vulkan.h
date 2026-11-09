#pragma once

// Single entry point for Vulkan, VMA and the VK_CHECK macro (same role as OpenGL.h in Lidar)
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <fmt/core.h>

#include <cstdlib>

// Most Vulkan calls return a VkResult: abort on the first failure so errors surface where they happen
#define VK_CHECK(x)                                                              \
    do                                                                           \
    {                                                                            \
        const VkResult vk_check_result = x;                                      \
        if (vk_check_result != VK_SUCCESS)                                       \
        {                                                                        \
            fmt::println("Detected Vulkan error: {}", string_VkResult(vk_check_result)); \
            std::abort();                                                        \
        }                                                                        \
    } while (0)
