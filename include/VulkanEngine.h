#pragma once

#include <Types.h>
#include <Vulkan.h>

struct SDL_Window;

class VulkanEngine
{
public:
    VulkanEngine();

    static VulkanEngine& get();

    void init();
    void shutdown();
    void draw();
    void run();

private:
    void init_vulkan();

    bool is_initialized;
    uint32 frame_number;
    bool stop_rendering;
    VkExtent2D window_extent;

    SDL_Window* window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice chosen_gpu;
    VkDevice device;
    VkSurfaceKHR surface;

    VkQueue graphics_queue;
    uint32 graphics_queue_family;
};
