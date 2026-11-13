#pragma once

#include <Types.h>
#include <Vulkan.h>

// An image together with its view and the VMA memory behind it
struct AllocatedImage
{
	VkImage image;
	VkImageView image_view;
	VmaAllocation allocation;
	VkExtent3D image_extent;
	VkFormat image_format;
};

namespace VkUtil
{
	void transition_image(VkCommandBuffer cmd, VkImage image, const VkImageLayout current_layout, const VkImageLayout new_layout);
	void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, const VkExtent2D src_size, const VkExtent2D dst_size);
}
