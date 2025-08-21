#pragma once

#include <compare>
#include <Polly/BlendState.hpp>
#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/Vulkan/VulkanImageAndViewPair.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>
#include <Polly/List.hpp>
#include <Polly/SortedMap.hpp>

enum class VertexElement;

namespace Polly
{
class VulkanPainter;
class GraphicsResource;

class VulkanPsoCache final
{
  public:
    struct Key
    {
        VkShaderModule         vkVsModule = VK_NULL_HANDLE;
        VkShaderModule         vkPsModule = VK_NULL_HANDLE;
        BlendState             blendState{};
        VkPrimitiveTopology    vkPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkPipelineLayout       vkPipelineLayout{};
        VkRenderPass           vkRenderPass{};
        List<VertexElement, 4> inputElements;

        defineDefaultEqualityOperations(Key);
    };

    explicit VulkanPsoCache(VulkanPainter& parentDevice);

    deleteCopyAndMove(VulkanPsoCache);

    ~VulkanPsoCache() noexcept;

    VkPipeline get(const Key& entry);

    void notifyVkShaderModuleAboutToBeDestroyed(VkShaderModule mod);

    void clear();

  private:
    class PipelineValue final
    {
      public:
        explicit PipelineValue(VkDevice vkDevice, VkPipeline vkPipeline);

        deleteCopy(PipelineValue);

        PipelineValue(PipelineValue&& moveFrom) noexcept;
        PipelineValue& operator=(PipelineValue&& moveFrom) noexcept;

        ~PipelineValue() noexcept;

        VkPipeline vkPipeline() const;

      private:
        void destroy();

        VkDevice   _vkDevice   = VK_NULL_HANDLE;
        VkPipeline _vkPipeline = VK_NULL_HANDLE;
    };

    VulkanPainter&         _parentDevice;
    SortedMap<Key, PipelineValue> _cache;
};
} // namespace Polly