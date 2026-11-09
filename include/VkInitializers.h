#pragma once

#include <Types.h>
#include <Vulkan.h>

// Helpers that fill Vk*CreateInfo / Vk*Info structs with sensible defaults
namespace VkInit
{
    VkCommandPoolCreateInfo command_pool_create_info(const uint32 queue_family_index, const VkCommandPoolCreateFlags flags = 0);
    VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, const uint32 count = 1);

    VkCommandBufferBeginInfo command_buffer_begin_info(const VkCommandBufferUsageFlags flags = 0);

    VkFenceCreateInfo fence_create_info(const VkFenceCreateFlags flags = 0);
    VkSemaphoreCreateInfo semaphore_create_info(const VkSemaphoreCreateFlags flags = 0);

    VkSemaphoreSubmitInfo semaphore_submit_info(const VkPipelineStageFlags2 stage_mask, VkSemaphore semaphore);
    VkCommandBufferSubmitInfo command_buffer_submit_info(VkCommandBuffer cmd);
    VkSubmitInfo2 submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signal_semaphore_info,
        VkSemaphoreSubmitInfo* wait_semaphore_info);

    VkPresentInfoKHR present_info();

    VkRenderingAttachmentInfo attachment_info(VkImageView view, VkClearValue* clear,
        const VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);
    VkRenderingAttachmentInfo depth_attachment_info(VkImageView view,
        const VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);
    VkRenderingInfo rendering_info(const VkExtent2D render_extent, VkRenderingAttachmentInfo* color_attachment,
        VkRenderingAttachmentInfo* depth_attachment);

    VkImageSubresourceRange image_subresource_range(const VkImageAspectFlags aspect_mask);

    VkDescriptorSetLayoutBinding descriptorset_layout_binding(const VkDescriptorType type,
        const VkShaderStageFlags stage_flags, const uint32 binding);
    VkDescriptorSetLayoutCreateInfo descriptorset_layout_create_info(VkDescriptorSetLayoutBinding* bindings,
        const uint32 binding_count);
    VkWriteDescriptorSet write_descriptor_image(const VkDescriptorType type, VkDescriptorSet dst_set,
        VkDescriptorImageInfo* image_info, const uint32 binding);
    VkWriteDescriptorSet write_descriptor_buffer(const VkDescriptorType type, VkDescriptorSet dst_set,
        VkDescriptorBufferInfo* buffer_info, const uint32 binding);
    VkDescriptorBufferInfo buffer_info(VkBuffer buffer, const VkDeviceSize offset, const VkDeviceSize range);

    VkImageCreateInfo image_create_info(const VkFormat format, const VkImageUsageFlags usage_flags,
        const VkExtent3D extent);
    VkImageViewCreateInfo imageview_create_info(const VkFormat format, VkImage image,
        const VkImageAspectFlags aspect_flags);

    VkPipelineLayoutCreateInfo pipeline_layout_create_info();
    VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(const VkShaderStageFlagBits stage,
        VkShaderModule shader_module, const char* entry = "main");
}
