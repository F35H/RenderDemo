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
void BufferFactory::AddUniformBuffer(size_t uniBuffSize) {
  if (!descPool) descPool = std::make_unique<DescPool>(DescPool());
  auto ptr = new UniformBuffer(uniBuffSize);
  descPool.value().get()->uniBuffs.emplace_back(*ptr);
  uniformBuffers.push_back(ptr);
}; //AddUniformBuffer
void BufferFactory::AddImageBuffer(Texture* t) {
  if (!descPool) descPool = std::make_unique<DescPool>(DescPool());
  descPool.value().get()->imageBuffs.push_back(ImageBuffer(t->height,t->width));
  imageBuffers.push_back({
    new StageBuffer(t->imageSize,t->data),
    &descPool.value().get()->imageBuffs[descPool.value().get()->imageBuffs.size()-1]});
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
std::pair<StageBuffer*, ModelBuffer*> BufferFactory::AddVerticeBuffer(Polytope* model) {
  StageBuffer(model->GetBufferSize(), model->vertices.data());
  vertexBuffers.push_back({
    new StageBuffer(model->GetBufferSize(), model->vertices.data()),
    new ModelBuffer(model->GetBufferSize(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) });
  return vertexBuffers[vertexBuffers.size() - 1];
}; //AddVertexBuffer
std::pair<StageBuffer*, ModelBuffer*> BufferFactory::AddIndiceBuffer(Polytope* model) {
  indexBuffers.push_back({
    new StageBuffer(model->GetIndiceSize() * sizeof(uint32_t), model->indices.data()),
    new ModelBuffer(model->GetIndiceSize() * sizeof(uint32_t),VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT) });
  return indexBuffers[indexBuffers.size() - 1];
}; //AddVertexBuffer
VkCommandBuffer BufferFactory::GetCommandBuffer(uint_fast8_t indice) {
  return cmdPool.value().get()->cmdBuffers[indice];
}; //GetCommandBuffer
VkDescriptorSet* BufferFactory::GetDescriptorSet(uint_fast8_t indice) {
  return &descPool.value().get()->descSets[indice];
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
UniformBuffer::UniformBuffer(size_t bufferSize) {
  buffInfo.size = bufferSize;
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
void DescPool::Activate(uint32_t descNum, VkDescriptorSetLayout descSetLayout) {
  descSets.resize(descNum);
  
  //DescriptorPools
  std::vector<VkDescriptorPoolSize> poolSizes;
  poolSizes.resize(2);
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = descNum;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = descNum;

  descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descPoolInfo.poolSizeCount = poolSizes.size();
  descPoolInfo.pPoolSizes = poolSizes.data();
  descPoolInfo.maxSets = descNum;

  auto result = vkCreateDescriptorPool(externalProgram->device, &descPoolInfo, nullptr, &descPool);
  errorHandler->ConfirmSuccess(result, "Creating Descriptor Pool");

  //DescriptorSets
  std::vector<VkDescriptorSetLayout> layouts(descNum, descSetLayout);
  
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descPool;
  allocInfo.descriptorSetCount = descNum;
  allocInfo.pSetLayouts = layouts.data();
  result = vkAllocateDescriptorSets(externalProgram->device, &allocInfo, descSets.data());
  errorHandler->ConfirmSuccess(result, "Allocating DescriptorSets");

  //DescriptorWrite

  for (uint_fast8_t i = descNum-1; i < descNum; --i) {
    std::vector<VkWriteDescriptorSet> descWrites(2);
    std::pair<VkDescriptorBufferInfo, VkDescriptorImageInfo> descInfo;
    
    descInfo.first.buffer = *uniBuffs[i].GetBuffer();
    descInfo.first.offset = 0;
    descInfo.first.range = uniBuffs[i].GetSize();

    descInfo.second.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descInfo.second.imageView = imageBuffs[i].imageView;
    descInfo.second.sampler = imageBuffs[i].imageSampler;
    
    descWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descWrites[0].dstSet = descSets[i];
    descWrites[0].dstBinding = 0;
    descWrites[0].dstArrayElement = 0;
    descWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descWrites[0].descriptorCount = 1;
    descWrites[0].pBufferInfo = &descInfo.first;

    descWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descWrites[1].dstSet = descSets[i];
    descWrites[1].dstBinding = 1;
    descWrites[1].dstArrayElement = 0;
    descWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descWrites[1].descriptorCount = 1;
    descWrites[1].pImageInfo = &descInfo.second;

    vkUpdateDescriptorSets(externalProgram->device, static_cast<uint32_t>(descWrites.size()), descWrites.data(), 0, nullptr);
  }; //everyDescNum

}; //Activate

//ModelBuffer
ModelBuffer::ModelBuffer(size_t buffSize, uint32_t buffFlags) {
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
  writtenTo = true;
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

//ImageBuffer 
ImageBuffer::ImageBuffer(uint32_t h, uint32_t w) {
  VkImageCreateInfo texInfo;
  VkBufferImageCopy imageCpy;

  height = h;
  width = w;

  texInfo = {};
  texInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  texInfo.imageType = VK_IMAGE_TYPE_2D;
  texInfo.extent.width = width;
  texInfo.extent.height = height;
  texInfo.extent.depth = 1;
  texInfo.mipLevels = 1;
  texInfo.arrayLayers = 1;
  texInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
  texInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  texInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  texInfo.usage =
    VK_IMAGE_USAGE_SAMPLED_BIT |
    VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  texInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  texInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  texInfo.flags = 0;

  VkResult result = vkCreateImage(externalProgram->device, &texInfo, nullptr, &image);
  errorHandler->ConfirmSuccess(result, "Creating Texture Image");

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(externalProgram->device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  auto propFlag = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  Utility::ConfirmMemory(&memRequirements, &propFlag, &allocInfo);
  VkDeviceMemory memory;

  result = vkAllocateMemory(externalProgram->device, &allocInfo, nullptr, &memory);
  errorHandler->ConfirmSuccess(result, "Allocating Image Memory");

  vkBindImageMemory(externalProgram->device, image, memory, 0);


  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  vkCreateImageView(externalProgram->device, &viewInfo, nullptr, &imageView);


  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(externalProgram->physicalDevice, &properties);
  
  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  result = vkCreateSampler(externalProgram->device, &samplerInfo, nullptr, &imageSampler);
  errorHandler->ConfirmSuccess(result, "Create Texture Sampler");

  oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
}; //ImageBuffer