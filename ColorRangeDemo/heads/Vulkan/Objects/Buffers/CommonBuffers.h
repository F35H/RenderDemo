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
  struct TextureBuffer {

  }; //TextureBuffer
  struct StageBuffer : GeneralBuffer {
    StageBuffer(size_t buffSize, void* srcPtr);
  }; //Stage Buffer
  struct ModelBuffer : GeneralBuffer {
    bool cpyBool = false;
    ModelBuffer(size_t buffSize, INT buffFlags);
  }; //Model Buffer
  struct DescPool {
    VkDescriptorPoolCreateInfo descPoolInfo = {};
    VkDescriptorPoolSize descPoolSize = {};
    std::vector<VkDescriptorSet> descSets = {};
    VkDescriptorPool descPool = {};
    DescPool(UINT uniBuffNum, VkDescriptorSetLayout descSetLayout);
  }; //DescPool
  struct CmdBuffer {
    VkCommandBuffer cmdBuffer;
  }; //CmdBuffer
  struct CmdPool: VkCommandPoolCreateInfo {
    std::vector<VkCommandBuffer> cmdBuffers;
    VkCommandPool cmdPool;
    CmdPool();
  }; //CmdPool
  struct UniformBuffer : GeneralBuffer {
    VkDescriptorSetLayout descSetLayout;
    UniformBuffer(VkDescriptorSet descSet);
    void CopyData(void* srcPtr);
  }; //Uniform
  struct FrameBuffer {
    VkFramebuffer framebuffer;
    FrameBuffer(std::vector<VkImageView> attachments, VkRenderPass renderPass, UINT scWidth, UINT scHeight);
  }; //FrameBuffer
  struct BufferFactory {
    std::optional<std::unique_ptr<DescPool>> descPool;
    std::optional<std::unique_ptr<CmdPool>> cmdPool;
    
    std::vector<FrameBuffer> frameBuffers;
    std::vector<UniformBuffer> uniformBuffers;
    std::vector<std::pair<StageBuffer, ModelBuffer>> vertexBuffers;
    std::vector<std::pair<StageBuffer, ModelBuffer>> indexBuffers;

    BufferFactory(std::shared_ptr<ExternalProgram>* eProgram);
    void AddFrameBuffer(std::vector<VkImageView> attachments, VkRenderPass renderPass, UINT scWidth, UINT scHeight);
    void AddUniformBuffers(UINT uniBuffNum, VkDescriptorSetLayout descSetLayout);
    void AddCmdBuffers(UINT cmdBuffNum);
    std::pair<StageBuffer, ModelBuffer> AddVerticeBuffer(Polytope* model);
    std::pair<StageBuffer, ModelBuffer> AddIndiceBuffer(Polytope* model);
    VkCommandBuffer GetCommandBuffer(uint16_t indice);
    VkDescriptorSet GetDescriptorSet(uint16_t indice);
  }; //BufferCollection
}; //CPUBuffer