#pragma once

#include "Components.h"
#include "ConsoleUI/UISingletons.h"

const inline INT numFrames = 4;
const inline INT frameCmd = std::pow(numFrames,2);
const inline INT cpyCmdNum = std::pow(numFrames,3);


namespace Vulkan{
  struct VulkanRender {
    VulkanRender();
  }; //Vulkan

}; //Vulkan