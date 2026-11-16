#pragma once

#include <Types.h>
#include <Vulkan.h>

#include <span>

// Collects bindings and builds a VkDescriptorSetLayout: the "signature" of the resources a shader expects
class DescriptorLayoutBuilder
{
public:
	DescriptorLayoutBuilder();

	void add_binding(const uint32 binding, const VkDescriptorType type);
	void clear();
	VkDescriptorSetLayout build(VkDevice device, const VkShaderStageFlags shader_stages);

private:
	vector<VkDescriptorSetLayoutBinding> bindings;
};

// One descriptor pool and the sets allocated from it. Simple version: the pool has a fixed size
class DescriptorAllocator
{
public:
	struct PoolSizeRatio
	{
		VkDescriptorType type;
		float32 ratio;
	};

	DescriptorAllocator();

	void init_pool(VkDevice device, const uint32 max_sets, std::span<const PoolSizeRatio> pool_ratios);
	void clear_descriptors(VkDevice device);
	void destroy_pool(VkDevice device);

	VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);

private:
	VkDescriptorPool pool;
};
