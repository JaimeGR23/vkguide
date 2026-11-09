#include <VulkanEngine.h>

#include <VkInitializers.h>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <cassert>
#include <chrono>
#include <thread>

namespace
{
    VulkanEngine* loaded_engine = nullptr;
}

VulkanEngine::VulkanEngine() :
    is_initialized(false),
    frame_number(0),
    stop_rendering(false),
    window_extent{ 1700, 900 },
    window(nullptr)
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

    is_initialized = true;
}

void VulkanEngine::shutdown()
{
    if (is_initialized)
    {
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
