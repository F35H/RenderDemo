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
void BufferFactory::AddUniformBuffer(size_t uniBuffSize, uint_fast8_t descPool) {
  if (descPools.size() <= descPool) { descPools.resize(descPool + 1); descPools[descPool] = std::make_unique<DescPool>(DescPool()); };
  auto ptr = new UniformBuffer(uniBuffSize);
  descPools[descPool]->uniBuffs.push_back(*ptr);
}; //AddUniformBuffer
std::pair<StageBuffer*, TextureImage*> BufferFactory::AddTextureImage(Texture* t, uint_fast8_t descPool) {
  if (descPools.size() <= descPool) { descPools.resize(descPool + 1); descPools[descPool] = std::make_unique<DescPool>(DescPool()); };
  descPools[descPool]->imageBuffs.push_back({
    new StageBuffer(t->imageSize,t->data),
    new TextureImage(t->height,t->width) 
  }); //AddTextureImage
  return descPools[descPool]->imageBuffs[descPools[descPool]->imageBuffs.size() - 1];
}; //AddTextureImage
std::pair<StageBuffer*, ComputeImage*> BufferFactory::AddComputeImage(uint32_t height, uint32_t width, uint_fast8_t descPool) {
  if (descPools.size() <= descPool) { descPools.resize(descPool + 1); descPools[descPool] = std::make_unique<DescPool>(DescPool()); };
  descPools[descPool]->computeImages.push_back({
    new StageBuffer(height * width * 4,nullptr),
    new ComputeImage(height,width)
    }); //AddTextureImage
  return descPools[descPool]->computeImages[descPools[descPool]->computeImages.size()-1];
}; //AddTextureImage
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
    new StageBuffer(model->GetIndiceSize() * sizeof(model->indices[0]), model->indices.data()),
    new ModelBuffer(model->GetIndiceSize() * sizeof(model->indices[0]),VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT) });
  return indexBuffers[indexBuffers.size() - 1];
}; //AddVertexBuffer
VkCommandBuffer BufferFactory::GetCommandBuffer(uint_fast8_t indice) {
  return cmdPool.value().get()->cmdBuffers[indice];
}; //GetCommandBuffer
VkDescriptorSet* BufferFactory::GetDescriptorSet(uint_fast8_t indice, uint_fast8_t descPool) {
  return &descPools[descPool]->descSets[indice];
}; //GetCommandBuffer
UniformBuffer* BufferFactory::GetUniformBuffer(uint_fast8_t indice, uint_fast8_t descPool) {
  return &descPools[descPool]->uniBuffs[indice];
}; //GetCommandBuffer
std::pair<StageBuffer*, TextureImage*>* BufferFactory::GetTextureImage(uint_fast8_t indice, uint_fast8_t descPool) {
  return &descPools[descPool]->imageBuffs[indice];
}; //GetCommandBuffer
std::pair<StageBuffer*, ComputeImage*>* BufferFactory::GetComputeImage(uint_fast8_t indice, uint_fast8_t descPool) {
  return &descPools[descPool]->computeImages[indice];
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
void DescPool::Activate(VkDescriptorSetLayout descSetLayout) {
  uint32_t descNum = computeImages.size() + uniBuffs.size() + imageBuffs.size();
  descSets.resize(descNum);
  
  //DescriptorPools
  std::vector<VkDescriptorPoolSize> poolSizes;

  if (uniBuffs.size() > 0) {
    VkDescriptorPoolSize pSize{};
    pSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pSize.descriptorCount = uniBuffs.size() * sizeof(UniformBufferObject);
    poolSizes.push_back(pSize);
  } //uniBuffSize
  if (imageBuffs.size() > 0) {
    VkDescriptorPoolSize pSize{};
    pSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pSize.descriptorCount = imageBuffs.size() * sizeof(VkImage);;
    poolSizes.push_back(pSize);
  } //uniBuffSize
  if (computeImages.size() > 0) {
    VkDescriptorPoolSize pSize{};
    pSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    pSize.descriptorCount = computeImages.size() * 2;
    poolSizes.push_back(pSize);
  } //uniBuffSize

  descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descPoolInfo.poolSizeCount = poolSizes.size();
  descPoolInfo.pPoolSizes = poolSizes.data();
  descPoolInfo.maxSets = descNum;
  descPoolInfo.flags = 0;

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
    std::vector<VkWriteDescriptorSet> descWrites;
    
    if (uniBuffs.size() > i) {
      VkDescriptorBufferInfo buffInfo{};
      buffInfo.buffer = *uniBuffs[i].GetBuffer();
      buffInfo.offset = 0;
      buffInfo.range = uniBuffs[i].GetSize();

      VkWriteDescriptorSet set{};
      set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      set.dstSet = descSets[i];
      set.dstBinding = 0;
      set.dstArrayElement = 0;
      set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      set.descriptorCount = 1;
      set.pBufferInfo = &buffInfo;
      descWrites.push_back(set);
    }; //if Size > 0

    if (imageBuffs.size() > i) {
      VkDescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = imageBuffs[i].second->imageView;
      imageInfo.sampler = imageBuffs[i].second->imageSampler;

      VkWriteDescriptorSet set{};
      set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      set.dstSet = descSets[i];
      set.dstBinding = 1;
      set.dstArrayElement = 0;
      set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      set.descriptorCount = 1;
      set.pImageInfo = &imageInfo;
      descWrites.push_back(set);
    }; //If Size is  > 0

    if (computeImages.size() > i) {
      VkDescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageInfo.imageView = computeImages[i].second->imageView;

      VkWriteDescriptorSet set{};
      set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      set.dstSet = descSets[i];
      set.dstBinding = 1;
      set.dstArrayElement = 0;
      set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      set.pImageInfo = &imageInfo;
      descWrites.push_back(set);
    }; //If Size is  > 0

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
  CreateBuffer(srcPtr);
}; //StageBuffer
void StageBuffer::CreateBuffer(void* srcPtr) {
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
  vkMapMemory(externalProgram->device, bufferMemory, 0, buffInfo.size, 0, &mappedBuffer);
  memcpy(mappedBuffer, srcPtr, buffInfo.size);
  vkUnmapMemory(externalProgram->device, bufferMemory);
  writtenTo = true;
}; //StageBufferCreateBuffer

void StageBuffer::Reset(size_t buffSize, void* srcPtr) {
  vkDestroyBuffer(externalProgram->device, buffer, nullptr);
  vkFreeMemory(externalProgram->device, bufferMemory, nullptr);
  buffInfo.size = buffSize;
  CreateBuffer(srcPtr);
}; //Reset

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
void GeneralBuffer::CopyData(void* srcPtr) {
  memcpy(mappedBuffer, srcPtr, buffInfo.size);
}; //MapBuffer

//TextureImage
TextureImage::TextureImage(uint32_t h, uint32_t w) {
  height = h;
  width = w;
  CreateImage(h,w);
  oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
}; //TextureImage
void TextureImage::CreateImage(uint32_t h, uint32_t w) {
  VkImageCreateInfo texInfo;
  VkBufferImageCopy imageCpy;

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

  image = new VkImage();
  VkResult result = vkCreateImage(externalProgram->device, &texInfo, nullptr, image);
  errorHandler->ConfirmSuccess(result, "Creating Texture Image");

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(externalProgram->device, *image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  auto propFlag = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  Utility::ConfirmMemory(&memRequirements, &propFlag, &allocInfo);

  result = vkAllocateMemory(externalProgram->device, &allocInfo, nullptr, &memory);
  errorHandler->ConfirmSuccess(result, "Allocating Image Memory");

  vkBindImageMemory(externalProgram->device, *image, memory, 0);


  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = *image;
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
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  result = vkCreateSampler(externalProgram->device, &samplerInfo, nullptr, &imageSampler);
  errorHandler->ConfirmSuccess(result, "Create Texture Sampler");
}; //CreateImage
void TextureImage::Reset() {
  vkDestroyImage(externalProgram->device, *image, nullptr);
  vkFreeMemory(externalProgram->device, memory, nullptr);
  CreateImage(width, height);
  readyToCpy = false;
  oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
}; //ResetComputeImage

//Compute Image
ComputeImage::ComputeImage(uint32_t h, uint32_t w) {
  height = h;
  width = w;
  CreateImage(h,w);
  oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
}; //ComputeImage

void ComputeImage::CreateImage(uint32_t h, uint32_t w) {
  VkImageCreateInfo texInfo;
  VkBufferImageCopy imageCpy;

  texInfo = {};
  texInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  texInfo.imageType = VK_IMAGE_TYPE_2D;
  texInfo.extent.width = width;
  texInfo.extent.height = height;
  texInfo.extent.depth = 1;
  texInfo.mipLevels = 1;
  texInfo.arrayLayers = 1;
  texInfo.format = VK_FORMAT_R8G8B8A8_SINT;
  texInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  texInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  texInfo.usage =
    VK_IMAGE_USAGE_STORAGE_BIT;
  texInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  texInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  texInfo.flags = 0;

  image = new VkImage();
  VkResult result = vkCreateImage(externalProgram->device, &texInfo, nullptr, image);
  errorHandler->ConfirmSuccess(result, "Creating Compute Image");

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(externalProgram->device, *image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  auto propFlag = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  Utility::ConfirmMemory(&memRequirements, &propFlag, &allocInfo);
  VkDeviceMemory memory;

  result = vkAllocateMemory(externalProgram->device, &allocInfo, nullptr, &memory);
  errorHandler->ConfirmSuccess(result, "Allocating Image Memory");

  vkBindImageMemory(externalProgram->device, *image, memory, 0);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = *image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_R8G8B8A8_SINT;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  vkCreateImageView(externalProgram->device, &viewInfo, nullptr, &imageView);

}; //CreateImage

void ComputeImage::Reset() {
  vkDestroyImage(externalProgram->device, *image, nullptr);
  vkFreeMemory(externalProgram->device, memory, nullptr);
  CreateImage(width, height);
}; //ResetComputeImage
