#include <Vulkan/VulkanRender.h>

Vulkan::VulkanRender::VulkanRender() {
  using namespace RenderIn;
  using namespace CPUBuffer;

  //Put Vertex in a Namespace
  //Have mainloop be able to accept multiple semaphores

  //RenderOut
  errorHandler = std::make_unique<RenderOut::ErrorHandler>(RenderOut::ErrorHandler());
  auto externalProgram = std::make_shared<ExternalProgram>(ExternalProgram(1));
  auto polyFact = new Polytopes::PolytopeFactory();
  SwapChain* swapChain = new SwapChain(&externalProgram); //Compiler Error, don't make this a smart pointer
  auto gfxPipe = std::make_shared<GFXPipeline>(GFXPipeline(&externalProgram));
  auto gfxPipeDos = std::make_shared<GFXPipeline>(GFXPipeline(&externalProgram));
  auto uniFact = std::make_unique<UniformFactory>(UniformFactory());
  auto bufferFact = std::make_shared<BufferFactory>(BufferFactory(&externalProgram));
  auto syncFact = std::make_unique<GPUSyncFactory::SyncFactory>(GPUSyncFactory::SyncFactory(&externalProgram));
  externalProgram->Activate();

  externalProgram->getCurrentWindow()->allModels = polyFact->GetFileModels("Sponza//1.0");
  externalProgram->getCurrentWindow()->currentModel = std::make_shared<Polytope>(externalProgram->getCurrentWindow()->allModels[0]);

  //Generate
  swapChain->Activate(swapChain->Create);

  gfxPipe->AddShaderFile(gfxPipe->Vertex, "shaders/SPIR-5/vert.spv");
  gfxPipe->AddShaderFile(gfxPipe->Fragment, "shaders/SPIR-5/frag.spv");
  gfxPipe->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, pos));
  gfxPipe->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, color));
  gfxPipe->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, norm));
  gfxPipe->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, texPos));
  gfxPipe->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
  gfxPipe->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
  gfxPipe->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
  gfxPipe->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
  gfxPipe->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
  gfxPipe->IsStencil(false);
  gfxPipe->AddPushConst(gfxPipe->Fragment);
  gfxPipe->AddUniformBuffer(gfxPipe->Vertex);
  
  gfxPipe->AddImageSampler(gfxPipe->Fragment);
  gfxPipe->Activate(swapChain->renderPass);

  gfxPipeDos->AddShaderFile(gfxPipe->Vertex, "shaders/SPIR-5/Outlines/outline.vert.spv");
  gfxPipeDos->AddShaderFile(gfxPipe->Fragment, "shaders/SPIR-5/Outlines/outline.frag.spv");
  gfxPipeDos->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, pos));
  gfxPipeDos->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, color));
  gfxPipeDos->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, norm));
  gfxPipeDos->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, texPos));
  gfxPipeDos->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
  gfxPipeDos->AddPushConst(gfxPipe->Fragment);
  gfxPipeDos->IsStencil(true);
  gfxPipeDos->AddUniformBuffer(gfxPipe->Vertex);
  gfxPipeDos->AddImageSampler(gfxPipe->Fragment);
  gfxPipeDos->Activate(swapChain->renderPass);

  uniFact->Activate(static_cast<float>(swapChain->imageExtent.width) / static_cast<float>(swapChain->imageExtent.height));

  auto shadLin = 0;
  uniFact->UpdateRenderProperties(uniFact->LightingMisc, glm::vec4(shadLin, 0.0f, 0.0f, 0.0f));
  uniFact->UpdateRenderProperties(uniFact->AmbientLighting, glm::vec4(0.1f, 1.0f, 1.0f, 1.0f));

  std::vector<VkSemaphore> imageSemaphores;
  std::vector<VkSemaphore> presentSemaphores;
  std::vector<VkFence> qFences;

  for (auto model : externalProgram->getCurrentWindow()->allModels) {
    bufferFact->AddVerticeBuffer(&model);
    bufferFact->AddIndiceBuffer(&model);
    bufferFact->AddImageBuffer(model.texture.get());
  }; //Creating Buffers

  int i = 0;
  for (; i != numFrames; ++i) {
    imageSemaphores.emplace_back(syncFact->AddSemaphore());
    presentSemaphores.emplace_back(syncFact->AddSemaphore());
    qFences.emplace_back(syncFact->AddFence());

    bufferFact->AddUniformBuffer(sizeof(UniformBufferObject));
  }; //for everyFrmae

  bufferFact->AddCmdBuffers(6); //cpyCmdBuffer
  bufferFact->descPool.value()->Activate(numFrames, gfxPipe->descriptorSetLayout);

  //for (auto image : swapChain->texImages) {
  //  bufferFact->AddTextureBuffer(&image);
  //}; //texImages

  std::vector<VkClearValue> clearColor = {
    {{0.0f, 0.0f, 0.0f, 1.0f}},
    {{1.0f, 0.0f}}
  }; //clearColor 
  auto mainLoop = std::make_unique<MainLoop>(MainLoop(swapChain, &externalProgram, &clearColor));

  //MainLoop Functions
  while (!glfwWindowShouldClose(externalProgram->getCurrentWindow()->window)) {
    glfwPollEvents();
    mainLoop->waitSemaphores = { imageSemaphores[mainLoop->currentFrameIndex] };
    mainLoop->signalSemaphores = { presentSemaphores[mainLoop->currentFrameIndex] };
    mainLoop->fences = { qFences[mainLoop->currentFrameIndex] };

    mainLoop->AddGfxPipeline(&gfxPipe, 1);
    mainLoop->AddGfxPipeline(&gfxPipeDos, 0);
    
    mainLoop->AddPresentCmdBuffer(bufferFact->GetCommandBuffer(mainLoop->currentFrameIndex), 0);
    mainLoop->AddCpyCmdBuffer(bufferFact->GetCommandBuffer(numFrames + mainLoop->currentFrameIndex), 0);
    mainLoop->AddImageCmdBuffer(bufferFact->GetCommandBuffer(numFrames + numFrames + mainLoop->currentFrameIndex), 0);
    
    for (int_fast8_t i = 0; i < externalProgram->getCurrentWindow()->allModels.size(); ++i) {
      mainLoop->AddVerticeBuffer(&bufferFact->vertexBuffers[i], 
        bufferFact->vertexBuffers[i].second->GetSize(), 0);
      mainLoop->AddVerticeBuffer(&bufferFact->vertexBuffers[i], 
        bufferFact->vertexBuffers[i].second->GetSize(), 1);
      mainLoop->AddIndiceBuffer(&bufferFact->indexBuffers[i], 
        externalProgram->getCurrentWindow()->allModels[i].indices.size(), 0);
      mainLoop->AddIndiceBuffer(&bufferFact->indexBuffers[i], 
        externalProgram->getCurrentWindow()->allModels[i].indices.size(), 1);
      mainLoop->AddImageBuffer(&bufferFact->imageBuffers[i], 0);
    }; //models

    mainLoop->AddPushConst(uniFact->GetPushConst(), 1);
    mainLoop->AddPushConst(uniFact->GetPushConst(), 0);
    mainLoop->AddDescSet(bufferFact->GetDescriptorSet(mainLoop->currentFrameIndex), 0);
    mainLoop->AddDescSet(bufferFact->GetDescriptorSet(mainLoop->currentFrameIndex), 1);

    uniFact->RotateTrue = externalProgram->getCurrentWindow()->rotateModel;
    uniFact->CamDegree = &externalProgram->getCurrentWindow()->camDegree;
    uniFact->CamPos = &externalProgram->getCurrentWindow()->camPos;
    uniFact->FreeMove = externalProgram->getCurrentWindow()->freeMove;

    uniFact->UpdatePolledInformation();
    bufferFact->uniformBuffers[
      mainLoop->currentFrameIndex]->CopyData(uniFact->GetUniformBuffer());

      mainLoop->ActivateSyncedInput();
      mainLoop->ActivateCmdInput();
      mainLoop->ActivateSyncedOutput();
      mainLoop->ActivateRender();

      mainLoop->currentFrameIndex += 1;
      mainLoop->currentFrameIndex %= numFrames;
  }; //while window
  vkDeviceWaitIdle(externalProgram->device);
};