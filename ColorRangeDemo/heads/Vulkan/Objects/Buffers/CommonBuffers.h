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
    void CopyData(void* srcPtr);
    const VkBuffer* GetBuffer();
    uint32_t GetSize();
  }; //Buffer

  struct ImageBuffer {
    VkImage* image;
    VkImageView imageView;
    VkImageLayout oldLayout;
    VkImageLayout newLayout;
    VkDeviceMemory memory;
    uint32_t width;
    uint32_t height;
    bool readyToCpy = false;
    ImageBuffer() = default;
  }; //ImageBuffer
  struct ComputeImage : ImageBuffer {
    ComputeImage(uint32_t height, uint32_t width);
    void CreateImage(uint32_t h, uint32_t w);
    void Reset();
  }; //ComputeImage
  struct TextureImage : ImageBuffer {
    VkSampler imageSampler;

    bool fromStagedBuffer = true;
    TextureImage(uint32_t height, uint32_t width);
    void CreateImage(uint32_t h, uint32_t w);
    void Reset();
  }; //TextureBuffer
  struct UniformBuffer : public GeneralBuffer {
    UniformBuffer(size_t uniformBuffer);
  }; //Uniform
  struct StageBuffer : GeneralBuffer {
    bool writtenTo = false;
    StageBuffer(size_t buffSize, void* srcPtr);
    void Reset(size_t buffSize, void* srcPtr);
    void CreateBuffer(void* srcPtr);
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
    std::vector<std::pair<StageBuffer*, TextureImage*>> imageBuffs;
    std::vector<std::pair<StageBuffer*, ComputeImage*>> computeImages;

    void Activate(VkDescriptorSetLayout descSetLayout);
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
    std::vector<std::unique_ptr<DescPool>> descPools;
    std::optional<std::unique_ptr<CmdPool>> cmdPool;
    
    std::vector<FrameBuffer> frameBuffers;
    std::vector<std::pair<StageBuffer*, ModelBuffer*>> vertexBuffers;
    std::vector<std::pair<StageBuffer*, ModelBuffer*>> indexBuffers;

    BufferFactory(std::shared_ptr<ExternalProgram>* eProgram);
    void AddFrameBuffer(std::vector<VkImageView> attachments, VkRenderPass renderPass, UINT scWidth, UINT scHeight);
    void AddUniformBuffer(size_t uniBuffSize, uint_fast8_t descPool);
    void AddCmdBuffers(UINT cmdBuffNum);

    std::pair<StageBuffer*, ComputeImage*> AddComputeImage(uint32_t height, uint32_t width, uint_fast8_t descPool);
    std::pair<StageBuffer*, TextureImage*> AddTextureImage(Texture* t, uint_fast8_t descPool);
    std::pair<StageBuffer*, ModelBuffer*> AddVerticeBuffer(Polytope* model);
    std::pair<StageBuffer*, ModelBuffer*> AddIndiceBuffer(Polytope* model);

    VkCommandBuffer GetCommandBuffer(uint_fast8_t indice);
    VkDescriptorSet* GetDescriptorSet(uint_fast8_t indice, uint_fast8_t descPool);
    UniformBuffer* GetUniformBuffer(uint_fast8_t indice, uint_fast8_t descPool);
    std::pair<StageBuffer*, TextureImage*>* GetTextureImage(uint_fast8_t indice, uint_fast8_t descPool);
    std::pair<StageBuffer*, ComputeImage*>* GetComputeImage(uint_fast8_t indice, uint_fast8_t descPool);
  }; //BufferCollection
}; //CPUBuffer