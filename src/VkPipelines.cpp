#include <VkPipelines.h>

#include <fstream>

bool VkUtil::load_shader_module(const char* file_path, VkDevice device, VkShaderModule* out_shader_module)
{
	// ate: start at the end, so tellg() gives the file size
	std::ifstream file(file_path, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	const size_t file_size = static_cast<size_t>(file.tellg());

	// SPIR-V is a stream of 32-bit words, and Vulkan want it as uint32
	vector<uint32> buffer(file_size / sizeof(uint32));

	file.seekg(0);
	file.read(reinterpret_cast<char*>(buffer.data()), file_size);
	file.close();

	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.pNext = nullptr;
	// codeSize is in bytes, not in words
	create_info.codeSize = buffer.size() * sizeof(uint32);
	create_info.pCode = buffer.data();

	VkShaderModule shader_module = VK_NULL_HANDLE;
	if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
	{
		return false;
	}

	*out_shader_module = shader_module;
	return true;
}
