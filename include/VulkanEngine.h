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
    bool is_initialized;
    uint32 frame_number;
    bool stop_rendering;
    VkExtent2D window_extent;

    SDL_Window* window;
};
