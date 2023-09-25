#pragma once

#include "Vulkan/Objects/ModelTypes/ModelFactory.h"

namespace RenderIn {
  BufferFactory* CPUBuffer;
  UniformFactory* GPUBuffer;
  
  struct VulkanInit {

  }; //VulkanInit

  struct GFXPipeline {
    enum mode {

    }; //mode

    GFXPipeline(std::string shader) {

    }; //GFXPipeline

    void Activate(VkDevice* device, VkPhysicalDevice* physDevice) {
    }; //Activate
  }; //GFXPipeline


  struct SwapChain : VkSwapchainCreateInfoKHR {
    enum mode {
      Recreate = 0,
      Cleanup,
      Create_Surface
    }; //mode

    //Others

  public:
    VkSwapchainKHR swapChain;
    std::vector<VkSurfaceFormatKHR> swapChainFormats;
    std::vector<VkPresentModeKHR> swapChainModes;
    VkSurfaceCapabilitiesKHR swapChainAbilities;

    SwapChain() {};

    void CreateSwapChain();

    void Activate(mode swapmode) {
      using namespace RenderOut;

      size_t fbSize = CPUBuffer->framebuffers.size();
      Window* window = externalProgram->getCurrentWindow();
      VkDevice device = externalProgram->device;
      VkPhysicalDevice physDevice = externalProgram->physicalDevice;
      VkResult result;

      switch (swapmode) {
      case_Recreate:
      case Recreate: {
        if (window->IsMinimized()) goto case_Recreate;
        vkDeviceWaitIdle(device);
      }; //Recreate

      case_Cleanup:
      case Cleanup: {
        CPUBuffer->framebuffers.clear();
        CPUBuffer->imageBuffers.clear();
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        if (swapmode == Cleanup) return;
      }; //Cleanup

      case_Create:
      case Create_Surface: {
        result = vkGetPhysicalDevicePropertiesKHR(physDevice, surface, &swapChainAbilities);
        errorHandler->ConfirmSuccess(result, "Getting Physical Device Properties");

        uint32_t formatCount;
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);
        errorHandler->ConfirmSuccess(result, "Getting Physical Device Formats");

        uint32_t presentModeCount;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);
        errorHandler->ConfirmSuccess(result, "Getting Physical Device Present Modes");

        if ((formatCount != 0) && (presentModeCount != 0)) {
          swapChainFormats.resize(formatCount);
          swapChainModes.resize(presentModeCount);

          result = vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);
          errorHandler->ComfirmSuccess(result, "Getting Physical Device Formats");

          vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);
          errorHandler->ComfirmSuccess(result, "Getting Physical Device Surface Present Modes");
        }
        else {
          errorHandler->ThrowError("Hardcoded Failure", "FormatCount or PresentModeCount is Zero");
        } //else

        for (auto& swapChainFormat : swapChainFormats) {
          if (swapChainFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && swapChainFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            imageFormat = swapChainFormat.format;
            imageColorSpace = swapChainFormat.colorSpace;
            break;
          } //for 
        }; // for every swapchain format

        if (imageFormat == NULL
          && imageColorSpace == NULL) {
          imageFormat = swapChainFormats[0].format;
          imageColorSpace = swapChainFormats[0].colorSpace;
        }; //imageFormat






      }; //case Create

      }; //switch
    }; //Activate

    ~SwapChain() {
      Activate(Cleanup);
    }; //SwapChain
  };
}; //Render Out


