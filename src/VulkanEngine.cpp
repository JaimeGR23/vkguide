#include <VulkanEngine.h>

#include <VkImages.h>
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
    constexpr uint64 k_one_second_ns = 1000000000;
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
    swapchain(VK_NULL_HANDLE),
    swapchain_image_format(VK_FORMAT_UNDEFINED),
    swapchain_images(),
    swapchain_image_views(),
    swapchain_extent{ 0, 0 },
    render_semaphores(),
    frames{},
    main_deletion_queue(),
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
    init_swapchain();
    init_commands();
    init_sync_structures();

    is_initialized = true;
}

void VulkanEngine::shutdown()
{
    if (is_initialized)
    {
        // The GPU may still be using these objects: destroy nothing until it is idle
        vkDeviceWaitIdle(device);

        for (FrameData& frame : frames)
        {
            // Destroying the pool also frees the command buffers allocated from it
            vkDestroyCommandPool(device, frame.command_pool, nullptr);
            vkDestroyFence(device, frame.render_fence, nullptr);
            vkDestroySemaphore(device, frame.swapchain_semaphore, nullptr);
            frame.deletion_queue.flush();
        }

        main_deletion_queue.flush();

        destroy_swapchain();

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
    FrameData& frame = get_current_frame();

    // Wait until the GPU has finished the last submission that used this frame's objects
    VK_CHECK(vkWaitForFences(device, 1, &frame.render_fence, VK_TRUE, k_one_second_ns));
    // The GPU has finished this frame: whatever it used can be destroyed now
    frame.deletion_queue.flush();
    VK_CHECK(vkResetFences(device, 1, &frame.render_fence));

    uint32 swapchain_image_index = 0;
    VK_CHECK(vkAcquireNextImageKHR(device, swapchain, k_one_second_ns, frame.swapchain_semaphore, VK_NULL_HANDLE, &swapchain_image_index));

    VkCommandBuffer cmd = frame.main_command_buffer;
    VK_CHECK(vkResetCommandBuffer(cmd, 0));

    const VkCommandBufferBeginInfo begin_info = VkInit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info));

    VkImage image = swapchain_images[swapchain_image_index];

    // UNDEFINED as the old layout: we overwrite the whole image, so its old contents can be discarded
    VkUtil::transition_image(cmd, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    const float32 flash = std::abs(std::sin(static_cast<float32>(frame_number) / 120.0f));
    const VkClearColorValue clear_value = { {0.0f, 0.0f, flash, 1.0f} };
    const VkImageSubresourceRange clear_range = VkInit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
    vkCmdClearColorImage(cmd, image, VK_IMAGE_LAYOUT_GENERAL, &clear_value, 1, &clear_range);

    VkUtil::transition_image(cmd, image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    VK_CHECK(vkEndCommandBuffer(cmd));

    // Wait for the image to be acquired before writing to it; signal when rendering is done
    VkCommandBufferSubmitInfo cmd_info = VkInit::command_buffer_submit_info(cmd);
    VkSemaphoreSubmitInfo wait_info = VkInit::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, frame.swapchain_semaphore);
    VkSemaphoreSubmitInfo signal_info = VkInit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, render_semaphores[swapchain_image_index]);

    const VkSubmitInfo2 submit = VkInit::submit_info(&cmd_info, &signal_info, &wait_info);
    VK_CHECK(vkQueueSubmit2(graphics_queue, 1, &submit, frame.render_fence));

    VkPresentInfoKHR present_info = VkInit::present_info();
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_semaphores[swapchain_image_index];
    present_info.pImageIndices = &swapchain_image_index;
    VK_CHECK(vkQueuePresentKHR(graphics_queue, &present_info));

    frame_number++;
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

void VulkanEngine::init_swapchain()
{
    create_swapchain(window_extent.width, window_extent.height);
}

void VulkanEngine::init_commands()
{
    // RESET_COMMAND_BUFFER lets us reset each buffer on its own instead of the whole pool
    const VkCommandPoolCreateInfo pool_info = VkInit::command_pool_create_info(
        graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (FrameData& frame : frames)
    {
        VK_CHECK(vkCreateCommandPool(device, &pool_info, nullptr, &frame.command_pool));

        const VkCommandBufferAllocateInfo alloc_info = VkInit::command_buffer_allocate_info(frame.command_pool, 1);
        VK_CHECK(vkAllocateCommandBuffers(device, &alloc_info, &frame.main_command_buffer));
    }
}

void VulkanEngine::init_sync_structures()
{
    // Create signaled so the very first wait in draw() return inmediately
    const VkFenceCreateInfo fence_info = VkInit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
    const VkSemaphoreCreateInfo semaphore_info = VkInit::semaphore_create_info();

    for (FrameData& frame : frames)
    {
        VK_CHECK(vkCreateFence(device, &fence_info, nullptr, &frame.render_fence));
        VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &frame.swapchain_semaphore));
    }
}

FrameData& VulkanEngine::get_current_frame()
{
    return frames[frame_number % k_frame_overlap];
}

void VulkanEngine::create_swapchain(const uint32 width, const uint32 height)
{
    swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;  

    VkSurfaceFormatKHR surface_format = {};
    surface_format.format = swapchain_image_format;
    surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    vkb::SwapchainBuilder swapchain_builder{ chosen_gpu, device, surface };
    vkb::Swapchain vkb_swapchain = swapchain_builder
        .set_desired_format(surface_format)
        // FIFO = vsync. The only present mode every driver must support
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(width, height)
        // TRANSFER_DST so vkCmdClearColorImage (and later blits) can write to it
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

    swapchain = vkb_swapchain.swapchain;
    swapchain_extent = vkb_swapchain.extent;
    swapchain_images = vkb_swapchain.get_images().value();
    swapchain_image_views = vkb_swapchain.get_image_views().value();

    const VkSemaphoreCreateInfo semaphore_info = VkInit::semaphore_create_info();
    render_semaphores.resize(swapchain_images.size());
    for (VkSemaphore& semaphore : render_semaphores)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore));
    }

}

void VulkanEngine::destroy_swapchain()
{
    for (VkSemaphore semaphore : render_semaphores)
    {
        vkDestroySemaphore(device, semaphore, nullptr);
    }
    render_semaphores.clear();

    // The swapchain owns its images; only the views are ours to destroy
    for (VkImageView view : swapchain_image_views)
    {
        vkDestroyImageView(device, view, nullptr);
    }
    
    swapchain_image_views.clear();

    vkDestroySwapchainKHR(device, swapchain, nullptr);
}
