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

    void AddShaderBinding(ShaderType shaderType, size_t offsetorstride) {
      switch (shaderType) {
      case Vertex:

        VkVertexInputAttributeDescription vertexBindings;
        vertexBindings.binding = 0;
        vertexBindings.location = vertexInput.size();
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

      ShaderModuleCreation(shaderModulesInfo.size()-1);

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

      depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilInfo.depthTestEnable = VK_TRUE;
      depthStencilInfo.depthWriteEnable = VK_TRUE;
      depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
      depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
      depthStencilInfo.minDepthBounds = 0.0f;
      depthStencilInfo.maxDepthBounds = 1.0f;
      depthStencilInfo.stencilTestEnable = VK_FALSE;
      depthStencilInfo.front = {};
      depthStencilInfo.back = {};

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
        //Move this to external

        //Select Swap Chain Format
        result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &scAbilities);
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

          result = vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, scFormats.data());
          errorHandler->ConfirmSuccess(result, "Getting Physical Device Formats");

          vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, scModes.data());
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
          if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = mode;
            break;
          }; //if mode = VK_PRESENT_MODE
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
            imageExtent.height,
            scAbilities.minImageExtent.height,
            scAbilities.maxImageExtent.height);
        }
        else { imageExtent = scAbilities.currentExtent; }


        //getImageCount
        minImageCount = scAbilities.minImageCount + 1;
        if (scAbilities.maxImageCount > 0 && minImageCount > scAbilities.maxImageCount) {
          minImageCount = scAbilities.maxImageCount;
        }; //if maxImageCount


        //Confirm Graphics Family
        if (externalProgram->graphicsFamily.value() != externalProgram->presentFamily.value()) {
          //Move this to external program
          uint32_t qFamilies[2] = { externalProgram->graphicsFamily.value(), externalProgram->presentFamily.value() };
          
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
        imageArrayLayers = 1;
        imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        preTransform = scAbilities.currentTransform;
        compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        presentMode = presentMode;
        clipped = VK_TRUE;
        oldSwapchain = VK_NULL_HANDLE;

        result = vkCreateSwapchainKHR(externalProgram->device, this, nullptr, &swapChain);
        errorHandler->ConfirmSuccess(result, "Creating SwapChain");

        
        //Create Images 
        vkGetSwapchainImagesKHR(externalProgram->device, swapChain, &minImageCount, nullptr);
        imageVector.resize(minImageCount);
        imageViewsVector.resize(minImageCount); 
        imageViewInfoVector.resize(minImageCount);
        vkGetSwapchainImagesKHR(externalProgram->device, swapChain, &minImageCount, imageVector.data());


        //Create Depth Image
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

        result = vkCreateImage(device, &imageInfo, nullptr, &depthImage);
        errorHandler->ConfirmSuccess(result, "Creating Depth Image");


        //Allocate Depth Image Memory
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, depthImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        VkMemoryPropertyFlags allocProp = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        
        CPUBuffer::ConfirmMemory(&memRequirements, &allocProp, &allocInfo);

        result = vkAllocateMemory(device, &allocInfo, nullptr, &depthImageMemory);
        errorHandler->ConfirmSuccess(result, "Depth Image Memory Allocation");

        vkBindImageMemory(device, depthImage, depthImageMemory, 0);

        //Create Image Views
        int i = imageVector.size();
        for (;i >= 0;--i) {
          
          //Create Depth View
          if (i == imageVector.size()) {
            depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            depthViewInfo.image = depthImage;
            depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            depthViewInfo.format = externalProgram->depthBufferFormat;
            depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthViewInfo.subresourceRange.baseMipLevel = 0;
            depthViewInfo.subresourceRange.levelCount = 1;
            depthViewInfo.subresourceRange.baseArrayLayer = 0;
            depthViewInfo.subresourceRange.layerCount = 1;
            
            result = vkCreateImageView(device, &depthViewInfo, nullptr, &depthView);
            errorHandler->ConfirmSuccess(result, "Creating Depth Image View");

            continue;
          }; //Create Depth View

          //CreateImageView
          imageViewInfoVector[i].sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
          imageViewInfoVector[i].image = imageVector[i];
          imageViewInfoVector[i].viewType = VK_IMAGE_VIEW_TYPE_2D;
          imageViewInfoVector[i].format = imageFormat;
          imageViewInfoVector[i].components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
          imageViewInfoVector[i].components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
          imageViewInfoVector[i].components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
          imageViewInfoVector[i].components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
          imageViewInfoVector[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          imageViewInfoVector[i].subresourceRange.baseMipLevel = 0;
          imageViewInfoVector[i].subresourceRange.levelCount = 1;
          imageViewInfoVector[i].subresourceRange.baseArrayLayer = 0;
          imageViewInfoVector[i].subresourceRange.layerCount = 1;


          result = vkCreateImageView(device, &imageViewInfoVector[i], nullptr, &imageViewsVector[i]);
          errorHandler->ConfirmSuccess(result, "Creating Image View");
        }; //for i >= 0



        //Create Render Pass
        renderPassAttach.resize(2);
        renderPassAttachRef.resize(2);

        //Color Attachment
        renderPassAttach[0].format = imageFormat;
        renderPassAttach[0].samples = VK_SAMPLE_COUNT_1_BIT;
        renderPassAttach[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        renderPassAttach[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        renderPassAttach[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        renderPassAttach[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        renderPassAttach[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        renderPassAttach[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        renderPassAttachRef[0].attachment = 0;
        renderPassAttachRef[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        //DepthAttachment
        renderPassAttach[1].format = externalProgram->depthBufferFormat;
        renderPassAttach[1].samples = VK_SAMPLE_COUNT_1_BIT;
        renderPassAttach[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        renderPassAttach[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        renderPassAttach[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        renderPassAttach[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        renderPassAttach[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        renderPassAttach[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        renderPassAttachRef[1].attachment = 1;
        renderPassAttachRef[1].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &renderPassAttachRef[0];
        subpass.pDepthStencilAttachment = &renderPassAttachRef[1];

        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.srcAccessMask = 0;

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(renderPassAttach.size());
        renderPassInfo.pAttachments = renderPassAttach.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
        errorHandler->ConfirmSuccess(result, "Creating Renderpass");

        //CreateFrameBuffers
        i = imageViewsVector.size() - 1;
        for (; i >= 0; --i) {
          frameBuffers.emplace_back(CPUBuffer::FrameBuffer({
            imageViewsVector[i],
            depthView},
            renderPass, 
            imageExtent.width,
            imageExtent.height));
        }; //imageViewsVector
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
    std::shared_ptr<CPUBuffer::BufferFactory> bufferFactory;

    std::vector<VkSemaphore> signalSemaphores;
    std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkFence> fences;

    UINT currentFrameIndex = 0;
    BOOL resizeFrameBuffer = false;

  private:
    VkPresentInfoKHR presentInfo;
    VkSubmitInfo submitInfo;

    uint32_t imageIndex = 0;

    VkCommandBufferBeginInfo startCmdBuffer;
    VkRenderPassBeginInfo startRenderPass;
    VkViewport cmdViewport;
    VkRect2D scissor;

  public:
    MainLoop() {
      startCmdBuffer = {};
      startRenderPass = {};
      cmdViewport = {};
      scissor = {};
      presentInfo = {};
      swapChain = {};
      gfxPipeline = {};
      bufferFactory = {};
      signalSemaphores = {};
      waitSemaphores = {};
      fences = {};
    }; //MainLoop

    void ActivateSyncedInput() {
      vkWaitForFences(externalProgram->device, fences.size(), fences.data(), VK_TRUE, UINT64_MAX);
      result = vkAcquireNextImageKHR(
        externalProgram->device, swapChain->swapChain, UINT64_MAX, waitSemaphores[0], VK_NULL_HANDLE, &imageIndex);

      if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapChain->Activate(swapChain->Recreate);
        return;
      }; //If Out_Of_Date
      errorHandler->ConfirmSuccess(result, "Getting SwapChain Image in MainLoop");
      vkResetFences(externalProgram->device, fences.size(), fences.data());
    }; //ActivateSyncedInput

    void ActivateCmdInput(PushConst* pushConstData, std::vector<VkClearValue> clearColor) {
      auto cmdBuffer = bufferFactory.get()->cmdPool->get()->cmdBuffers[currentFrameIndex];
      auto descSet = bufferFactory.get()->descPool->get()->descSets[currentFrameIndex];
      auto indiceBuffer = bufferFactory.get()->indexBuffers[currentFrameIndex].GetBuffer();
      auto verticeBuffer = bufferFactory.get()->vertexBuffers[currentFrameIndex].GetBuffer();

      vkResetCommandBuffer(cmdBuffer, 0);

      startCmdBuffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      result = vkBeginCommandBuffer(cmdBuffer, &startCmdBuffer);
      errorHandler->ConfirmSuccess(result, "Beginning CmdBuffer");

      startRenderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      startRenderPass.renderPass = swapChain->renderPass;
      startRenderPass.framebuffer = swapChain->frameBuffers[imageIndex].framebuffer;
      startRenderPass.renderArea.offset = { 0, 0 };
      startRenderPass.renderArea.extent = swapChain->imageExtent;
      startRenderPass.clearValueCount = static_cast<uint32_t>(clearColor.size());
      startRenderPass.pClearValues = clearColor.data();
      vkCmdBeginRenderPass(cmdBuffer, &startRenderPass, VK_SUBPASS_CONTENTS_INLINE);
      vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfxPipeline->graphicsPipeline);

      cmdViewport.x = 0.0f;
      cmdViewport.y = 0.0f;
      cmdViewport.width = static_cast<float>(swapChain->imageExtent.width);
      cmdViewport.height = static_cast<float>(swapChain->imageExtent.height);
      cmdViewport.minDepth = 0.0f;
      cmdViewport.maxDepth = 1.0f;
      vkCmdSetViewport(cmdBuffer, 0, 1, &cmdViewport);

      scissor.offset = { 0, 0 };
      scissor.extent = swapChain->imageExtent;
      vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

      VkDeviceSize offsets[] = { 0 };
      vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &verticeBuffer, offsets);
      vkCmdBindIndexBuffer(cmdBuffer, indiceBuffer, 0, VK_INDEX_TYPE_UINT16);

      vkCmdPushConstants(
        cmdBuffer,
        gfxPipeline->pipelineLayout,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        0, sizeof(PushConst), pushConstData
      ); //VkCmdPushCOnst

      auto test = externalProgram->getCurrentWindow()->currentModel;
      vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfxPipeline->pipelineLayout, 0, 1, &descSet, 0, nullptr);
      vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(externalProgram->getCurrentWindow()->currentModel->indices.size()), 1, 0, 0, 0);

      vkCmdEndRenderPass(cmdBuffer);

      result = vkEndCommandBuffer(cmdBuffer);
      errorHandler->ConfirmSuccess(result, "Ending CmdBuffer");
    }; //ActivateCommandInput

    void ActivateSyncedOutput() {
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
      submitInfo.waitSemaphoreCount = waitSemaphores.size();
      submitInfo.pWaitSemaphores = waitSemaphores.data();
      submitInfo.pWaitDstStageMask = waitStages;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &bufferFactory->cmdPool.value()->cmdBuffers[currentFrameIndex];
      submitInfo.signalSemaphoreCount = signalSemaphores.size();
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
      presentInfo.pNext = nullptr;
      presentInfo.pImageIndices = &imageIndex;

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

  }; //MainLoop
}; //Render Out


