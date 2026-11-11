#pragma once

#include <Types.h>
#include <Vulkan.h>

struct SDL_Window;

// Frames de CPU may record ahead of the GPU: while the GPU draws frame N, the CPU records N+1
constexpr uint32 k_frame_overlap = 2;

// Everything one in-flight frame owns, so two frames never touch the same objects
struct FrameData
{
    VkCommandPool command_pool;
    VkCommandBuffer main_command_buffer;
    VkSemaphore swapchain_semaphore;
    VkFence render_fence;
};


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
    void init_swapchain();
    void init_commands();
    void init_sync_structures();

    void create_swapchain(const uint32 width, const uint32 height);
    void destroy_swapchain();

    FrameData& get_current_frame();

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

    VkSwapchainKHR swapchain;
    VkFormat swapchain_image_format;
    vector<VkImage> swapchain_images;
    vector<VkImageView> swapchain_image_views;
    VkExtent2D swapchain_extent;
    // One per swapchain image, not per frame: presentation may still be reading it
    vector<VkSemaphore> render_semaphores;

    FrameData frames[k_frame_overlap];

    VkQueue graphics_queue;
    uint32 graphics_queue_family;
};
