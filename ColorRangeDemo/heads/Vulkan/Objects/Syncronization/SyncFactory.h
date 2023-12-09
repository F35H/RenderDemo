#pragma once

#include "Vulkan/Objects/Buffers/CPUBuffer.h"

struct Semaphore {
  VkSemaphore semaphore;
  
  Semaphore() {
    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkResult result = vkCreateSemaphore(externalProgram->device, &createInfo, nullptr, &semaphore);
    errorHandler->ConfirmSuccess(result, "Creating Semaphore");
  }; //Semaphore
}; //Semaphore

struct Fence {
  VkFence fence;

  Fence() {
    VkFenceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkResult result = vkCreateFence(externalProgram->device, &createInfo, nullptr, &fence);
    errorHandler->ConfirmSuccess(result, "Creating Fence");
  }; //Fence
}; //Fence

struct SyncFactory {
  std::vector<Semaphore> semaphores;
  std::vector<Fence> fences;

  VkSemaphore AddSemaphore() {
    semaphores.emplace_back(Semaphore());
    return semaphores[semaphores.size() - 1].semaphore;
  }; //AddSemaphore

  VkFence AddFence() {
    fences.emplace_back(Fence());
    return fences[fences.size() - 1].fence;
  }; //AddFence
}; //SyncFactory