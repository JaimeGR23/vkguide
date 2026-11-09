# Naming: vkguide vs this repo

This repo follows the Lidar engine conventions. vkguide snippets must be translated when copied.

## Rules (same as Lidar)

- Functions, variables and members: `snake_case`, **no `_` prefix** (`_device` -> `device`).
- Types (classes, structs, enums) and namespaces: `PascalCase`.
- Constants: `k_` prefix (`FRAME_OVERLAP` -> `k_frame_overlap`).
- A parameter that clashes with a member: `in_` prefix.
- Use the `Types.h` aliases: `uint32`, `float32`, `vector`, `unique_ptr`, `string`, `vec3`, `mat4`...
- Members initialized in the constructor's initializer list, in declaration order (no `{ default }` in the header).
- Functions in the `.cpp` in the same order as in the header.
- Scalars as `const` by value, structs and vectors by `const&`.
- Two-phase init: cheap constructor, then `init()` / `shutdown()`.
- Allman braces, 4 spaces, CRLF.
- Headers in `include/`, sources in `src/`, file name = class or namespace name.
- Vulkan, VMA, fmt and `VK_CHECK` come only through `Vulkan.h`, the same way Lidar uses `OpenGL.h`.
- Comments in English, and only when they explain the why.

## Name map

| vkguide | Here |
|---|---|
| `vk_engine.h/.cpp` | `include/VulkanEngine.h`, `src/VulkanEngine.cpp` |
| `vk_types.h` | `include/Vulkan.h` (+ `include/Types.h`) |
| `vk_initializers.h/.cpp` | `include/VkInitializers.h`, `src/VkInitializers.cpp` |
| `vk_images`, `vk_descriptors`, `vk_pipelines`, `vk_loader` | `VkImages`, `VkDescriptors`, `VkPipelines`, `VkLoader` |
| `camera.h/.cpp` | `Camera.h/.cpp` |
| `namespace vkinit` | `namespace VkInit` |
| `namespace vkutil` | `namespace VkUtil` |
| `VulkanEngine::Get()` | `VulkanEngine::get()` |
| `VulkanEngine::cleanup()` | `VulkanEngine::shutdown()` |
| `_isInitialized`, `_frameNumber`, `_windowExtent`, `_window` | `is_initialized`, `frame_number`, `window_extent`, `window` |
| `_device`, `_chosenGPU`, `_swapchainImages`... | `device`, `chosen_gpu`, `swapchain_images`... |
| `uint32_t`, `float` | `uint32`, `float32` |
| `std::vector<T>` | `vector<T>` |

Vulkan's own API keeps its names (`vkCreateDevice`, `VkDevice`, `info.sType`...). Only our code is renamed.

When adding a new `.cpp`/`.h`, list it in `src/CMakeLists.txt` and regenerate (`cmake -S . -B build`).
