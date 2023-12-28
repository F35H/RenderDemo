#pragma once

#include "Vulkan/Objects/Syncronization/GPUSyncFactory.h"

namespace RenderIn {  
  struct GFXPipeline {
    std::shared_ptr<ExternalProgram> externalProgram;
    VkResult result;

    bool isStencil = false;
    std::vector<char> vertBuffer;
    std::vector<char> fragBuffer;
    std::vector<VkShaderModule> shaders;
    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderInfo;
    std::vector<VkPushConstantRange> pushConstantRange;
    std::vector<VkVertexInputBindingDescription> fragInput;
    std::vector<VkVertexInputAttributeDescription> vertexInput;
    std::vector<VkShaderModuleCreateInfo> shaderModulesInfo;

    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {};
    std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBinding;

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

    GFXPipeline(std::shared_ptr<ExternalProgram>* eProgram);
    void ClearPipeline();
    void IsStencil(bool stencilTrue);
    void AddShaderFile(ShaderType shaderType, std::string filestr);
    void AddShaderBinding(ShaderType shaderType, uint32_t offsetorstride);
    void AddPushConst(ShaderType pushConstLocation);
    void ShaderModuleCreation(int_fast8_t indice);
    void AddUniformBuffer(ShaderType shader);
    void AddImageSampler(ShaderType shader);
    void Activate(VkRenderPass renderPass);
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

    std::shared_ptr<ExternalProgram> externalProgram;
    VkResult result;

    void Activate(mode swapmode);

    SwapChain(std::shared_ptr<ExternalProgram>* eProgram);
    ~SwapChain();
  }; //SwapChain

  struct MainLoop {
    std::shared_ptr<ExternalProgram> externalProgram;
    VkResult result;

    SwapChain* swapChain;
    std::vector <std::shared_ptr<GFXPipeline>> gfxPipelines;

    std::vector<VkSemaphore> signalSemaphores;
    std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkFence> fences;

    std::vector<VkCommandBuffer> cpyCmdBuffers;
    std::vector<VkCommandBuffer> presentCmdBuffers;
    std::vector<VkCommandBuffer> imageCmdBuffers;
    std::vector<VkCommandBuffer> totalCmdBuffers;

    std::vector<VkBufferCopy> verticeCpy;
    std::vector<VkBufferCopy> indiceCpy;
    std::vector<VkBufferImageCopy> imageCpy;

    std::vector<std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>*> verticeBuffers;
    std::vector<std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>*> indiceBuffers;
    std::vector<std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ImageBuffer*>*> imageBuffers;
    std::vector<size_t> indiceSizes;
    std::vector<size_t> verticeSizes;

    std::vector<std::optional<VkDescriptorSet*>> descSets;
    std::vector<std::optional<PushConst*>> pushConsts;

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
    MainLoop(SwapChain* swapChainCtor, std::shared_ptr<ExternalProgram>* eProgram, std::vector<VkClearValue>* clearColor);

    void ActivateSyncedInput();
    void ActivateCmdInput();
    void ActivateSyncedOutput();
    void ActivateRender();

    void AddGfxPipeline(std::shared_ptr<GFXPipeline>* pipe, uint_fast8_t index);
    void AddCpyCmdBuffer(VkCommandBuffer buff, uint_fast8_t index);
    void AddPresentCmdBuffer(VkCommandBuffer buff, uint_fast8_t index);
    void AddPushConst(PushConst* pushConst, uint_fast8_t index);
    void AddDescSet(VkDescriptorSet* set, uint_fast8_t index);
    void AddVerticeBuffer(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>* buff, size_t verticeCount, uint_fast8_t index);
    void AddIndiceBuffer(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>* buff, size_t verticeCount, uint_fast8_t index);
    void AddImageBuffer(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ImageBuffer*>* buff, uint_fast8_t index);
    void AddImageCmdBuffer(VkCommandBuffer imageBuff, uint_fast8_t index);
  private:
    void TriggerRenderInput();
    void TriggerCpyCmd();
    void TriggerTransitionCmd();
  }; //MainLoop
}; //Render Out


