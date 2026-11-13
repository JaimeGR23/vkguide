#pragma once

#include <Types.h>
#include <Vulkan.h>

namespace VkUtil
{
	void transition_image(VkCommandBuffer cmd, VkImage image, const VkImageLayout current_layout, const VkImageLayout new_layout);
}
