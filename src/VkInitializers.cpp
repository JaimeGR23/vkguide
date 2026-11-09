#include <VkInitializers.h>

VkCommandPoolCreateInfo VkInit::command_pool_create_info(const uint32 queue_family_index,
    const VkCommandPoolCreateFlags flags)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.queueFamilyIndex = queue_family_index;
    info.flags = flags;
    return info;
}

VkCommandBufferAllocateInfo VkInit::command_buffer_allocate_info(VkCommandPool pool, const uint32 count)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = pool;
    info.commandBufferCount = count;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return info;
}

VkCommandBufferBeginInfo VkInit::command_buffer_begin_info(const VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}

VkFenceCreateInfo VkInit::fence_create_info(const VkFenceCreateFlags flags)
{
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}

VkSemaphoreCreateInfo VkInit::semaphore_create_info(const VkSemaphoreCreateFlags flags)
{
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}

VkSemaphoreSubmitInfo VkInit::semaphore_submit_info(const VkPipelineStageFlags2 stage_mask, VkSemaphore semaphore)
{
    VkSemaphoreSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.semaphore = semaphore;
    submit_info.stageMask = stage_mask;
    submit_info.deviceIndex = 0;
    submit_info.value = 1;
    return submit_info;
}

VkCommandBufferSubmitInfo VkInit::command_buffer_submit_info(VkCommandBuffer cmd)
{
    VkCommandBufferSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.pNext = nullptr;
    info.commandBuffer = cmd;
    info.deviceMask = 0;
    return info;
}

VkSubmitInfo2 VkInit::submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signal_semaphore_info,
    VkSemaphoreSubmitInfo* wait_semaphore_info)
{
    VkSubmitInfo2 info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    info.pNext = nullptr;

    info.waitSemaphoreInfoCount = wait_semaphore_info == nullptr ? 0 : 1;
    info.pWaitSemaphoreInfos = wait_semaphore_info;

    info.signalSemaphoreInfoCount = signal_semaphore_info == nullptr ? 0 : 1;
    info.pSignalSemaphoreInfos = signal_semaphore_info;

    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;
    return info;
}

VkPresentInfoKHR VkInit::present_info()
{
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.pNext = nullptr;
    info.swapchainCount = 0;
    info.pSwapchains = nullptr;
    info.pWaitSemaphores = nullptr;
    info.waitSemaphoreCount = 0;
    info.pImageIndices = nullptr;
    return info;
}

VkRenderingAttachmentInfo VkInit::attachment_info(VkImageView view, VkClearValue* clear, const VkImageLayout layout)
{
    VkRenderingAttachmentInfo color_attachment = {};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.pNext = nullptr;
    color_attachment.imageView = view;
    color_attachment.imageLayout = layout;
    // A clear value means "clear on load"; without one the previous contents are kept
    color_attachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    if (clear)
    {
        color_attachment.clearValue = *clear;
    }
    return color_attachment;
}

VkRenderingAttachmentInfo VkInit::depth_attachment_info(VkImageView view, const VkImageLayout layout)
{
    VkRenderingAttachmentInfo depth_attachment = {};
    depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_attachment.pNext = nullptr;
    depth_attachment.imageView = view;
    depth_attachment.imageLayout = layout;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // Reversed depth: 0 is the far plane
    depth_attachment.clearValue.depthStencil.depth = 0.0f;
    return depth_attachment;
}

VkRenderingInfo VkInit::rendering_info(const VkExtent2D render_extent, VkRenderingAttachmentInfo* color_attachment,
    VkRenderingAttachmentInfo* depth_attachment)
{
    VkRenderingInfo render_info = {};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.pNext = nullptr;
    render_info.renderArea = VkRect2D{ VkOffset2D{ 0, 0 }, render_extent };
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = color_attachment;
    render_info.pDepthAttachment = depth_attachment;
    render_info.pStencilAttachment = nullptr;
    return render_info;
}

VkImageSubresourceRange VkInit::image_subresource_range(const VkImageAspectFlags aspect_mask)
{
    VkImageSubresourceRange sub_image = {};
    sub_image.aspectMask = aspect_mask;
    sub_image.baseMipLevel = 0;
    sub_image.levelCount = VK_REMAINING_MIP_LEVELS;
    sub_image.baseArrayLayer = 0;
    sub_image.layerCount = VK_REMAINING_ARRAY_LAYERS;
    return sub_image;
}

VkDescriptorSetLayoutBinding VkInit::descriptorset_layout_binding(const VkDescriptorType type,
    const VkShaderStageFlags stage_flags, const uint32 binding)
{
    VkDescriptorSetLayoutBinding set_binding = {};
    set_binding.binding = binding;
    set_binding.descriptorCount = 1;
    set_binding.descriptorType = type;
    set_binding.pImmutableSamplers = nullptr;
    set_binding.stageFlags = stage_flags;
    return set_binding;
}

VkDescriptorSetLayoutCreateInfo VkInit::descriptorset_layout_create_info(VkDescriptorSetLayoutBinding* bindings,
    const uint32 binding_count)
{
    VkDescriptorSetLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.pBindings = bindings;
    info.bindingCount = binding_count;
    info.flags = 0;
    return info;
}

VkWriteDescriptorSet VkInit::write_descriptor_image(const VkDescriptorType type, VkDescriptorSet dst_set,
    VkDescriptorImageInfo* image_info, const uint32 binding)
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstBinding = binding;
    write.dstSet = dst_set;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pImageInfo = image_info;
    return write;
}

VkWriteDescriptorSet VkInit::write_descriptor_buffer(const VkDescriptorType type, VkDescriptorSet dst_set,
    VkDescriptorBufferInfo* buffer_info, const uint32 binding)
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstBinding = binding;
    write.dstSet = dst_set;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pBufferInfo = buffer_info;
    return write;
}

VkDescriptorBufferInfo VkInit::buffer_info(VkBuffer buffer, const VkDeviceSize offset, const VkDeviceSize range)
{
    VkDescriptorBufferInfo info = {};
    info.buffer = buffer;
    info.offset = offset;
    info.range = range;
    return info;
}

VkImageCreateInfo VkInit::image_create_info(const VkFormat format, const VkImageUsageFlags usage_flags,
    const VkExtent3D extent)
{
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = format;
    info.extent = extent;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    // No MSAA by default
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    // Optimal tiling lets the driver pick the GPU-friendly memory layout (not readable from the CPU)
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usage_flags;
    return info;
}

VkImageViewCreateInfo VkInit::imageview_create_info(const VkFormat format, VkImage image,
    const VkImageAspectFlags aspect_flags)
{
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = aspect_flags;
    return info;
}

VkPipelineLayoutCreateInfo VkInit::pipeline_layout_create_info()
{
    VkPipelineLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.setLayoutCount = 0;
    info.pSetLayouts = nullptr;
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges = nullptr;
    return info;
}

VkPipelineShaderStageCreateInfo VkInit::pipeline_shader_stage_create_info(const VkShaderStageFlagBits stage,
    VkShaderModule shader_module, const char* entry)
{
    VkPipelineShaderStageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.stage = stage;
    info.module = shader_module;
    info.pName = entry;
    return info;
}
