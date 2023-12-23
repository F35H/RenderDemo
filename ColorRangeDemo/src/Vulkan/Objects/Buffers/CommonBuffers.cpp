#include <Vulkan/Objects/Buffers/CommonBuffers.h>

using namespace CPUBuffer;
std::shared_ptr<ExternalProgram> externalProgram;

void Utility::ConfirmMemory(VkMemoryRequirements* memRequirements, VkMemoryPropertyFlags* allocProperties, VkMemoryAllocateInfo* allocInfo) {
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

//Buffer Factory
BufferFactory::BufferFactory(std::shared_ptr<ExternalProgram>* eProgram) {
  externalProgram = *eProgram;
}; //BufferFactory
void BufferFactory::AddFrameBuffer(std::vector<VkImageView> attachments, VkRenderPass renderPass, UINT scWidth, UINT scHeight) {
  frameBuffers.emplace_back(FrameBuffer(attachments, renderPass, scWidth, scHeight));
}; //AddFrameBuffer
void BufferFactory::AddUniformBuffers(UINT uniBuffNum, VkDescriptorSetLayout descSetLayout) {
  if (uniBuffNum == 0) return;
  if (!descPool) descPool = std::make_unique<DescPool>(DescPool(uniBuffNum, descSetLayout));
  uniformBuffers.emplace_back(
    UniformBuffer(descPool.value().get()->descSets[uniBuffNum - 1]));
  AddUniformBuffers(uniBuffNum - 1, descSetLayout);
}; //AddUniformBuffer
void BufferFactory::AddCmdBuffers(UINT cmdBuffNum) {
  if (!cmdPool) cmdPool = std::make_unique<CmdPool>(CmdPool());
  cmdPool.value().get()->cmdBuffers.resize(cmdBuffNum);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = cmdPool.value().get()->cmdPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(cmdPool.value().get()->cmdBuffers.size());

  auto result = vkAllocateCommandBuffers(externalProgram->device, &allocInfo, cmdPool.value().get()->cmdBuffers.data());
  errorHandler->ConfirmSuccess(result, "Allocating Command Buffers");
}; //AddCmdBUffers
std::pair<StageBuffer, ModelBuffer> BufferFactory::AddVerticeBuffer(Polytope* model) {
  StageBuffer(model->GetBufferSize(), model->vertices.data());
  vertexBuffers.push_back({
    StageBuffer(model->GetBufferSize(), model->vertices.data()),
    ModelBuffer(model->GetBufferSize(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) });
  return vertexBuffers[vertexBuffers.size() - 1];
}; //AddVertexBuffer
std::pair<StageBuffer, ModelBuffer> BufferFactory::AddIndiceBuffer(Polytope* model) {
  indexBuffers.push_back({
    StageBuffer(model->GetIndiceSize() * sizeof(uint32_t), model->indices.data()),
    ModelBuffer(model->GetIndiceSize() * sizeof(uint32_t),VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT) });
  return indexBuffers[indexBuffers.size() - 1];
}; //AddVertexBuffer
VkCommandBuffer BufferFactory::GetCommandBuffer(uint16_t indice) {
  return cmdPool.value().get()->cmdBuffers[indice];
}; //GetCommandBuffer
VkDescriptorSet BufferFactory::GetDescriptorSet(uint16_t indice) {
  return descPool.value().get()->descSets[indice];
}; //GetCommandBuffer


//FrameBuffer
FrameBuffer::FrameBuffer(std::vector<VkImageView> attachments, VkRenderPass renderPass, UINT scWidth, UINT scHeight) {
  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  framebufferInfo.pAttachments = attachments.data();
  framebufferInfo.width = scWidth;
  framebufferInfo.height = scHeight;
  framebufferInfo.layers = 1;

  VkResult result = vkCreateFramebuffer(externalProgram->device, &framebufferInfo, nullptr, &framebuffer);
  errorHandler->ConfirmSuccess(result, "Creating FrameBuffer");
}; //FrameBuffer

//UniformBuffer
UniformBuffer::UniformBuffer(VkDescriptorSet descSet) {
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
void UniformBuffer::CopyData(void* srcPtr) {
  memcpy(mappedBuffer, srcPtr, buffInfo.size);
}; //MapBuffer

//CmdPool
CmdPool::CmdPool() {
  sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  queueFamilyIndex = externalProgram->graphicsFamily.value();
  pNext = nullptr;

  auto result = vkCreateCommandPool(externalProgram->device, this, nullptr, &cmdPool);
  errorHandler->ConfirmSuccess(result, "Creating Command Pool");
}; //CmdPool

//DescPool
DescPool::DescPool(UINT uniBuffNum, VkDescriptorSetLayout descSetLayout) {
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

//ModelBuffer
ModelBuffer::ModelBuffer(size_t buffSize, INT buffFlags) {
  buffInfo.size = buffSize;
  buffInfo.usage = buffFlags;
  buffInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
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

//StageBuffer
StageBuffer::StageBuffer(size_t buffSize, void* srcPtr) {
  buffInfo.size = buffSize;
  buffInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  buffInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
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
  memcpy(mappedBuffer, srcPtr, buffInfo.size);
  vkUnmapMemory(externalProgram->device, bufferMemory);
}; //StageBuffer

//GeneralBuffer
const VkBuffer* GeneralBuffer::GetBuffer() {
  return &buffer;
}; //GetBuffer
uint32_t GeneralBuffer::GetSize() {
  return buffInfo.size;
}; //GetBuffer
GeneralBuffer::GeneralBuffer() {
  buffInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
}; //GeneralBuffer
void GeneralBuffer::CreateBuffer() {
  auto result = vkCreateBuffer(externalProgram->device, &buffInfo, nullptr, &buffer);
  errorHandler->ConfirmSuccess(result, "Creating Buffer on Device");
}; //CreateBuffer
void GeneralBuffer::ConfirmMemoryType() {
  Utility::ConfirmMemory(&memRequirements, &allocProperties, &allocInfo);
}; //ConfirmMemoryType