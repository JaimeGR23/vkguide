#include <VulkanEngine.h>

#include <VkInitializers.h>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <VkBootstrap.h>

#include <cassert>
#include <chrono>
#include <cmath>
#include <thread>


namespace
{
    VulkanEngine* loaded_engine = nullptr;

    // Validation layers check every call against the spec. Turn off only for release builds
    constexpr bool k_use_validation_layers = true;

    // 1 second, in nanoseconds: Vulkan timeouts are always in nanoseconds
    constexpr uint64 k_one_nanosecond_ns = 1000000000;
}

VulkanEngine::VulkanEngine() :
    is_initialized(false),
    frame_number(0),
    stop_rendering(false),
    window_extent{ 1700, 900 },
    window(nullptr),
    instance(VK_NULL_HANDLE),
    debug_messenger(VK_NULL_HANDLE),
    chosen_gpu(VK_NULL_HANDLE),
    device(VK_NULL_HANDLE),
    surface(VK_NULL_HANDLE),
    graphics_queue(VK_NULL_HANDLE),
    graphics_queue_family(0)
{
}

VulkanEngine& VulkanEngine::get()
{
    return *loaded_engine;
}

void VulkanEngine::init()
{
    // Only one engine per application: get() hands out this instance
    assert(loaded_engine == nullptr);
    loaded_engine = this;

    SDL_Init(SDL_INIT_VIDEO);

    const SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN);

    window = SDL_CreateWindow(
        "Vulkan Engine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        static_cast<int>(window_extent.width),
        static_cast<int>(window_extent.height),
        window_flags);

    init_vulkan();

    is_initialized = true;
}

void VulkanEngine::shutdown()
{
    if (is_initialized)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyDevice(device, nullptr);

        vkb::destroy_debug_utils_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);

        SDL_DestroyWindow(window);
    }

    loaded_engine = nullptr;
}

void VulkanEngine::draw()
{
}

void VulkanEngine::run()
{
    SDL_Event event;
    bool quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }

            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
                {
                    stop_rendering = true;
                }
                if (event.window.event == SDL_WINDOWEVENT_RESTORED)
                {
                    stop_rendering = false;
                }
            }
        }

        if (stop_rendering)
        {
            // Minimized: sleep instead of spinning the loop at 100% CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw();
    }
}

void VulkanEngine::init_vulkan()
{
    vkb::InstanceBuilder instance_builder;
    vkb::Instance vkb_instance = instance_builder
        .set_app_name("Vulkan Engine")
        .request_validation_layers(k_use_validation_layers)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build()
        .value();

    instance = vkb_instance.instance;
    debug_messenger = vkb_instance.debug_messenger;

    SDL_Vulkan_CreateSurface(window, instance, &surface);

    // Vulkan 1.3 core feature this guide relies on
    VkPhysicalDeviceVulkan13Features features_13 = {};
    features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features_13.dynamicRendering = VK_TRUE;
    features_13.synchronization2 = VK_TRUE;

    VkPhysicalDeviceVulkan12Features features_12 = {};
    features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features_12.bufferDeviceAddress = VK_TRUE;
    features_12.descriptorIndexing = VK_TRUE;

    vkb::PhysicalDeviceSelector selector{ vkb_instance };
    vkb::PhysicalDevice physical_device = selector
        .set_minimum_version(1, 3)
        .set_required_features_13(features_13)
        .set_required_features_12(features_12)
        .set_surface(surface)
        .select()
        .value();

    vkb::DeviceBuilder device_builder{ physical_device };
    vkb::Device vkb_device = device_builder.build().value();

    chosen_gpu = physical_device.physical_device;
    device = vkb_device.device;

    graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
}

