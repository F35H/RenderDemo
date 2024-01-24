#pragma once

#include "Vulkan/Objects/Syncronization/GPUSyncFactory.h"

namespace RenderIn {
  struct GFXPipeline {
    std::shared_ptr<ExternalProgram> externalProgram;
    VkResult result;

    struct GFXVars {
      bool isStencil = false;
      std::vector<char> vertBuffer = {};
      std::vector<char> fragBuffer = {};
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
    }; //GFXVars

    struct ComputeVars {
      VkDescriptorSetLayoutCreateInfo descLayoutInfo = {};
      VkDescriptorSetLayout descriptorSetLayout = {};
      std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBinding;
      std::vector<VkPushConstantRange> pushConstantRange;
      
      VkPipeline                      pipeline = {};
      VkComputePipelineCreateInfo     pipelineCreateInfo = {};
      VkPipelineLayout                pipelineLayout = {};
      VkPipelineLayoutCreateInfo      pipelineLayoutInfo = {};
      VkPipelineShaderStageCreateInfo pipelineShaderInfo = {};
      VkShaderModuleCreateInfo  shaderModuleInfo;
      VkShaderModule shaderModule;
      std::vector<char> buffer;
    }; //ComputeVars

    GFXVars gfxVars;
    ComputeVars compVars;

    enum ShaderType {
      Vertex    = 0b00,
      Fragment  = 0b01,
      Compute  =  0b11
    }; //ShaderType

    enum BlenderType {
      None = 0,
      Partial
    }; //ShaderType

    GFXPipeline();
    GFXPipeline(std::shared_ptr<ExternalProgram>* eProgram);
    void ClearPipeline();
    void IsStencil(bool stencilTrue);
    void AddShaderFile(ShaderType shaderType, std::string filestr);
    void AddShaderBinding(uint32_t offsetorstride);
    void AddBindingDesc(uint32_t offsetorstride, VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX);
    void AddPushConst(ShaderType locations[], int_fast8_t shadCount);
    void ShaderModuleCreation(int_fast8_t indice, GFXVars* gVars);
    void ShaderModuleCreation(ComputeVars* gVars);
    void AddUniformBuffer(ShaderType locations[], int_fast8_t shadCount, uint32_t binding);
    void AddImageSampler(ShaderType locations[], int_fast8_t shadCount, uint32_t binding);
    void AddStorageImage(ShaderType locations[], int_fast8_t shadCount, uint32_t binding);
    void SetBlendingAttachment(BlenderType type);
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

    VkSemaphore signalSemaphore;
    VkSemaphore waitSemaphore;
    VkFence fence;

    VkCommandBuffer cmdBuffer;

    std::vector <std::shared_ptr<GFXPipeline>> gfxPipelines;

    std::vector<VkBufferCopy> verticeCpy;
    std::vector<VkBufferCopy> indiceCpy;
    std::vector<VkBufferImageCopy> imageCpy;

    std::vector<std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>*> verticeBuffers;
    std::vector<std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>*> indiceBuffers;
    std::vector<std::pair<CPUBuffer::StageBuffer*, CPUBuffer::TextureImage*>*> resetTimages;
    std::vector<std::pair<CPUBuffer::StageBuffer*, CPUBuffer::TextureImage*>*> cpyTimages;
    std::vector< std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ComputeImage*>> computeImages;
    std::vector<size_t> indiceSizes;
    std::vector<size_t> verticeSizes;
    std::vector<Texture*> resetTextures;

    std::vector<std::optional<VkDescriptorSet*>> pesentDescSets;
    std::vector<std::optional<VkDescriptorSet*>> computeDescSets;
    std::vector<std::optional<PushConst*>> pushConsts;

    std::vector<VkClearValue> clearColor;

    UINT currentFrameIndex = 0;
    BOOL resizeFrameBuffer = false;

  private:
    VkPresentInfoKHR presentInfo;
    VkSubmitInfo submitInfo;

    bool cpyVectors = false;
    uint32_t imageIndex = 0;

    VkCommandBufferBeginInfo startCmdBuffer;
    VkViewport cmdViewport;
    VkRect2D scissor;

    VkRenderPassBeginInfo startRenderPass;

  public:
    MainLoop(SwapChain* swapChainCtor, std::shared_ptr<ExternalProgram>* eProgram, std::vector<VkClearValue>* clearColor);

    void ActivateSyncedInput();
    void ActivateCmdInput();
    void TriggerRenderInput();
    void TriggerCpyCmd();
    void TerminateCmdInput();
    void ActivateSyncedOutput();
    void ActivateRender();

    void AddGfxPipeline(std::shared_ptr<GFXPipeline>* pipe, uint_fast8_t index);
    void AddPushConst(PushConst* pushConst, uint_fast8_t index);
    void AddPresentDescSet(VkDescriptorSet* set, uint_fast8_t index);
    void AddCompDescSet(VkDescriptorSet* set, uint_fast8_t index);
    void AddVerticeBuffer(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>* buff, size_t verticeCount, uint_fast8_t index);
    void AddIndiceBuffer(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>* buff, size_t verticeCount, uint_fast8_t index);
    
    void AddCpyTimage(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::TextureImage*>* buff);
    void AddResetTimage(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::TextureImage*>* buff, Texture* t);

    void AddComputeImage(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ComputeImage*>* buff, uint_fast8_t index);

    void AddImageCmdBuffer(VkCommandBuffer imageBuff, uint_fast8_t index);

    void ClearMainLoop();

  }; //MainLoop
}; //Render Out


