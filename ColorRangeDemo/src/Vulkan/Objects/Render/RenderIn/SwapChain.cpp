#include <Vulkan/Objects/Render/RenderIn.h>

using namespace RenderIn;

SwapChain::SwapChain(std::shared_ptr<ExternalProgram>* eProgram) {
  externalProgram = *eProgram;
}; //externalProgram

void SwapChain::Activate(mode swapmode) {
  surface = externalProgram->getCurrentWindow()->surface;
  VkDevice device = externalProgram->device;
  VkPhysicalDevice physDevice = externalProgram->physicalDevice;

  switch (swapmode) {
  case_Recreate:
  case Recreate: {
    if (externalProgram->getCurrentWindow()->IsMinimized()) goto case_Recreate;
    vkDeviceWaitIdle(device);
  }; //Recreate

             case_Cleanup:
  case Cleanup: {
    size_t i = 0;
    for (; i < imageVector.size(); ++i) {
      if (i == imageVector.size() - 1) {
        vkDestroyImage(device, depthImage, nullptr);
        vkDestroyImageView(device, depthView, nullptr);
        vkFreeMemory(device, depthImageMemory, nullptr);
        continue;
      }; //Depth View

      vkDestroyFramebuffer(device, frameBuffers[i].framebuffer, nullptr);
      vkDestroyImageView(device, imageViewsVector[i], nullptr);


    }; //frameBuffers

    frameBuffers.clear();

    vkDestroySwapchainKHR(device, swapChain, nullptr);
    if (swapmode == Cleanup) return;
  }; //Cleanup

            case_Create:
  case Create: {
    //CREATE SWAP CHAIN
    surface = externalProgram->getCurrentWindow()->surface;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(externalProgram->physicalDevice, surface, &scAbilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(externalProgram->physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
      scFormats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(externalProgram->physicalDevice, surface, &formatCount, scFormats.data());
    } //if (formatCount != 0)
    else {
      throw std::runtime_error("formatCount Failed");
    }; //formatCount != 0

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(externalProgram->physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
      scModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(externalProgram->physicalDevice, surface, &presentModeCount, scModes.data());
    } //if presentModeCount != 0
    else {
      throw std::runtime_error("presentModeCountFailed Failed");
    }; //presentModeCount

    auto surfaceFormat = scFormats[0];
    for (auto& swapChainFormat : scFormats) {
      if (swapChainFormat.format == VK_FORMAT_B8G8R8A8_SRGB && swapChainFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        surfaceFormat = swapChainFormat;
        break;
      } //for 
    } //for formats


    auto presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode : scModes) {
      if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
        presentMode = mode;
        break;
      } //if mode == VK_PRESENT_MODE
    } //for (const auto mode)


    if (scAbilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) imageExtent = scAbilities.currentExtent;
    else {
      int width, height;
      glfwGetFramebufferSize(externalProgram->getCurrentWindow()->window, &width, &height);

      VkExtent2D extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
      }; //VkExtent2D

      imageExtent.width = std::clamp(extent.width, scAbilities.minImageExtent.width, scAbilities.maxImageExtent.width);
      imageExtent.height = std::clamp(extent.height, scAbilities.minImageExtent.height, scAbilities.maxImageExtent.height);
    }; //else

    uint32_t imageCount = scAbilities.minImageCount + 1;
    if (scAbilities.maxImageCount > 0 && imageCount > scAbilities.maxImageCount) {
      imageCount = scAbilities.maxImageCount;
    } //Swap Chain Abilities

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = imageExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { externalProgram->graphicsFamily.value(), externalProgram->presentFamily.value() };

    if (externalProgram->graphicsFamily.value() != externalProgram->presentFamily.value()) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0;
      createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = scAbilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(externalProgram->device, &createInfo, nullptr, &swapChain);
    errorHandler->ConfirmSuccess(result, "Creating SwapChain");

    vkGetSwapchainImagesKHR(externalProgram->device, swapChain, &imageCount, nullptr);
    imageVector.resize(imageCount);
    vkGetSwapchainImagesKHR(externalProgram->device, swapChain, &imageCount, imageVector.data());

    imageFormat = surfaceFormat.format;

    //CREATE DEPTH BUFFER 

    //for (VkFormat format : {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}) {
    //  VkFormatProperties props;
    //  vkGetPhysicalDeviceFormatProperties(externalProgram->physicalDevice, format, &props);

    //  if (VK_IMAGE_TILING_OPTIMAL == VK_IMAGE_TILING_LINEAR
    //    && (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
    //    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
    //    depthFormat = format;
    //    break;
    //  }
    //  else if (VK_IMAGE_TILING_OPTIMAL == VK_IMAGE_TILING_OPTIMAL
    //    && (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
    //    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
    //    depthFormat = format;
    //    break;
    //  } //if correct format;
    //} //for VkFormat

    if (!externalProgram->depthBufferFormat) throw std::runtime_error("failed to find supported format!");

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = imageExtent.width;
    imageInfo.extent.height = imageExtent.width;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = externalProgram->depthBufferFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateImage(externalProgram->device, &imageInfo, nullptr, &depthImage);
    errorHandler->ConfirmSuccess(result, "Creating SwapChain Images");

    VkMemoryRequirements memRequirements{};
    vkGetImageMemoryRequirements(externalProgram->device, depthImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    VkMemoryPropertyFlags allocProp = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    CPUBuffer::Utility::ConfirmMemory(&memRequirements, &allocProp, &allocInfo);

    result = vkAllocateMemory(externalProgram->device, &allocInfo, nullptr, &depthImageMemory);
    errorHandler->ConfirmSuccess(result, "Allocate Memory");

    vkBindImageMemory(externalProgram->device, depthImage, depthImageMemory, 0);


    // CREATE IMAGE VIEWS

    imageViewsVector.resize(imageVector.size());

    //Plus One added for Depth Buffer
    for (size_t i = 0; i < (imageVector.size() + 1); ++i) {

      VkImageViewCreateInfo createInfo{};
      if (i < imageVector.size()) {
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = imageVector[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = imageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(externalProgram->device, &createInfo, nullptr, &imageViewsVector[i]);
        errorHandler->ConfirmSuccess(result, "Creating Depth Image View");
      } //if swapChain Image
      else {
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = depthImage;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = externalProgram->depthBufferFormat;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(externalProgram->device, &createInfo, nullptr, &depthView);
        errorHandler->ConfirmSuccess(result, "Creating Depth Image View");
      } //if depth Buffer Image
    }; //for images in swapchain

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = imageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = externalProgram->depthBufferFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;


    result = vkCreateRenderPass(externalProgram->device, &renderPassInfo, nullptr, &renderPass);
    errorHandler->ConfirmSuccess(result, "Creating RenderPass");

    // CREATE FRAME BUFFERS

    for (size_t i = 0; i < imageViewsVector.size(); i++) {
      frameBuffers.emplace_back(CPUBuffer::FrameBuffer({
      imageViewsVector[i],
      depthView },
      renderPass,
      imageExtent.width,
      imageExtent.height));
    } //for (size_t i)
  }; //case Create

  }; //switch
}; //Activate

void SwapChain::AddTexture(uint32_t height, uint32_t width, uint_fast8_t index) {
  VkImage texImage;
  VkImageCreateInfo texInfo;
  VkBufferImageCopy imageCpy;
  if (texImages.size() <= index) { 
    texImages.resize(index + 1); 
    texImages[index] = texImage; 
    texCpy.resize(index + 1);
    texCpy[index] = imageCpy;
  }  //texImages
  else { texImages[index] = texImage; };
  
  texInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  texInfo.imageType = VK_IMAGE_TYPE_2D;
  texInfo.extent.width = static_cast<uint32_t>(width);
  texInfo.extent.height = static_cast<uint32_t>(height);
  texInfo.extent.depth = 1;
  texInfo.mipLevels = 1;
  texInfo.arrayLayers = 1;
  texInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
  texInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  texInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  texInfo.usage = 
    VK_IMAGE_USAGE_SAMPLED_BIT | 
    VK_IMAGE_USAGE_STORAGE_BIT |
    VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  texInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  texInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  texInfo.flags = 0;

  texCpy[index].bufferOffset = 0;
  texCpy[index].bufferRowLength = 0;
  texCpy[index].bufferImageHeight = 0;
  texCpy[index].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  texCpy[index].imageSubresource.mipLevel = 0;
  texCpy[index].imageSubresource.baseArrayLayer = 0;
  texCpy[index].imageSubresource.layerCount = 1;
  texCpy[index].imageOffset = { 0, 0, 0 };
  texCpy[index].imageExtent = { width, height, 1};

  result = vkCreateImage(externalProgram->device, &texInfo, nullptr, &texImage);
  errorHandler->ConfirmSuccess(result, "Creating Texture Image");
}; //AddTexture

SwapChain::~SwapChain() {
  Activate(Cleanup);
}; //SwapChain