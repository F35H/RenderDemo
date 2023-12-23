#pragma once

#include "Vulkan/Objects/Buffers/CommonBuffers.h"

namespace GPUSyncFactory {

  struct Semaphore {
    VkSemaphore semaphore;
    Semaphore(VkDevice device);
  }; //Semaphore

  struct Fence {
    VkFence fence;
    Fence(VkDevice device);
  }; //Fence

  struct SyncFactory {
    std::shared_ptr<ExternalProgram> externalProgram;
    std::vector<Semaphore> semaphores;
    std::vector<Fence> fences;
    SyncFactory(std::shared_ptr<ExternalProgram>* eProgram);
    VkSemaphore AddSemaphore();
    VkFence AddFence();
  }; //SyncFactory
}; //GPUSyncFactory