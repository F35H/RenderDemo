#include <vulkan/VulkanRender.h>

using namespace GPUSyncFactory;

SyncFactory::SyncFactory(std::shared_ptr<ExternalProgram>* eProgram) {
  externalProgram = *eProgram;
}; //SyncFactory
VkSemaphore SyncFactory::AddSemaphore() {
  semaphores.emplace_back(externalProgram->device);
  return semaphores[semaphores.size() - 1].semaphore;
}; //AddSemaphore
VkFence SyncFactory::AddFence() {
  fences.emplace_back(Fence(externalProgram->device));
  return fences[fences.size() - 1].fence;
}; //AddFence

//Fence
Fence::Fence(VkDevice device) {
  VkFenceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  VkResult result = vkCreateFence(device, &createInfo, nullptr, &fence);
  errorHandler->ConfirmSuccess(result, "Creating Fence");
}; //Fence

//Semaphore
Semaphore::Semaphore(VkDevice device) {
  VkSemaphoreCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkResult result = vkCreateSemaphore(device, &createInfo, nullptr, &semaphore);
  errorHandler->ConfirmSuccess(result, "Creating Semaphore");
}; //Semaphore
