#include <Polly/Graphics/Vulkan/VulkanPsoCache.hpp>

#include <Polly/Graphics/Vulkan/VulkanPainter.hpp>
#include <Polly/Logging.hpp>

namespace Polly
{
VulkanPsoCache::VulkanPsoCache(VulkanPainter& painter)
    : _painter(painter)
{
}

VulkanPsoCache::~VulkanPsoCache() noexcept
{
    logVerbose("Destroying VulkanPsoCache");
}

VkPipeline VulkanPsoCache::get(const Key& entry)
{
    auto cacheEntry = _cache.find(entry);

    if (not cacheEntry)
    {
        assume(entry.vkVsModule != VK_NULL_HANDLE);
        assume(entry.vkPsModule != VK_NULL_HANDLE);
        assume(entry.vkPipelineLayout != VK_NULL_HANDLE);
        assume(entry.vkRenderPass != VK_NULL_HANDLE);

        logVerbose("Creating VkPipeline");

        const auto& inputElements = entry.inputElements;

        auto vertexBindingDesc      = VkVertexInputBindingDescription();
        vertexBindingDesc.binding   = 0;
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        auto attributeDescs = List<VkVertexInputAttributeDescription>();
        {
            auto idx    = static_cast<u32>(0);
            auto offset = static_cast<u32>(0);

            for (const auto& element : inputElements)
            {
                const auto& [format, sizeInBytes] = convert(element);

                auto desc     = VkVertexInputAttributeDescription();
                desc.binding  = 0;
                desc.location = idx;
                desc.format   = format;
                desc.offset   = offset;
                attributeDescs.add(desc);

                offset += static_cast<u32>(sizeInBytes);
                ++idx;
            }

            vertexBindingDesc.stride = offset;
        }

        auto vertexInputInfo  = VkPipelineVertexInputStateCreateInfo();
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount   = 1;
        vertexInputInfo.pVertexBindingDescriptions      = &vertexBindingDesc;
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescs.size();
        vertexInputInfo.pVertexAttributeDescriptions    = attributeDescs.data();

        auto vsStageInfo   = VkPipelineShaderStageCreateInfo();
        vsStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vsStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vsStageInfo.module = entry.vkVsModule;
        vsStageInfo.pName  = "main";

        auto psStageInfo   = VkPipelineShaderStageCreateInfo();
        psStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        psStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        psStageInfo.module = entry.vkPsModule;
        psStageInfo.pName  = "main";

        const auto     shaderStages  = Array{vsStageInfo, psStageInfo};
        constexpr auto dynamicStates = Array{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        auto viewportStateInfo          = VkPipelineViewportStateCreateInfo();
        viewportStateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.scissorCount  = 1;

        auto dynamicStateInfo              = VkPipelineDynamicStateCreateInfo();
        dynamicStateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount = u32(dynamicStates.size());
        dynamicStateInfo.pDynamicStates    = dynamicStates.data();

        auto inputAssemblyInfo     = VkPipelineInputAssemblyStateCreateInfo();
        inputAssemblyInfo.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = entry.vkPrimitiveTopology;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        auto rasterizerInfo             = VkPipelineRasterizationStateCreateInfo();
        rasterizerInfo.sType            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerInfo.depthClampEnable = VK_FALSE;
        rasterizerInfo.polygonMode      = VK_POLYGON_MODE_FILL;
        rasterizerInfo.lineWidth        = 1.0f;
        rasterizerInfo.cullMode         = VK_CULL_MODE_NONE;
        rasterizerInfo.frontFace        = VK_FRONT_FACE_CLOCKWISE;

        auto multisamplingInfo                 = VkPipelineMultisampleStateCreateInfo();
        multisamplingInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisamplingInfo.sampleShadingEnable  = VK_FALSE;
        multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisamplingInfo.minSampleShading     = 1.0f;

        const auto blendState = entry.blendState;

        auto colorBlendAttachmentInfo                = VkPipelineColorBlendAttachmentState();
        colorBlendAttachmentInfo.colorWriteMask      = convert(blendState.colorWriteMask);
        colorBlendAttachmentInfo.blendEnable         = blendState.isBlendingEnabled;
        colorBlendAttachmentInfo.srcColorBlendFactor = convert(blendState.colorSrcBlend);
        colorBlendAttachmentInfo.dstColorBlendFactor = convert(blendState.colorDstBlend);
        colorBlendAttachmentInfo.colorBlendOp        = convert(blendState.colorBlendFunction);
        colorBlendAttachmentInfo.srcAlphaBlendFactor = convert(blendState.alphaSrcBlend);
        colorBlendAttachmentInfo.dstAlphaBlendFactor = convert(blendState.alphaDstBlend);
        colorBlendAttachmentInfo.alphaBlendOp        = convert(blendState.alphaBlendFunction);

        auto blendStateInfo              = VkPipelineColorBlendStateCreateInfo();
        blendStateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendStateInfo.logicOpEnable     = VK_FALSE;
        blendStateInfo.logicOp           = VK_LOGIC_OP_COPY;
        blendStateInfo.attachmentCount   = 1;
        blendStateInfo.pAttachments      = &colorBlendAttachmentInfo;
        blendStateInfo.blendConstants[0] = blendState.blendFactor.r;
        blendStateInfo.blendConstants[1] = blendState.blendFactor.g;
        blendStateInfo.blendConstants[2] = blendState.blendFactor.b;
        blendStateInfo.blendConstants[3] = blendState.blendFactor.a;

        auto pipelineInfo                = VkGraphicsPipelineCreateInfo();
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount          = static_cast<u32>(shaderStages.size());
        pipelineInfo.pStages             = shaderStages.data();
        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pRasterizationState = &rasterizerInfo;
        pipelineInfo.pMultisampleState   = &multisamplingInfo;
        pipelineInfo.pColorBlendState    = &blendStateInfo;
        pipelineInfo.pDynamicState       = &dynamicStateInfo;
        pipelineInfo.pViewportState      = &viewportStateInfo;
        pipelineInfo.layout              = entry.vkPipelineLayout;

        pipelineInfo.renderPass = entry.vkRenderPass;
        pipelineInfo.subpass    = 0;

        const auto vkDevice   = _painter.vkDevice();
        auto       vkPipeline = VkPipeline();

        checkVkResult(
            vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkPipeline),
            "Failed to create a Vulkan pipeline object.");

        logVerbose("Created VkPipeline 0x{}", uintptr_t(vkPipeline));

        cacheEntry = _cache.add(entry, PipelineValue(vkDevice, vkPipeline))->second;
    }

    return cacheEntry->vkPipeline();
}

void VulkanPsoCache::notifyVkShaderModuleAboutToBeDestroyed(VkShaderModule mod)
{
    _cache.removeWhere(
        [mod](const auto& pair)
        {
            const auto& entry  = pair.first;
            const auto  result = entry.vkVsModule == mod or entry.vkPsModule == mod;

            if (result)
            {
                logVerbose("Destroying VkPipeline 0x{}", pair.second.vkPipeline());
            }

            return result;
        });
}

void VulkanPsoCache::clear()
{
    logVerbose("Clearing VulkanPsoCache");
    _cache.clear();
}

VulkanPsoCache::PipelineValue::PipelineValue(VkDevice vkDevice, VkPipeline vkPipeline)
    : _vkDevice(vkDevice)
    , _vkPipeline(vkPipeline)
{
}

VulkanPsoCache::PipelineValue::PipelineValue(PipelineValue&& moveFrom) noexcept
    : _vkDevice(moveFrom._vkDevice)
    , _vkPipeline(moveFrom._vkPipeline)
{
    moveFrom._vkDevice   = VK_NULL_HANDLE;
    moveFrom._vkPipeline = VK_NULL_HANDLE;
}

VulkanPsoCache::PipelineValue& VulkanPsoCache::PipelineValue::operator=(PipelineValue&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        _vkDevice   = moveFrom._vkDevice;
        _vkPipeline = moveFrom._vkPipeline;

        moveFrom._vkDevice   = VK_NULL_HANDLE;
        moveFrom._vkPipeline = VK_NULL_HANDLE;
    }

    return *this;
}

VulkanPsoCache::PipelineValue::~PipelineValue() noexcept
{
    destroy();
}

VkPipeline VulkanPsoCache::PipelineValue::vkPipeline() const
{
    return _vkPipeline;
}

void VulkanPsoCache::PipelineValue::destroy()
{
    if (_vkPipeline != VK_NULL_HANDLE)
    {
        assume(_vkDevice != VK_NULL_HANDLE);
        vkDestroyPipeline(_vkDevice, _vkPipeline, nullptr);
        _vkPipeline = VK_NULL_HANDLE;
    }
}
} // namespace Polly
