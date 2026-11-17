#pragma once

#include <Types.h>
#include <Vulkan.h>

namespace VkUtil
{
	// Reads a compiled .spv file and wraps it in a VkShaderModule. Returns false if the file can't be read
	bool load_shader_module(const char* file_path, VkDevice device, VkShaderModule* out_shader_module);
}
