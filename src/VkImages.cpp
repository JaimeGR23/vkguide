#include <VkImages.h>

#include <VkInitializers.h>

void VkUtil::copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, const VkExtent2D src_size, const VkExtent2D dst_size)
{
	// A blit copies a region and can scale it and convert the format (RGBA16F -> BRGA9 here)
	VkImageBlit2 blit_region = {};
	blit_region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	blit_region.pNext = nullptr;

	// offsets[0] is the top-left corner (0,0,0); offsets[1] the bottom-right one
	blit_region.srcOffsets[1].x = static_cast<int32>(src_size.width);
	blit_region.srcOffsets[1].y = static_cast<int32>(src_size.height);
	blit_region.srcOffsets[1].z = 1;

	blit_region.dstOffsets[1].x = static_cast<int32>(dst_size.width);
	blit_region.dstOffsets[1].y = static_cast<int32>(dst_size.height);
	blit_region.dstOffsets[1].z = 1;

	blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blit_region.srcSubresource.baseArrayLayer = 0;
	blit_region.srcSubresource.layerCount = 1;
	blit_region.srcSubresource.mipLevel = 0;

	blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blit_region.dstSubresource.baseArrayLayer = 0;
	blit_region.dstSubresource.layerCount = 1;
	blit_region.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blit_info = {};
	blit_info.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	blit_info.pNext = nullptr;
	blit_info.srcImage = source;
	blit_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blit_info.dstImage = destination;
	blit_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blit_info.filter = VK_FILTER_LINEAR;
	blit_info.regionCount = 1;
	blit_info.pRegions = &blit_region;

	vkCmdBlitImage2(cmd, &blit_info);
}

void VkUtil::transition_image(VkCommandBuffer cmd, VkImage image, const VkImageLayout current_layout, const VkImageLayout new_layout)
{
	// ALL_COMMANDS on both sides is the simlpest correct barrier: it waits for everything.
	// Fine for a fer barriers per frame, a real renderer narrows the states.
	VkImageMemoryBarrier2 image_barrier = {};
	image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	image_barrier.pNext = nullptr;
	image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
	image_barrier.oldLayout = current_layout;
	image_barrier.newLayout = new_layout;

	const VkImageAspectFlags aspect_mask = (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
		? VK_IMAGE_ASPECT_DEPTH_BIT
		: VK_IMAGE_ASPECT_COLOR_BIT;
	image_barrier.subresourceRange = VkInit::image_subresource_range(aspect_mask);
	image_barrier.image = image;

	VkDependencyInfo dependency_info = {};
	dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency_info.pNext = nullptr;
	dependency_info.imageMemoryBarrierCount = 1;
	dependency_info.pImageMemoryBarriers = &image_barrier;

	vkCmdPipelineBarrier2(cmd, &dependency_info);
}
