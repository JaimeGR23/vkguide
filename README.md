# Learning Vulkan 1.3 with vkguide

My work through [vkguide.dev](https://vkguide.dev/), Victor Blanco's Vulkan guide, which targets **Vulkan 1.3**: dynamic rendering, synchronization2, buffer device address and bindless-friendly descriptors.

**Why:** I am learning modern explicit graphics APIs so I can port my C++ engine from **OpenGL 4.5 to Vulkan**. That engine has deferred PBR, DDGI global illumination, a client/server architecture and an EnTT ECS. This repo is where I learn the API step by step before designing that port.

## Progress

- [ ] Chapter 0: Project setup
- [ ] Chapter 1: Initializing Vulkan (instance, device, swapchain, command buffers, frame synchronization)
- [ ] Chapter 2: Drawing with compute (images, descriptors, compute shaders, Dear ImGui)
- [ ] Chapter 3: Graphics pipelines (dynamic rendering, mesh buffers, depth, glTF meshes)
- [ ] Chapter 4: Textures and engine architecture (descriptor abstraction, materials)
- [ ] Chapter 5: GLTF loading (scene graph, camera, full scenes)
- [ ] Extra chapters

## Notes

Things I learn along the way, especially how each concept maps to what I already do in OpenGL, go in [`notes/`](notes/).

The code follows the same conventions as my engine (snake_case functions and members, PascalCase types, `include/` + `src/`), not the guide's own style, so every snippet from the guide gets translated as I write it. The name map is in [`notes/naming.md`](notes/naming.md).

## Building

Requirements: Windows, Visual Studio 2022, [CMake](https://cmake.org/) 3.8+ and the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).

```
cmake -S . -B build
cmake --build build --config Debug
```

Or open the folder in Visual Studio as a CMake project, select the `engine` target and press F5. With the generated solution (`build/vulkan_guide.sln`), set `engine` as the startup project first.

## Credits

The starting point and third-party setup come from the `starting-point-2` branch of [vblanco20-1/vulkan-guide](https://github.com/vblanco20-1/vulkan-guide) (MIT, see `LICENSE.txt`). Third-party libraries in `third_party/` (vk-bootstrap, VMA, volk, SDL, GLM, {fmt}, stb_image, Dear ImGui, fastgltf) keep their own licenses. All code beyond the starting point is written by me while following the guide.
