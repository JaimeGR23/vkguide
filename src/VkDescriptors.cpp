#include <VkDescriptors.h>

#include <VkInitializers.h>

DescriptorLayoutBuilder::DescriptorLayoutBuilder() :
	bindings()
{
}

void DescriptorLayoutBuilder::add_binding(const uint32 binding, const VkDescriptorType type)
{
	// Stage fkafs are filled in build(), once for all bindings
	bindings.push_back(VkInit::descriptorset_layout_binding(type, 0, binding));
}

void DescriptorLayoutBuilder::clear()
{
	bindings.clear();
}

VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device, const VkShaderStageFlags shader_stages)
{
	for (VkDescriptorSetLayoutBinding& binding : bindings)
	{
		binding.stageFlags |= shader_stages;
	}

	const VkDescriptorSetLayoutCreateInfo info = VkInit::descriptorset_layout_create_info(bindings.data(), static_cast<uint32>(bindings.size()));

	VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
	VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set_layout));
	return set_layout;
}

DescriptorAllocator::DescriptorAllocator() :
	pool(VK_NULL_HANDLE)
{
}

void DescriptorAllocator::init_pool(VkDevice device, const uint32 max_sets, std::span<const PoolSizeRatio> pool_ratios)
{
	vector<VkDescriptorPoolSize> pool_sizes;
	for (const PoolSizeRatio& ratio : pool_ratios)
	{
		VkDescriptorPoolSize pool_size = {};
		pool_size.type = ratio.type;
		pool_size.descriptorCount = static_cast<uint32>(ratio.ratio * max_sets);
		pool_sizes.push_back(pool_size);
	}

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = max_sets;
	pool_info.poolSizeCount = static_cast<uint32>(pool_sizes.size());
	pool_info.pPoolSizes = pool_sizes.data();

	VK_CHECK(vkCreateDescriptorPool(device, &pool_info, nullptr, &pool));
}

void DescriptorAllocator::destroy_pool(VkDevice device)
{
	vkDestroyDescriptorPool(device, pool, nullptr);
}

VkDescriptorSet DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout)
{
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.pNext = nullptr;
	alloc_info.descriptorPool = pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &layout;

	VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
	VK_CHECK(vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set));
	return descriptor_set;

}