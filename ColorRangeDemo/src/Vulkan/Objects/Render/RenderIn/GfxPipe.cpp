#include <Vulkan/Objects/Render/RenderIn.h>

using namespace RenderIn;

GFXPipeline::GFXPipeline(std::shared_ptr<ExternalProgram>* eProgram) {
  externalProgram = *eProgram;
  
  gfxVars.shaders.resize(2);
  gfxVars.pipelineShaderInfo.resize(2);
  gfxVars.shaderModulesInfo.resize(2);

  gfxVars.dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  }; //Dynamic States
}; //GFXPipeline

void GFXPipeline::AddShaderFile(ShaderType shaderType, std::string filestr) {
  std::ifstream file(filestr, std::ios::ate | std::ios::binary);

  if (!file.is_open()) throw std::runtime_error(
    "Failed to Open Shader File!");

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  if (shaderType == Vertex) gfxVars.vertBuffer = buffer;
  if (shaderType == Fragment) gfxVars.fragBuffer = buffer;
  if (shaderType == Compute) compVars.buffer = buffer;

  file.close();
}; //AddShaderFile

void GFXPipeline::ClearPipeline() {
  compVars.pushConstantRange.clear();

  gfxVars.pushConstantRange.clear();
  gfxVars.fragInput.clear();
  gfxVars.vertexInput.clear();
}; //ClearPipeline

void GFXPipeline::IsStencil(bool stencilTrue) {
  gfxVars.isStencil = stencilTrue;
}; //IsStencil

void GFXPipeline::Activate(VkRenderPass renderPass) {
  gfxVars.shaderModulesInfo[0].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  gfxVars.shaderModulesInfo[0].codeSize = gfxVars.vertBuffer.size();
  gfxVars.shaderModulesInfo[0].pCode = reinterpret_cast<const uint32_t*>(gfxVars.vertBuffer.data());

  gfxVars.shaderModulesInfo[1].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  gfxVars.shaderModulesInfo[1].codeSize = gfxVars.fragBuffer.size();
  gfxVars.shaderModulesInfo[1].pCode = reinterpret_cast<const uint32_t*>(gfxVars.fragBuffer.data());

  ShaderModuleCreation(static_cast<uint32_t>(gfxVars.shaderModulesInfo.size()) - 1, &gfxVars);

  //Vertex Shader Info
  gfxVars.pipelineShaderInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  gfxVars.pipelineShaderInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  gfxVars.pipelineShaderInfo[0].module = gfxVars.shaders[0];
  gfxVars.pipelineShaderInfo[0].pName = "main";

  //Fragment Shader
  gfxVars.pipelineShaderInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  gfxVars.pipelineShaderInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  gfxVars.pipelineShaderInfo[1].module = gfxVars.shaders[1];
  gfxVars.pipelineShaderInfo[1].pName = "main";

  gfxVars.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  gfxVars.vertexInputInfo.vertexBindingDescriptionCount = gfxVars.fragInput.size();
  gfxVars.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(gfxVars.vertexInput.size());
  gfxVars.vertexInputInfo.pVertexBindingDescriptions = gfxVars.fragInput.data();
  gfxVars.vertexInputInfo.pVertexAttributeDescriptions = gfxVars.vertexInput.data();

  //DescriptorSets
  gfxVars.descLayoutInfo.bindingCount = gfxVars.descSetLayoutBinding.size();
  gfxVars.descLayoutInfo.pBindings = gfxVars.descSetLayoutBinding.data();
  gfxVars.descLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

  result = vkCreateDescriptorSetLayout(externalProgram->device, &gfxVars.descLayoutInfo, nullptr, &gfxVars.descriptorSetLayout);
  errorHandler->ConfirmSuccess(result, "Creating DescriptorSet Layout");


  //General Pipeline Information
  gfxVars.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  gfxVars.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  gfxVars.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

  gfxVars.viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  gfxVars.viewportStateInfo.viewportCount = 1;
  gfxVars.viewportStateInfo.scissorCount = 1;

  gfxVars.rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  gfxVars.rasterizerInfo.depthClampEnable = VK_FALSE;
  gfxVars.rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
  gfxVars.rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
  gfxVars.rasterizerInfo.lineWidth = 1.0f;
  gfxVars.rasterizerInfo.cullMode = VK_CULL_MODE_NONE;
  gfxVars.rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  gfxVars.rasterizerInfo.depthBiasEnable = VK_FALSE;

  gfxVars.multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  gfxVars.multisamplingInfo.sampleShadingEnable = VK_FALSE;
  gfxVars.multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;



  gfxVars.colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  gfxVars.colorBlendingInfo.logicOpEnable = VK_FALSE;
  gfxVars.colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
  gfxVars.colorBlendingInfo.attachmentCount = 1;
  gfxVars.colorBlendingInfo.pAttachments = &gfxVars.colorBlendAttachInfo;
  gfxVars.colorBlendingInfo.blendConstants[0] = 0.0f;
  gfxVars.colorBlendingInfo.blendConstants[1] = 0.0f;
  gfxVars.colorBlendingInfo.blendConstants[2] = 0.0f;
  gfxVars.colorBlendingInfo.blendConstants[3] = 0.0f;

  if (gfxVars.isStencil == true) {
    gfxVars.stencilState.compareOp = VK_COMPARE_OP_NOT_EQUAL;
    gfxVars.stencilState.failOp = VK_STENCIL_OP_KEEP;
    gfxVars.stencilState.passOp = VK_STENCIL_OP_REPLACE;
    gfxVars.stencilState.depthFailOp = VK_STENCIL_OP_KEEP;
    gfxVars.depthStencilInfo.depthTestEnable = VK_TRUE;
  } //isStencil is true
  else {
    gfxVars.stencilState.compareOp = VK_COMPARE_OP_ALWAYS;
    gfxVars.stencilState.failOp = VK_STENCIL_OP_KEEP;
    gfxVars.stencilState.passOp = VK_STENCIL_OP_REPLACE;
    gfxVars.stencilState.depthFailOp = VK_STENCIL_OP_KEEP;
    gfxVars.depthStencilInfo.depthTestEnable = VK_TRUE;
  }; //Stencil

  gfxVars.stencilState.reference = 1;
  gfxVars.stencilState.writeMask = 0xff;
  gfxVars.stencilState.compareMask = 0xff;

  gfxVars.depthStencilInfo.front = gfxVars.stencilState;
  gfxVars.depthStencilInfo.back = gfxVars.stencilState;
  gfxVars.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  gfxVars.depthStencilInfo.depthWriteEnable = VK_TRUE;
  gfxVars.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  gfxVars.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
  gfxVars.depthStencilInfo.minDepthBounds = 0.0f;
  gfxVars.depthStencilInfo.maxDepthBounds = 1.0f;
  gfxVars.depthStencilInfo.stencilTestEnable = VK_TRUE;


  gfxVars.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  gfxVars.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(gfxVars.dynamicStates.size());
  gfxVars.dynamicStateInfo.pDynamicStates = gfxVars.dynamicStates.data();

  //Pipeline
  gfxVars.pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  gfxVars.pipelineLayoutInfo.pPushConstantRanges = gfxVars.pushConstantRange.data();
  gfxVars.pipelineLayoutInfo.pushConstantRangeCount = gfxVars.pushConstantRange.size();
  gfxVars.pipelineLayoutInfo.setLayoutCount = 1;
  gfxVars.pipelineLayoutInfo.pSetLayouts = { &gfxVars.descriptorSetLayout };

  result = vkCreatePipelineLayout(externalProgram->device, &gfxVars.pipelineLayoutInfo, nullptr, &gfxVars.pipelineLayout);
  errorHandler->ConfirmSuccess(result, "Creating Graphics Pipeline Layout");


  gfxVars.pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  gfxVars.pipelineInfo.stageCount = 2;
  gfxVars.pipelineInfo.pStages = gfxVars.pipelineShaderInfo.data();
  gfxVars.pipelineInfo.pVertexInputState = &gfxVars.vertexInputInfo;
  gfxVars.pipelineInfo.pInputAssemblyState = &gfxVars.inputAssemblyInfo;
  gfxVars.pipelineInfo.pViewportState = &gfxVars.viewportStateInfo;
  gfxVars.pipelineInfo.pRasterizationState = &gfxVars.rasterizerInfo;
  gfxVars.pipelineInfo.pMultisampleState = &gfxVars.multisamplingInfo;
  gfxVars.pipelineInfo.pColorBlendState = &gfxVars.colorBlendingInfo;
  gfxVars.pipelineInfo.pDynamicState = &gfxVars.dynamicStateInfo;
  gfxVars.pipelineInfo.layout = gfxVars.pipelineLayout;
  gfxVars.pipelineInfo.renderPass = renderPass;
  gfxVars.pipelineInfo.subpass = 0;
  gfxVars.pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  gfxVars.pipelineInfo.pDepthStencilState = &gfxVars.depthStencilInfo;

  result = vkCreateGraphicsPipelines(
    externalProgram->device,
    VK_NULL_HANDLE, 1,
    &gfxVars.pipelineInfo, nullptr,
    &gfxVars.graphicsPipeline);

  errorHandler->ConfirmSuccess(result, "Creating Graphics Pipeline");


  if (compVars.descSetLayoutBinding.size() > 0) {
    //Craft Compute Shader
    compVars.shaderModuleInfo = {};
    compVars.shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    compVars.shaderModuleInfo.codeSize = compVars.buffer.size();
    compVars.shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(compVars.buffer.data());

    ShaderModuleCreation(&compVars);

    //Compute Shader
    compVars.pipelineShaderInfo = {};
    compVars.pipelineShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    compVars.pipelineShaderInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    compVars.pipelineShaderInfo.module = compVars.shaderModule;
    compVars.pipelineShaderInfo.pName = "main";

    compVars.descLayoutInfo.bindingCount = compVars.descSetLayoutBinding.size();
    compVars.descLayoutInfo.pBindings = compVars.descSetLayoutBinding.data();
    compVars.descLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    result = vkCreateDescriptorSetLayout(externalProgram->device, &compVars.descLayoutInfo, nullptr, &compVars.descriptorSetLayout);
    errorHandler->ConfirmSuccess(result, "Creating DescriptorSet Layout");

    //Compute Pipeline 
    compVars.pipelineLayoutInfo = {};
    compVars.pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    compVars.pipelineLayoutInfo.pPushConstantRanges = compVars.pushConstantRange.data();
    compVars.pipelineLayoutInfo.pushConstantRangeCount = compVars.pushConstantRange.size();
    compVars.pipelineLayoutInfo.setLayoutCount = 1;
    compVars.pipelineLayoutInfo.pSetLayouts = &compVars.descriptorSetLayout;

    //DescriptorSetLayour is null
    result = vkCreatePipelineLayout(externalProgram->device, &compVars.pipelineLayoutInfo, nullptr, &compVars.pipelineLayout);
    errorHandler->ConfirmSuccess(result, "Creating Compute Pipeline Layout");

    compVars.pipelineCreateInfo = {};
    compVars.pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compVars.pipelineCreateInfo.layout = compVars.pipelineLayout;
    compVars.pipelineCreateInfo.stage = compVars.pipelineShaderInfo;

    result = vkCreateComputePipelines(externalProgram->device, VK_NULL_HANDLE, 1, &compVars.pipelineCreateInfo, nullptr, &compVars.pipeline);
    errorHandler->ConfirmSuccess(result, "Creating Compute Pipeline");
    
    vkDestroyShaderModule(externalProgram->device, compVars.shaderModule, nullptr);
  }; //compVars.descSetLayoutBinding.size()


  for (auto& shader : gfxVars.shaders) {
    vkDestroyShaderModule(externalProgram->device, shader, nullptr);
  }; //shaders
}; //Activate

void GFXPipeline::ShaderModuleCreation(int_fast8_t indice, GFXVars* gVars) {
  result = vkCreateShaderModule(externalProgram->device, &gVars->shaderModulesInfo[indice], nullptr, &gVars->shaders[indice]);
  errorHandler->ConfirmSuccess(result, "Creating Graphics Pipeline Shader: " + indice);
  if (indice > 0) return ShaderModuleCreation(indice - 1, gVars);
}; //ShaderModuleCreation

void GFXPipeline::ShaderModuleCreation(ComputeVars* gVars) {
  result = vkCreateShaderModule(externalProgram->device, &gVars->shaderModuleInfo, nullptr, &gVars->shaderModule);
  errorHandler->ConfirmSuccess(result, "Creating Compute Shader");
}; //ShaderModuleCreation

inline void GFXPipeline::AddShaderBinding(uint32_t offsetorstride) {
  VkVertexInputAttributeDescription vertexBindings;
  vertexBindings.binding = 0;
  vertexBindings.location = static_cast<uint32_t>(gfxVars.vertexInput.size());
  vertexBindings.format = VK_FORMAT_R32G32B32A32_SFLOAT;
  vertexBindings.offset = offsetorstride;
  gfxVars.vertexInput.emplace_back(vertexBindings);
}; //AddShaderBinding

inline void GFXPipeline::AddBindingDesc(uint32_t offsetorstride, VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX) {
  VkVertexInputBindingDescription inputBinding;
  inputBinding.binding = 0;
  inputBinding.stride = offsetorstride;
  inputBinding.inputRate = rate;
  gfxVars.fragInput.emplace_back(inputBinding);
}; //AddBindingDesc

void GFXPipeline::AddUniformBuffer(ShaderType* locations, int_fast8_t shadCount, uint32_t binding) {
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.binding = binding;
  
  uint_fast8_t pipeMark = 0;
  for (uint_fast8_t i = 0; i <= shadCount;++i) {
    if (locations[i] == Fragment) {
      uboLayoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
      if (pipeMark > 0) pipeMark++;
    }; //if Fragment
    if (locations[i] == Vertex) {
      uboLayoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
      if (pipeMark > 0) pipeMark++;
    }; //if Vertex
    if (locations[i] == Compute)
    {
      uboLayoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
      pipeMark++;
    }; //if Compute
  }; //forloop
  if (pipeMark == 0) { gfxVars.descSetLayoutBinding.push_back(uboLayoutBinding);  }
  if (pipeMark == 1) { compVars.descSetLayoutBinding.push_back(uboLayoutBinding); }
  if (pipeMark > 1) { gfxVars.descSetLayoutBinding.push_back(uboLayoutBinding); 
  compVars.descSetLayoutBinding.push_back(uboLayoutBinding); };

}; //AddUniformBuffer

void GFXPipeline::AddImageSampler(ShaderType* locations, int_fast8_t shadCount, uint32_t binding) {
  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = binding;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  uint_fast8_t pipeMark = 0;
  for (uint_fast8_t i = 0; i <= shadCount; ++i) {
    if (locations[i] == Fragment) {
      samplerLayoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
      if (pipeMark > 0) pipeMark++;
    }; //if Fragment
    if (locations[i] == Vertex) {
      samplerLayoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
      if (pipeMark > 0) pipeMark++;
    }; //if Vertex
    if (locations[i] == Compute) {
      samplerLayoutBinding.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
      pipeMark++;
    }; //if Compute
  }; //forloop
  if (pipeMark == 0) { gfxVars.descSetLayoutBinding.push_back(samplerLayoutBinding); }
  if (pipeMark == 1) { compVars.descSetLayoutBinding.push_back(samplerLayoutBinding); }
  if (pipeMark > 1) {
    gfxVars.descSetLayoutBinding.push_back(samplerLayoutBinding);
    compVars.descSetLayoutBinding.push_back(samplerLayoutBinding);
  };
}; //AddImageSampler

void GFXPipeline::AddStorageImage(ShaderType locations[], int_fast8_t shadCount, uint32_t binding) {
  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = binding;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

  uint_fast8_t pipeMark = 0;
  for (uint_fast8_t i = 0; i < shadCount; i++) {
    if (locations[i] == Fragment) {
      samplerLayoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
      if (pipeMark > 0) pipeMark++;
    }; //if Fragment
    if (locations[i] == Vertex) {
      samplerLayoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
      if (pipeMark > 0) pipeMark++;
    }; //if Vertex
    if (locations[i] == Compute) {
      samplerLayoutBinding.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
      pipeMark++;
    }; //if Compute
  }; //forloop
  if (pipeMark == 0) { gfxVars.descSetLayoutBinding.push_back(samplerLayoutBinding); }
  if (pipeMark == 1) { compVars.descSetLayoutBinding.push_back(samplerLayoutBinding); }
  if (pipeMark > 1) {
    gfxVars.descSetLayoutBinding.push_back(samplerLayoutBinding);
    compVars.descSetLayoutBinding.push_back(samplerLayoutBinding);
  };
}; //AddImageSampler

void GFXPipeline::AddPushConst(ShaderType* locations, int_fast8_t shadCount) {
  VkPushConstantRange pushConst;
  pushConst.offset = 0;
  pushConst.size = sizeof(PushConst);
  pushConst.stageFlags = 0;

  uint_fast8_t pipeMark = 0;
  for (uint_fast8_t i = 0; i <= shadCount; ++i) {
    if (locations[i] == Fragment) {
      pushConst.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
      if (pipeMark > 0) pipeMark++;
    }; //if Fragment
    if (locations[i] == Vertex) {
      pushConst.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
      if (pipeMark > 0) pipeMark++;
    }; //if Vertex
    if (locations[i] == Compute)
    {
      pushConst.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
      pipeMark++;
    }; //if Compute
  }; //forloop
  if (pipeMark == 0) { gfxVars.pushConstantRange.emplace_back(pushConst);}
  if (pipeMark == 1) { compVars.pushConstantRange.emplace_back(pushConst);}
  if (pipeMark > 1) {
    gfxVars.pushConstantRange.emplace_back(pushConst);
    compVars.pushConstantRange.emplace_back(pushConst);
  };// range
}; //PushConst

void GFXPipeline::SetBlendingAttachment(BlenderType type = None) {
  switch (type) {
  default:
  None: {
    gfxVars.colorBlendAttachInfo.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | 
      VK_COLOR_COMPONENT_G_BIT | 
      VK_COLOR_COMPONENT_B_BIT | 
      VK_COLOR_COMPONENT_A_BIT;
    gfxVars.colorBlendAttachInfo.blendEnable = VK_FALSE;
    gfxVars.colorBlendAttachInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    gfxVars.colorBlendAttachInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    gfxVars.colorBlendAttachInfo.colorBlendOp = VK_BLEND_OP_ADD;
    gfxVars.colorBlendAttachInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    gfxVars.colorBlendAttachInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    gfxVars.colorBlendAttachInfo.alphaBlendOp = VK_BLEND_OP_ADD;
  } return; //None
  Partial: {
    gfxVars.colorBlendAttachInfo.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT |
      VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT |
      VK_COLOR_COMPONENT_A_BIT;
    gfxVars.colorBlendAttachInfo.blendEnable = VK_TRUE;
    gfxVars.colorBlendAttachInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    gfxVars.colorBlendAttachInfo.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    gfxVars.colorBlendAttachInfo.colorBlendOp = VK_BLEND_OP_ADD;
    gfxVars.colorBlendAttachInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    gfxVars.colorBlendAttachInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    gfxVars.colorBlendAttachInfo.alphaBlendOp = VK_BLEND_OP_ADD;
  } return;

  }; //switch
}; //BlenderType