#include <Vulkan/Objects/Render/RenderIn.h>

using namespace RenderIn;


GFXPipeline::GFXPipeline(std::shared_ptr<ExternalProgram>* eProgram) {
  externalProgram = *eProgram;
  
  shaders.resize(2);
  pipelineShaderInfo.resize(2);
  shaderModulesInfo.resize(2);

  dynamicStates = {
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

  if (shaderType == Vertex) vertBuffer = buffer;
  if (shaderType == Fragment) fragBuffer = buffer;

  file.close();
}; //AddShaderFile

void GFXPipeline::ClearPipeline() {
  pushConstantRange.clear();
  fragInput.clear();
  vertexInput.clear();
}; //ClearPipeline

void GFXPipeline::IsStencil(bool stencilTrue) {
  isStencil = stencilTrue;
}; //IsStencil

void GFXPipeline::Activate(VkRenderPass renderPass) {
  shaderModulesInfo[0].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModulesInfo[0].codeSize = vertBuffer.size();
  shaderModulesInfo[0].pCode = reinterpret_cast<const uint32_t*>(vertBuffer.data());

  shaderModulesInfo[1].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModulesInfo[1].codeSize = fragBuffer.size();
  shaderModulesInfo[1].pCode = reinterpret_cast<const uint32_t*>(fragBuffer.data());

  ShaderModuleCreation(static_cast<uint32_t>(shaderModulesInfo.size()) - 1);

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
  descLayoutInfo.bindingCount = descSetLayoutBinding.size();
  descLayoutInfo.pBindings = descSetLayoutBinding.data();
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

  colorBlendAttachInfo.colorWriteMask = 0xf;
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

  if (isStencil == true) {
    stencilState.compareOp = VK_COMPARE_OP_NOT_EQUAL;
    stencilState.failOp = VK_STENCIL_OP_KEEP;
    stencilState.passOp = VK_STENCIL_OP_REPLACE;
    stencilState.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencilInfo.depthTestEnable = VK_TRUE;
  } //isStencil is true
  else {
    stencilState.compareOp = VK_COMPARE_OP_ALWAYS;
    stencilState.failOp = VK_STENCIL_OP_KEEP;
    stencilState.passOp = VK_STENCIL_OP_REPLACE;
    stencilState.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencilInfo.depthTestEnable = VK_TRUE;
  }; //Stencil

  stencilState.reference = 1;
  stencilState.writeMask = 0xff;
  stencilState.compareMask = 0xff;

  depthStencilInfo.front = stencilState;
  depthStencilInfo.back = stencilState;
  depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilInfo.depthWriteEnable = VK_TRUE;
  depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
  depthStencilInfo.minDepthBounds = 0.0f;
  depthStencilInfo.maxDepthBounds = 1.0f;
  depthStencilInfo.stencilTestEnable = VK_TRUE;


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

void GFXPipeline::ShaderModuleCreation(int_fast8_t indice) {
  result = vkCreateShaderModule(externalProgram->device, &shaderModulesInfo[indice], nullptr, &shaders[indice]);
  errorHandler->ConfirmSuccess(result, "Creating Shader: " + indice);
  if (indice > 0) return ShaderModuleCreation(indice - 1);
}; //ShaderModuleCreation

void GFXPipeline::AddShaderBinding(ShaderType shaderType, uint32_t offsetorstride) {
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

void GFXPipeline::AddUniformBuffer(ShaderType shader) {
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = descSetLayoutBinding.size();
  uboLayoutBinding.descriptorCount = 1;
  if (shader == Fragment) uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  if (shader == Vertex) uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descSetLayoutBinding.push_back(uboLayoutBinding);
}; //AddUniformBuffer

void GFXPipeline::AddImageSampler(ShaderType shader) {
  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = descSetLayoutBinding.size();
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  if (shader == Fragment) samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  if (shader == Vertex) samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  descSetLayoutBinding.push_back(samplerLayoutBinding);
}; //AddImageSampler

void GFXPipeline::AddPushConst(ShaderType pushConstLocation) {
  VkPushConstantRange pushConst;
  pushConst.offset = 0;
  pushConst.size = sizeof(PushConst);

  if (pushConstLocation == ShaderType::Fragment) pushConst.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  if (pushConstLocation == ShaderType::Vertex) pushConst.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  pushConstantRange.emplace_back(pushConst);
}; //PushConst