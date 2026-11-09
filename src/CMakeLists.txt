set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

add_executable(engine
    Main.cpp
    VulkanEngine.cpp
    VkInitializers.cpp
    VkImages.cpp
    VkDescriptors.cpp
    VkPipelines.cpp
    VkLoader.cpp
    Camera.cpp
    ${INCLUDE_DIR}/Types.h
    ${INCLUDE_DIR}/Vulkan.h
    ${INCLUDE_DIR}/VulkanEngine.h
    ${INCLUDE_DIR}/VkInitializers.h
    ${INCLUDE_DIR}/VkImages.h
    ${INCLUDE_DIR}/VkDescriptors.h
    ${INCLUDE_DIR}/VkPipelines.h
    ${INCLUDE_DIR}/VkLoader.h
    ${INCLUDE_DIR}/Camera.h
)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)
target_compile_definitions(engine PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_include_directories(engine PUBLIC "${INCLUDE_DIR}")

target_link_libraries(engine PUBLIC vma glm Vulkan::Vulkan fmt::fmt stb_image SDL2::SDL2 vkbootstrap imgui fastgltf::fastgltf)

target_precompile_headers(engine PUBLIC <optional> <vector> <memory> <string> <unordered_map> <glm/mat4x4.hpp> <glm/vec4.hpp> <vulkan/vulkan.h>)

if(WIN32)
    add_custom_command(TARGET engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:engine> $<TARGET_FILE_DIR:engine>
        COMMAND_EXPAND_LISTS
    )
endif()
