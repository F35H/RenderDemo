#pragma once

#include "Vulkan/Objects/Syncronization/SyncFactory.h"

namespace RenderIn {  
  struct GFXPipeline {
    VkResult result;

    std::vector<char> vertBuffer;
    std::vector<char> fragBuffer;
    std::vector<VkShaderModule> shaders;
    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderInfo;
    std::vector<VkPushConstantRange> pushConstantRange;
    std::vector<VkVertexInputBindingDescription> fragInput;
    std::vector<VkVertexInputAttributeDescription> vertexInput;
    std::vector<VkShaderModuleCreateInfo> shaderModulesInfo;

    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {};
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};

    VkDescriptorSetLayout descriptorSetLayout;

    //Pipeline General Info
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
    VkPipelineViewportStateCreateInfo viewportStateInfo = {};
    VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
    VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
    VkPipelineColorBlendAttachmentState colorBlendAttachInfo = {};
    VkPipelineColorBlendStateCreateInfo colorBlendingInfo = {};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    VkGraphicsPipelineCreateInfo pipelineInfo = {};

    std::vector<VkDynamicState> dynamicStates;
    VkStencilOpState            stencilState;

    VkPipelineLayout pipelineLayout = {};
    VkPipeline       graphicsPipeline = {};

    enum ShaderType {
      Vertex,
      Fragment
    }; //ShaderType

    GFXPipeline() {
      shaders.resize(2);
      pipelineShaderInfo.resize(2);
      shaderModulesInfo.resize(2);

      dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
      }; //Dynamic States
    }; //GFXPipeline

    void ClearPipeline() {
      pushConstantRange.clear();
      fragInput.clear();
      vertexInput.clear();
    }; //ClearPipeline

    void AddShaderFile(ShaderType shaderType, std::string filestr) {
      std::ifstream file(filestr, std::ios::ate | std::ios::binary);

      if (!file.is_open()) throw std::runtime_error("failed to open file!");

      size_t fileSize = (size_t)file.tellg();
      std::vector<char> buffer(fileSize);

      file.seekg(0);
      file.read(buffer.data(), fileSize);

      if (shaderType == Vertex) vertBuffer = buffer;
      if (shaderType == Fragment) fragBuffer = buffer;

      file.close();
    }; //AddShaderFile

    void AddShaderBinding(ShaderType shaderType, uint32_t offsetorstride) {
      switch (shaderType) {
      case Vertex:

        VkVertexInputAttributeDescription vertexBindings;
        vertexBindings.binding = 0;
        vertexBindings.location = static_cast<uint32_t>(vertexInput.size());
        vertexBindings.format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexBindings.offset = offsetorstride;
        vertexInput.emplace_back(vertexBindings);
        return;
      case Fragment:
        VkVertexInputBindingDescription fragmentBinding;
        fragmentBinding.binding = 0;
        fragmentBinding.stride = offsetorstride;
        fragmentBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        fragInput.emplace_back(fragmentBinding);
        return;
      }; //ShaderType Switch
    }

    void AddPushConst() {
      VkPushConstantRange pushConst;
      pushConst.offset = 0;
      pushConst.size = sizeof(PushConst);
      pushConst.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      pushConstantRange.emplace_back(pushConst);
    }; //PushConst

    void ShaderModuleCreation(UINT indice) {
      result = vkCreateShaderModule(externalProgram->device, &shaderModulesInfo[indice], nullptr, &shaders[indice]);
      errorHandler->ConfirmSuccess(result, "Creating Shader: " + indice);
      if (indice > 0) return ShaderModuleCreation(indice - 1);
    }; //ShaderModuleCreation

    void Activate(VkRenderPass renderPass) {
      shaderModulesInfo[0].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      shaderModulesInfo[0].codeSize = vertBuffer.size();
      shaderModulesInfo[0].pCode = reinterpret_cast<const uint32_t*>(vertBuffer.data());

      shaderModulesInfo[1].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      shaderModulesInfo[1].codeSize = fragBuffer.size();
      shaderModulesInfo[1].pCode = reinterpret_cast<const uint32_t*>(fragBuffer.data());

      ShaderModuleCreation(static_cast<uint32_t>(shaderModulesInfo.size())-1);

      //Vertex Shader Info
      pipelineShaderInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderInfo[0].module = shaders[0];
      pipelineShaderInfo[0].pName = "main";


      //Fragment Shader
      pipelineShaderInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderInfo[1].module = shaders[1];
      pipelineShaderInfo[1].pName = "main";

      vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertexInputInfo.vertexBindingDescriptionCount = 1;
      vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInput.size());
      vertexInputInfo.pVertexBindingDescriptions = fragInput.data();
      vertexInputInfo.pVertexAttributeDescriptions = vertexInput.data();


      //DescriptorSets
      uboLayoutBinding.binding = 0;
      uboLayoutBinding.descriptorCount = 1;
      uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
      uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

      descLayoutInfo.bindingCount = 1;
      descLayoutInfo.pBindings = &uboLayoutBinding;
      descLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

      result = vkCreateDescriptorSetLayout(externalProgram->device, &descLayoutInfo, nullptr, &descriptorSetLayout);
      errorHandler->ConfirmSuccess(result, "Creating DescriptorSet Layout");


      //General Pipeline Information
      inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

      viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportStateInfo.viewportCount = 1;
      viewportStateInfo.scissorCount = 1;

      rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizerInfo.depthClampEnable = VK_FALSE;
      rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
      rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
      rasterizerInfo.lineWidth = 1.0f;
      rasterizerInfo.cullMode = VK_CULL_MODE_NONE;
      rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
      rasterizerInfo.depthBiasEnable = VK_FALSE;

      multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisamplingInfo.sampleShadingEnable = VK_FALSE;
      multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

      colorBlendAttachInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
        | VK_COLOR_COMPONENT_G_BIT 
        | VK_COLOR_COMPONENT_B_BIT 
        | VK_COLOR_COMPONENT_A_BIT;
      colorBlendAttachInfo.blendEnable = VK_FALSE;

      colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlendingInfo.logicOpEnable = VK_FALSE;
      colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
      colorBlendingInfo.attachmentCount = 1;
      colorBlendingInfo.pAttachments = &colorBlendAttachInfo;
      colorBlendingInfo.blendConstants[0] = 0.0f;
      colorBlendingInfo.blendConstants[1] = 0.0f;
      colorBlendingInfo.blendConstants[2] = 0.0f;
      colorBlendingInfo.blendConstants[3] = 0.0f;

      //stencilState.compareOp = VK_COMPARE_OP_ALWAYS;
      //stencilState.failOp = VK_STENCIL_OP_KEEP;
      //stencilState.passOp = VK_STENCIL_OP_KEEP;
      //stencilState.depthFailOp = VK_STENCIL_OP_REPLACE;
      //stencilState.reference = 1;
      //stencilState.writeMask = 0xff;
      //stencilState.compareMask = 0xff;

      depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilInfo.depthTestEnable = VK_TRUE;
      depthStencilInfo.depthWriteEnable = VK_TRUE;
      depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
      depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
      depthStencilInfo.minDepthBounds = 0.0f;
      depthStencilInfo.maxDepthBounds = 1.0f;
      depthStencilInfo.stencilTestEnable = VK_FALSE;
      //depthStencilInfo.front = stencilState;
      //depthStencilInfo.back = stencilState;

      dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
      dynamicStateInfo.pDynamicStates = dynamicStates.data();

      //Pipeline
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data();
      pipelineLayoutInfo.pushConstantRangeCount = 1;
      pipelineLayoutInfo.setLayoutCount = 1;
      pipelineLayoutInfo.pSetLayouts = { &descriptorSetLayout };

      result = vkCreatePipelineLayout(externalProgram->device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
      errorHandler->ConfirmSuccess(result, "Creating Graphics Pipeline Layout");


      pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineInfo.stageCount = 2;
      pipelineInfo.pStages = pipelineShaderInfo.data();
      pipelineInfo.pVertexInputState = &vertexInputInfo;
      pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
      pipelineInfo.pViewportState = &viewportStateInfo;
      pipelineInfo.pRasterizationState = &rasterizerInfo;
      pipelineInfo.pMultisampleState = &multisamplingInfo;
      pipelineInfo.pColorBlendState = &colorBlendingInfo;
      pipelineInfo.pDynamicState = &dynamicStateInfo;
      pipelineInfo.layout = pipelineLayout;
      pipelineInfo.renderPass = renderPass;
      pipelineInfo.subpass = 0;
      pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
      pipelineInfo.pDepthStencilState = &depthStencilInfo;

      result = vkCreateGraphicsPipelines(
        externalProgram->device, 
        VK_NULL_HANDLE, 1, 
        &pipelineInfo, nullptr, 
        &graphicsPipeline);

      errorHandler->ConfirmSuccess(result, "Creating Graphics Pipeline");

      vkDestroyShaderModule(externalProgram->device, shaders[0], nullptr);
      vkDestroyShaderModule(externalProgram->device, shaders[1], nullptr);
    }; //Activate


    void Activate(VkDevice* device, VkPhysicalDevice* physDevice) {
    }; //Activate
  }; //GFXPipeline

  struct SwapChain : VkSwapchainCreateInfoKHR {
    enum mode {
      Recreate = 0,
      Cleanup,
      Create
    }; //mode

    //Others

  public:
    VkSwapchainKHR swapChain;
    std::vector<VkSurfaceFormatKHR> scFormats;
    std::vector<VkPresentModeKHR> scModes;
    VkSurfaceCapabilitiesKHR scAbilities;
    VkImageViewCreateInfo depthViewInfo;
    VkDeviceMemory depthImageMemory;
    VkImageView depthView;
    VkImage depthImage;

    std::vector<VkImage> imageVector; //Convert This to Vector
    std::vector<VkImageView> imageViewsVector;
    std::vector<VkImageViewCreateInfo> imageViewInfoVector;

    //RenderPass
    VkSubpassDescription subpass;
    VkSubpassDependency dependency;
    std::vector<VkAttachmentDescription> renderPassAttach;
    std::vector<VkAttachmentReference> renderPassAttachRef;
    VkRenderPassCreateInfo renderPassInfo;
    VkRenderPass renderPass;

    //FrameBuffer
    std::vector<CPUBuffer::FrameBuffer> frameBuffers;

    VkResult result;

    void Activate(mode swapmode) {
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
          if (i == imageVector.size()-1) {
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

        CPUBuffer::ConfirmMemory(&memRequirements, &allocProp, &allocInfo);

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
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
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
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
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
        dependency.srcAccessMask = 0;

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

    ~SwapChain() {
      Activate(Cleanup);
    }; //SwapChain
  };

  struct MainLoop {
    VkResult result;

    SwapChain* swapChain;
    std::shared_ptr<GFXPipeline> gfxPipeline;

    std::vector<VkSemaphore> signalSemaphores;
    std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkFence> fences;

    std::vector<VkCommandBuffer> cpyCmdBuffers;
    std::vector<VkCommandBuffer> presentCmdBuffers;
    std::vector<VkCommandBuffer> totalCmdBuffers;

    std::vector<std::pair<CPUBuffer::StageBuffer, CPUBuffer::ModelBuffer>*> verticeBuffers;
    std::vector<std::pair<CPUBuffer::StageBuffer, CPUBuffer::ModelBuffer>*> indiceBuffers;

    std::vector<VkDescriptorSet> descSets;
    std::vector<PushConst*> pushConsts;

    std::vector<VkClearValue> clearColor;

    UINT currentFrameIndex = 0;
    BOOL resizeFrameBuffer = false;

  private:
    VkPresentInfoKHR presentInfo;
    VkSubmitInfo submitInfo;

    bool cpyVectors = false;
    uint32_t imageIndex = 0;
    std::optional<uint16_t> recurseIndex;

    VkCommandBufferBeginInfo startCmdBuffer;
    VkViewport cmdViewport;
    VkRect2D scissor;

    VkRenderPassBeginInfo startRenderPass;

  public:
    MainLoop() {
      startCmdBuffer =    {};
      cmdViewport =       {};
      scissor =           {};
      presentInfo =       {};
      gfxPipeline =       {};
      signalSemaphores =  {};
      waitSemaphores =    {};
      fences = {};
    }; //MainLoop

    void ActivateSyncedInput() {
      vkWaitForFences(externalProgram->device, static_cast<uint32_t>(fences.size()), fences.data(), VK_TRUE, UINT64_MAX);
      result = vkAcquireNextImageKHR(
        externalProgram->device, swapChain->swapChain, UINT64_MAX, waitSemaphores[0], VK_NULL_HANDLE, &imageIndex);

      if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapChain->Activate(swapChain->Recreate);
        return;
      }; //If Out_Of_Date
      errorHandler->ConfirmSuccess(result, "Getting SwapChain Image in MainLoop");
      vkResetFences(externalProgram->device, static_cast<uint32_t>(fences.size()), fences.data());
    }; //ActivateSyncedInput

    void ActivateCmdInput() {
      TriggerRenderInput();
      TriggerCpyCmd();
    }; //ActivateCommandInput

    void ActivateSyncedOutput() {
      totalCmdBuffers.clear();
      totalCmdBuffers.reserve(cpyCmdBuffers.size() + presentCmdBuffers.size());
      totalCmdBuffers.insert(totalCmdBuffers.begin(), cpyCmdBuffers.begin(), cpyCmdBuffers.end());
      totalCmdBuffers.insert(totalCmdBuffers.begin(), presentCmdBuffers.begin(), presentCmdBuffers.end());

      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
      submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
      submitInfo.pWaitSemaphores = waitSemaphores.data();
      submitInfo.pWaitDstStageMask = waitStages;
      submitInfo.commandBufferCount = totalCmdBuffers.size();
      submitInfo.pCommandBuffers = totalCmdBuffers.data();
      submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
      submitInfo.pSignalSemaphores = signalSemaphores.data();
      submitInfo.pNext = nullptr;

      result = vkQueueSubmit(externalProgram->gfxQueue, 1, &submitInfo, *fences.data());
      errorHandler->ConfirmSuccess(result, "Sending GPU Draw Command");
    }; //ActivateCommandOutput

    void ActivateRender() {
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = signalSemaphores.data();
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = &swapChain->swapChain;
      presentInfo.pImageIndices = &imageIndex;
      presentInfo.pNext = nullptr;
      presentInfo.pResults = nullptr;

      result = vkQueuePresentKHR(externalProgram->presentQueue, &presentInfo);
      if (result == VK_SUBOPTIMAL_KHR) externalProgram->getCurrentWindow()->windowResize = true;
      if (result == VK_ERROR_OUT_OF_DATE_KHR) externalProgram->getCurrentWindow()->windowResize = true;
      if (externalProgram->getCurrentWindow()->windowResize) {
        swapChain->Activate(swapChain->Recreate);
        externalProgram->getCurrentWindow()->windowResize = false;
        return;
      }; //windowResize
      errorHandler->ConfirmSuccess(result, "Rendering to Window");
    }; //ActivateRender

  private:
    void TriggerRenderInput() {
      if (!recurseIndex) recurseIndex = presentCmdBuffers.size() - 1;
      if (recurseIndex.value() > presentCmdBuffers.size()) { recurseIndex.reset(); return; };
      
      //Begin
      vkResetCommandBuffer(presentCmdBuffers[recurseIndex.value()], 0);

      startCmdBuffer = { };
      startCmdBuffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      result = vkBeginCommandBuffer(presentCmdBuffers[recurseIndex.value()], &startCmdBuffer);
      errorHandler->ConfirmSuccess(result, "Beginning CmdBuffer");

      startRenderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      startRenderPass.renderPass = swapChain->renderPass;
      startRenderPass.framebuffer = swapChain->frameBuffers[imageIndex].framebuffer;
      startRenderPass.renderArea.offset = { 0, 0 };
      startRenderPass.renderArea.extent = swapChain->imageExtent;
      startRenderPass.clearValueCount = static_cast<uint32_t>(clearColor.size());
      startRenderPass.pClearValues = clearColor.data();
      startRenderPass.pNext = nullptr;
      vkCmdBeginRenderPass(presentCmdBuffers[recurseIndex.value()], &startRenderPass, VK_SUBPASS_CONTENTS_INLINE);
      vkCmdBindPipeline(presentCmdBuffers[recurseIndex.value()], VK_PIPELINE_BIND_POINT_GRAPHICS, gfxPipeline->graphicsPipeline);

      cmdViewport.x = 0.0f;
      cmdViewport.y = 0.0f;
      cmdViewport.width = static_cast<float>(swapChain->imageExtent.width);
      cmdViewport.height = static_cast<float>(swapChain->imageExtent.height);
      cmdViewport.minDepth = 0.0f;
      cmdViewport.maxDepth = 1.0f;
      vkCmdSetViewport(presentCmdBuffers[recurseIndex.value()], 0, 1, &cmdViewport);

      scissor.offset = { 0, 0 };
      scissor.extent = swapChain->imageExtent;
      vkCmdSetScissor(presentCmdBuffers[recurseIndex.value()], 0, 1, &scissor);


      //Trigger Geometry Input
      if (verticeBuffers[recurseIndex.value()]->second.cpyBool
        && indiceBuffers[recurseIndex.value()]->second.cpyBool) {
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(
          presentCmdBuffers[recurseIndex.value()], 0, 1,
          verticeBuffers[recurseIndex.value()]->second.GetBuffer(), offsets);
        vkCmdBindIndexBuffer(
          presentCmdBuffers[recurseIndex.value()],
          *indiceBuffers[recurseIndex.value()]->second.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

      //TriggerUniformInput
      vkCmdPushConstants(
        presentCmdBuffers[recurseIndex.value()],
        gfxPipeline->pipelineLayout,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        0, sizeof(PushConst), pushConsts[recurseIndex.value()]
      ); //VkCmdPushCOnst
      vkCmdBindDescriptorSets(
        presentCmdBuffers[recurseIndex.value()],
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        gfxPipeline->pipelineLayout, 0, 1,
        &descSets[recurseIndex.value()], 0, nullptr);
        
      vkCmdDrawIndexed(
        presentCmdBuffers[recurseIndex.value()],
        static_cast<uint32_t>(externalProgram->getCurrentWindow()->currentModel->indices.size()), 1, 0, 0, 0);
      }; //Confirm Buffers


      //Finish Render
      vkCmdEndRenderPass(presentCmdBuffers[recurseIndex.value()]);
      result = vkEndCommandBuffer(presentCmdBuffers[recurseIndex.value()]);
      errorHandler->ConfirmSuccess(result, "Ending CmdBuffer");

      recurseIndex.value() -= 1;
      TriggerRenderInput();
    }; //TriggerRenderInput

    inline void TriggerCpyCmd() {
      if (!recurseIndex) recurseIndex = cpyCmdBuffers.size() - 1;
      if (recurseIndex.value() > cpyCmdBuffers.size()) { recurseIndex.reset(); return; };

      vkResetCommandBuffer(cpyCmdBuffers[recurseIndex.value()], 0);
      
      startCmdBuffer = { };
      startCmdBuffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      vkBeginCommandBuffer(cpyCmdBuffers[recurseIndex.value()], &startCmdBuffer);
      
      VkBufferCopy vCpyRegion{};
      vCpyRegion.size = verticeBuffers[recurseIndex.value()]->first.GetSize();
      VkBufferCopy iCpyRegion{};
      iCpyRegion.size = indiceBuffers[recurseIndex.value()]->first.GetSize();

      vkCmdCopyBuffer(
        cpyCmdBuffers[recurseIndex.value()], 
        *verticeBuffers[recurseIndex.value()]->first.GetBuffer(), 
        *verticeBuffers[recurseIndex.value()]->second.GetBuffer(), 
        1, &vCpyRegion);
      vkCmdCopyBuffer(
        cpyCmdBuffers[recurseIndex.value()],
        *indiceBuffers[recurseIndex.value()]->first.GetBuffer(),
        *indiceBuffers[recurseIndex.value()]->second.GetBuffer(),
        1, &iCpyRegion);

      verticeBuffers[recurseIndex.value()]->second.cpyBool = true;
      indiceBuffers[recurseIndex.value()]->second.cpyBool = true;

      vkEndCommandBuffer(cpyCmdBuffers[recurseIndex.value()]);
      recurseIndex.value() -= 1;
      TriggerCpyCmd();
    }; //TriggerCpyCmd

  }; //MainLoop
}; //Render Out


