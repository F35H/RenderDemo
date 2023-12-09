#pragma once

#include "Vulkan/Objects/Buffers/GPUBuffer.h"

#include <utility>
#include <string>
#include <vector>

namespace CPUBuffer {

  inline void ConfirmMemory(VkMemoryRequirements* memRequirements, VkMemoryPropertyFlags* allocProperties, VkMemoryAllocateInfo* allocInfo) {
    

    uint32_t i = 0;
    for (; i <= externalProgram->memProperties.memoryTypeCount; ++i) {
      if ((memRequirements->memoryTypeBits & (1 << i)) && (externalProgram->memProperties.memoryTypes[i].propertyFlags & *allocProperties) == *allocProperties) {
        allocInfo->memoryTypeIndex = i;
        break;
      } //if (typeFilter)

      if (i == externalProgram->memProperties.memoryTypeCount) {
        throw std::runtime_error("failed to find suitable memory type!");
      }; //if i == memProperties
    } //for uint32_t
  }; //ConfirmMemoryType

  struct GeneralBuffer {
  protected:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void* mappedBuffer;

    VkMemoryRequirements memRequirements = {};
    VkMemoryPropertyFlags allocProperties = {};
    VkMemoryAllocateInfo allocInfo = {};
    
    VkBufferCreateInfo buffInfo = {};

    GeneralBuffer() {
      buffInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      buffInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }; //GeneralBuffer

    void CreateBuffer() {
      auto result = vkCreateBuffer(externalProgram->device, &buffInfo, nullptr, &buffer);
      errorHandler->ConfirmSuccess(result, "Creating Buffer on Device");
    }; //CreateBuffer

    void ConfirmMemoryType() {
      ConfirmMemory(&memRequirements, &allocProperties, &allocInfo);
    }; //ConfirmMemoryType
  }; //Buffer

  struct TextureBuffer {

  }; //TextureBuffer

  struct StageBuffer : GeneralBuffer {
    StageBuffer(size_t buffSize) {
      buffInfo.size = buffSize;
      buffInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      auto result = vkCreateBuffer(externalProgram->device, &buffInfo, nullptr, &buffer);
      errorHandler->ConfirmSuccess(result, "Creating Stage Buffer");
      vkGetBufferMemoryRequirements(externalProgram->device, buffer, &memRequirements);
      allocProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memRequirements.size;
      ConfirmMemoryType();
      result = vkAllocateMemory(externalProgram->device, &allocInfo, nullptr, &bufferMemory);
      errorHandler->ConfirmSuccess(result, "Allocating Stage Buffer Memory");
      vkBindBufferMemory(externalProgram->device, buffer, bufferMemory, 0);
      vkMapMemory(externalProgram->device, bufferMemory, 0, buffSize, 0, &mappedBuffer);
    }; //StageBuffer

    void CopyData(void* srcPtr) {
      memcpy(mappedBuffer, srcPtr, buffInfo.size);
      vkUnmapMemory(externalProgram->device, bufferMemory);
    }; //MapBuffer
  }; //Stage Buffer

  struct ModelBuffer : GeneralBuffer {
    
    ModelBuffer(size_t buffSize, INT buffFlags) {
      buffInfo.size = buffSize;
      buffInfo.usage = buffFlags;
      CreateBuffer();
      vkGetBufferMemoryRequirements(externalProgram->device, buffer, &memRequirements);
      allocProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memRequirements.size;
      ConfirmMemoryType();
      VkResult result = vkAllocateMemory(externalProgram->device, &allocInfo, nullptr, &bufferMemory);
      errorHandler->ConfirmSuccess(result, "Allocating Model Buffer");
      vkBindBufferMemory(externalProgram->device, buffer, bufferMemory, 0);

    }; //Model Buffer

    VkBuffer GetBuffer() {
      return buffer;
    }; //GetBuffer
  }; //Model Buffer

  struct DescPool {
    VkDescriptorPoolCreateInfo descPoolInfo = {};
    VkDescriptorPoolSize descPoolSize = {};
  
    std::vector<VkDescriptorSet> descSets = {};
    VkDescriptorPool descPool = {};

    
    DescPool(UINT uniBuffNum, VkDescriptorSetLayout descSetLayout) {
      descSets.resize(uniBuffNum);

      //DescPoolSize
      descPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descPoolSize.descriptorCount = uniBuffNum;

      descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descPoolInfo.poolSizeCount = 1;
      descPoolInfo.pPoolSizes = &descPoolSize;
      descPoolInfo.maxSets = uniBuffNum;

      auto result = vkCreateDescriptorPool(externalProgram->device, &descPoolInfo, nullptr, &descPool);
      errorHandler->ConfirmSuccess(result, "Creating Descriptor Pool");

      std::vector<VkDescriptorSetLayout> layouts(uniBuffNum, descSetLayout);

      VkDescriptorSetAllocateInfo allocInfo = {};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descPool;
      allocInfo.descriptorSetCount = uniBuffNum;
      allocInfo.pSetLayouts = layouts.data();
      result = vkAllocateDescriptorSets(externalProgram->device, &allocInfo, descSets.data());
      errorHandler->ConfirmSuccess(result, "Allocating DescriptorSets");
    }; //DescriptorPool
  }; //DescPool

  struct CmdBuffer {
    VkCommandBuffer cmdBuffer;
  }; //CmdBuffer

  struct CmdPool: VkCommandPoolCreateInfo {
    std::vector<VkCommandBuffer> cmdBuffers;
    
    VkCommandPool cmdPool;

    CmdPool() {
      sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      queueFamilyIndex = externalProgram->graphicsFamily.value();
      pNext = nullptr;

      auto result = vkCreateCommandPool(externalProgram->device, this, nullptr, &cmdPool);
      errorHandler->ConfirmSuccess(result, "Creating Command Pool");
    }; //CmdPool
  }; //CmdPool

  struct UniformBuffer : GeneralBuffer {
    VkDescriptorSetLayout descSetLayout;

    UniformBuffer(VkDescriptorSet descSet) {
      buffInfo.size = sizeof(UniformBufferObject);
      buffInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

      CreateBuffer();

      vkGetBufferMemoryRequirements(externalProgram->device, buffer, &memRequirements);

      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memRequirements.size;

      allocProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

      ConfirmMemoryType();

      auto result = vkAllocateMemory(externalProgram->device, &allocInfo, nullptr, &bufferMemory);
      errorHandler->ConfirmSuccess(result, "Failed to Allocate Uniform Buffer Memory");

      vkBindBufferMemory(externalProgram->device, buffer, bufferMemory, 0);
      vkMapMemory(externalProgram->device, bufferMemory, 0, buffInfo.size, 0, &mappedBuffer);


      //Create DescriptorBuffer
      VkDescriptorBufferInfo descBuffInfo{};
      descBuffInfo.buffer = buffer;
      descBuffInfo.offset = 0;
      descBuffInfo.range = sizeof(UniformBufferObject);

      VkWriteDescriptorSet descriptorWrite{};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descSet;
      descriptorWrite.dstBinding = 0;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = &descBuffInfo;
      descriptorWrite.pImageInfo = nullptr; 
      descriptorWrite.pTexelBufferView = nullptr; 

      vkUpdateDescriptorSets(externalProgram->device, 1, &descriptorWrite, 0, nullptr);
    }; //UniformBuffer

    void CopyData(void* srcPtr) {
      memcpy(mappedBuffer, srcPtr, buffInfo.size);
    }; //MapBuffer
  }; //Uniform

  struct FrameBuffer {
    VkFramebuffer framebuffer;

    FrameBuffer(std::vector<VkImageView> attachments, VkRenderPass renderPass, UINT scWidth, UINT scHeight) {
      VkFramebufferCreateInfo framebufferInfo{};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = renderPass;
      framebufferInfo.attachmentCount = attachments.size();
      framebufferInfo.pAttachments = attachments.data();
      framebufferInfo.width = scWidth;
      framebufferInfo.height = scHeight;
      framebufferInfo.layers = 1;

      VkResult result = vkCreateFramebuffer(externalProgram->device, &framebufferInfo, nullptr, &framebuffer);
      errorHandler->ConfirmSuccess(result, "Creating FrameBuffer");
    }; //FrameBuffer
  }; //FrameBuffer

  struct BufferFactory {
    std::optional<std::unique_ptr<DescPool>> descPool;
    std::optional<std::unique_ptr<CmdPool>> cmdPool;
    
    std::vector<FrameBuffer> frameBuffers;
    std::vector<UniformBuffer> uniformBuffers;
    std::vector<ModelBuffer> vertexBuffers;
    std::vector<ModelBuffer> indexBuffers;

    void AddFrameBuffer(std::vector<VkImageView> attachments, VkRenderPass renderPass, UINT scWidth, UINT scHeight) {
      frameBuffers.emplace_back(FrameBuffer(attachments, renderPass, scWidth, scHeight));
    }; //AddFrameBuffer

    void AddUniformBuffers(UINT uniBuffNum, VkDescriptorSetLayout descSetLayout) {
      if (uniBuffNum == 0) return;
      if (!descPool) descPool = std::make_unique<DescPool>(DescPool(uniBuffNum, descSetLayout));
      uniformBuffers.emplace_back(
        UniformBuffer(descPool.value().get()->descSets[uniBuffNum-1]));
      AddUniformBuffers(uniBuffNum - 1, descSetLayout);
    }; //AddUniformBuffer

    void AddCmdBuffers(UINT cmdBuffNum) {
      

      if (!cmdPool) cmdPool = std::make_unique<CmdPool>(CmdPool());
      cmdPool.value().get()->cmdBuffers.resize(cmdBuffNum);
      
      VkCommandBufferAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      allocInfo.commandPool = cmdPool.value().get()->cmdPool;
      allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      allocInfo.commandBufferCount = cmdPool.value().get()->cmdBuffers.size();

      auto result = vkAllocateCommandBuffers(externalProgram->device, &allocInfo, cmdPool.value().get()->cmdBuffers.data());
      errorHandler->ConfirmSuccess(result, "Allocating Command Buffers");
    }; //AddCmdBUffers

    ModelBuffer AddVerticeBuffer(Polyhedra* model) {
      auto stageBuff = StageBuffer(model->GetBufferSize());
      stageBuff.CopyData(model->vertices.data());
      vertexBuffers.emplace_back(ModelBuffer(model->GetBufferSize(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
      return vertexBuffers[vertexBuffers.size()-1];
    }; //AddVertexBuffer

    ModelBuffer AddIndiceBuffer(Polyhedra* model) {
      auto stageBuff = StageBuffer(model->GetBufferSize());
      stageBuff.CopyData(model->indices.data());
      indexBuffers.emplace_back(ModelBuffer(model->GetBufferSize(),VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT));
      return indexBuffers[indexBuffers.size() - 1];
    }; //AddVertexBuffer
  }; //BufferCollection
}; //CPUBuffer