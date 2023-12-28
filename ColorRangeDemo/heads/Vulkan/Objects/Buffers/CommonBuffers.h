#pragma once

#include "Vulkan/Objects/Buffers/UniformData.h"

#include <utility>
#include <string>
#include <vector>

namespace CPUBuffer {
  namespace Utility {
    void ConfirmMemory(VkMemoryRequirements* memRequirements, VkMemoryPropertyFlags* allocProperties, VkMemoryAllocateInfo* allocInfo);
  }; //Utiltiy


  struct GeneralBuffer {
  protected:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void* mappedBuffer;
    VkMemoryRequirements memRequirements = {};
    VkMemoryPropertyFlags allocProperties = {};
    VkMemoryAllocateInfo allocInfo = {};
    VkBufferCreateInfo buffInfo = {};
    GeneralBuffer();
    void CreateBuffer();
    void ConfirmMemoryType();
  public:
    const VkBuffer* GetBuffer();
    uint32_t GetSize();
  }; //Buffer
  struct ImageBuffer {
    VkSampler imageSampler;
    VkDeviceMemory memory;
    VkImageLayout oldLayout;
    VkImageLayout newLayout;
    VkImage image;
    VkImageView imageView;
    bool fromStagedBuffer = true;
    uint32_t width;
    uint32_t height;
    ImageBuffer(uint32_t height, uint32_t width);
  }; //TextureBuffer
  struct UniformBuffer : public GeneralBuffer {
    UniformBuffer(size_t uniformBuffer);
    void CopyData(void* srcPtr);
  }; //Uniform
  struct StageBuffer : GeneralBuffer {
    bool writtenTo = false;
    StageBuffer(size_t buffSize, void* srcPtr);
  }; //Stage Buffer
  struct ModelBuffer : GeneralBuffer {
    bool fromStagedBuffer = false;
    ModelBuffer(size_t buffSize, uint32_t buffFlags);
  }; //Model Buffer
  struct DescPool {
    VkDescriptorPoolCreateInfo descPoolInfo = {};
    VkDescriptorPoolSize descPoolSize = {};
    std::vector<VkDescriptorSet> descSets = {};
    VkDescriptorPool descPool = {};

    std::vector<UniformBuffer> uniBuffs;
    std::vector<ImageBuffer> imageBuffs;

    void Activate(uint32_t descNum, VkDescriptorSetLayout descSetLayout);
  }; //DescPool
  struct CmdBuffer {
    VkCommandBuffer cmdBuffer;
  }; //CmdBuffer
  struct CmdPool: VkCommandPoolCreateInfo {
    std::vector<VkCommandBuffer> cmdBuffers;
    VkCommandPool cmdPool;
    CmdPool();
  }; //CmdPool
  struct FrameBuffer {
    VkFramebuffer framebuffer;
    FrameBuffer(std::vector<VkImageView> attachments, VkRenderPass renderPass, UINT scWidth, UINT scHeight);
  }; //FrameBuffer
  struct BufferFactory {
    std::optional<std::unique_ptr<DescPool>> descPool;
    std::optional<std::unique_ptr<CmdPool>> cmdPool;
    
    std::vector<FrameBuffer> frameBuffers;
    std::vector<UniformBuffer*> uniformBuffers;
    std::vector<std::pair<StageBuffer*, ModelBuffer*>> vertexBuffers;
    std::vector<std::pair<StageBuffer*, ModelBuffer*>> indexBuffers;
    std::vector<std::pair<StageBuffer*, ImageBuffer*>> imageBuffers;

    BufferFactory(std::shared_ptr<ExternalProgram>* eProgram);
    void AddFrameBuffer(std::vector<VkImageView> attachments, VkRenderPass renderPass, UINT scWidth, UINT scHeight);
    void AddUniformBuffer(size_t uniBuffSize);
    void AddImageBuffer(Texture* t);
    void AddCmdBuffers(UINT cmdBuffNum);
    std::pair<StageBuffer*, ModelBuffer*> AddVerticeBuffer(Polytope* model);
    std::pair<StageBuffer*, ModelBuffer*> AddIndiceBuffer(Polytope* model);
    VkCommandBuffer GetCommandBuffer(uint_fast8_t indice);
    VkDescriptorSet* GetDescriptorSet(uint_fast8_t indice);
  }; //BufferCollection
}; //CPUBuffer