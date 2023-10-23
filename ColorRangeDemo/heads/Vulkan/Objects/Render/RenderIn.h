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
    std::vector<VkSurfaceFormatKHR> scFormats;
    std::vector<VkPresentModeKHR> scModes;
    VkSurfaceCapabilitiesKHR scAbilities;
    VkSwapchainKHR swapChain;

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

        //Select Swap Chain Format
        result = vkGetPhysicalDevicePropertiesKHR(physDevice, surface, &scAbilities);
        errorHandler->ConfirmSuccess(result, "Getting Physical Device Properties");

        uint32_t formatCount;
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);
        errorHandler->ConfirmSuccess(result, "Getting Physical Device Formats");

        uint32_t presentModeCount;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);
        errorHandler->ConfirmSuccess(result, "Getting Physical Device Present Modes");

        if ((formatCount != 0) && (presentModeCount != 0)) {
          scFormats.resize(formatCount);
          scModes.resize(presentModeCount);

          result = vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);
          errorHandler->ConfirmSuccess(result, "Getting Physical Device Formats");

          vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);
          errorHandler->ConfirmSuccess(result, "Getting Physical Device Surface Present Modes");
        }
        else {
          errorHandler->ConfirmSuccess(VK_ERROR_UNKNOWN, "FormatCount or PresentModeCount is Zero");
        } //else

        for (auto& swapChainFormat : scFormats) {
          if (swapChainFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && swapChainFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            imageFormat = swapChainFormat.format;
            imageColorSpace = swapChainFormat.colorSpace;
            break;
          } //for 
        }; // for every swapchain format

        if (imageFormat == NULL
          && imageColorSpace == NULL) {
          imageFormat = scFormats[0].format;
          imageColorSpace = scFormats[0].colorSpace;
        }; //imageFormat

        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        for (const VkPresentModeKHR& mode : scModes) {
          presentMode = mode;
          break;
        }; //scModes


        //Create SwapChain Extent
        if (scAbilities.currentExtent.width
          == std::numeric_limits<uint32_t>::max()) {
          imageExtent.height = externalProgram->getCurrentWindow()->winHeight;
          imageExtent.width = externalProgram->getCurrentWindow()->winWidth;

          imageExtent.width = std::clamp(
            imageExtent.width,
            scAbilities.minImageExtent.width, 
            scAbilities.maxImageExtent.width);

          imageExtent.height = std::clamp(
            imageExtent.width,
            scAbilities.minImageExtent.width,
            scAbilities.maxImageExtent.height);
        }
        else { imageExtent = scAbilities.currentExtent; }


        //getImageCount
        minImageCount = scAbilities.minImageCount + 1;
        if (scAbilities.maxImageCount > 0 && minImageCount > scAbilities.maxImageCount) {
          minImageCount = scAbilities.maxImageCount;
        }; //if maxImageCount


        //Confirm Graphics Family
        if (externalProgram->graphicsFamily != externalProgram->presentFamily) {
          //Move this to external program
          uint32_t qFamilies[2] = { externalProgram->graphicsFamily, externalProgram->presentFamily };
          
          imageSharingMode = VK_SHARING_MODE_CONCURRENT;
          queueFamilyIndexCount = 2;
          pQueueFamilyIndices = qFamilies;
        }
        else {
          imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
          queueFamilyIndexCount = 0;
          pQueueFamilyIndices = nullptr;
        } //else


        sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        surface = externalProgram->getCurrentWindow()->surface;
        imageArrayLayers = 1;
        imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        preTransform = scAbilities.currentTransform;
        compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        presentMode = presentMode;
        clipped = VK_TRUE;
        oldSwapchain = VK_NULL_HANDLE;

        result = vkCreateSwapchainKHR(externalProgram->device, this, nullptr, &swapChain);
        errorHandler->ConfirmSuccess(result, "Creating SwapChain");

        vkGetSwapchainImagesKHR(externalProgram->device, swapChain, &minImageCount, nullptr);
        //Finish Sizing Image Views

      }; //case Create

      }; //switch
    }; //Activate

    ~SwapChain() {
      Activate(Cleanup);
    }; //SwapChain
  };
}; //Render Out


