#include <Vulkan/VulkanRender.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

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
  auto bufferFact = std::make_shared<BufferFactory>(BufferFactory(&externalProgram));
  auto syncFact = std::make_unique<GPUSyncFactory::SyncFactory>(GPUSyncFactory::SyncFactory(&externalProgram));
  externalProgram->Activate();

  swapChain->Activate(swapChain->Create);



  std::vector<Polytope> polytopes = polyFact->GetFileModels("Stanford//bunny");
  std::unordered_map<std::string, Aether::WorldAether*> aether;
  std::vector<std::shared_ptr<Shaders::GfxShader>> shaders;
  std::vector<UniformFactory*> uniFact;
  std::vector<VkSemaphore> imageSemaphores;
  std::vector<VkSemaphore> presentSemaphores;
  std::vector<VkFence> qFences;

  uint_fast32_t pTopeIndex = 0;
  for (auto& polytope : polytopes) {
    using namespace Shaders;
    auto shadLin = 4;

    auto aetherOutline = std::pair<std::string, Aether::WorldBody*>("bunnyOutline", new Aether::WorldBody());
    aetherOutline.second->aetherPos = glm::mat4(1.0f);
    aetherOutline.second->aetherRot = glm::vec3(1.0f);
    aetherOutline.second->aetherTrans = glm::vec3(1.0f);
    aetherOutline.second->meshIndex = bufferFact->vertexBuffers.size();
    aetherOutline.second->textureIndex = 0;
    aetherOutline.second->descPoolIndex = bufferFact->descPools.size();
    aetherOutline.second->gfxPipelineIndex = shaders.size();
    aetherOutline.second->verticeCount = polytope.vertices.size();
    aetherOutline.second->indiceCount =  polytope.indices.size();
    aetherOutline.second->uniFactIndex = uniFact.size();
    aetherOutline.second->modifierVector.push_back(Aether::WorldAether::RefreshTexture);
    aetherOutline.second->type = Aether::WorldAether::WorldBody;
    aether.insert(aetherOutline);

    bufferFact->AddVerticeBuffer(&polytope);
    bufferFact->AddIndiceBuffer(&polytope);
    bufferFact->AddTextureImage(polytope.texture.get(), pTopeIndex);
    bufferFact->AddUniformBuffer(sizeof(UniformBufferObject), pTopeIndex);
    
    shaders.push_back(std::make_shared<OutlineShader>(OutlineShader(OutlineShader::NormalOutline,swapChain, &externalProgram)));
    uniFact.push_back(new UniformFactory());
    bufferFact->descPools[bufferFact->descPools.size() - 1]->Activate(shaders[shaders.size()-1]->shaderPipe->gfxVars.descriptorSetLayout);
    uniFact[uniFact.size() - 1]->Activate(static_cast<float>(swapChain->imageExtent.width) / static_cast<float>(swapChain->imageExtent.height));
    
    ++pTopeIndex;

    auto aetherProduct = std::pair<std::string, Aether::WorldBody*>("bunny", new Aether::WorldBody());
    aetherProduct.second->aetherPos = glm::mat4(1.0f);
    aetherProduct.second->aetherRot = glm::vec3(1.0f);
    aetherProduct.second->aetherTrans = glm::vec3(1.0f);
    aetherProduct.second->meshIndex = bufferFact->vertexBuffers.size();
    aetherProduct.second->textureIndex = 0;
    aetherProduct.second->descPoolIndex = bufferFact->descPools.size();
    aetherProduct.second->gfxPipelineIndex = shaders.size();
    aetherProduct.second->verticeCount = polytope.vertices.size();
    aetherProduct.second->indiceCount = polytope.indices.size();
    aetherProduct.second->uniFactIndex = uniFact.size();
    aetherProduct.second->modifierVector.push_back(Aether::WorldAether::RefreshTexture);
    aetherProduct.second->type = Aether::WorldAether::WorldBody;
    aether.insert(aetherProduct);

    bufferFact->AddVerticeBuffer(&polytope);
    bufferFact->AddIndiceBuffer(&polytope);
    bufferFact->AddTextureImage(polytope.texture.get(), pTopeIndex);
    bufferFact->AddUniformBuffer(sizeof(UniformBufferObject), pTopeIndex);

    uniFact.push_back(new UniformFactory());
    shaders.push_back(std::make_shared<ModelShader>(ModelShader(ModelShader::Default, swapChain, &externalProgram)));
    bufferFact->descPools[bufferFact->descPools.size() - 1]->Activate(shaders[shaders.size() - 1]->shaderPipe->gfxVars.descriptorSetLayout);
    uniFact[uniFact.size() - 1]->Activate(static_cast<float>(swapChain->imageExtent.width) / static_cast<float>(swapChain->imageExtent.height));
    uniFact[uniFact.size() - 1]->UpdateRenderProperties(UniformFactory::AmbientLighting, glm::vec4(0.1f, 1.0f, 1.0f, 1.0f));
    uniFact[uniFact.size() - 1]->UpdateRenderProperties(UniformFactory::LightingMisc, glm::vec4(shadLin, 0.0f, 0.0f, 0.0f));

    ++pTopeIndex;
  }; //Creating Buffers

  int i = 0;
  for (; i != numFrames; ++i) {
    imageSemaphores.emplace_back(syncFact->AddSemaphore());
    presentSemaphores.emplace_back(syncFact->AddSemaphore());
    qFences.emplace_back(syncFact->AddFence());

    //bufferFact->AddComputeImage(swapChain->imageExtent.height, swapChain->imageExtent.width, 1);
    //bufferFact->AddTextureImage(swapChain->imageExtent.height, swapChain->imageExtent.width, 2);
  }; //for everyFrmae

  bufferFact->AddCmdBuffers(6); //cpyCmdBuffer

  std::vector<VkClearValue> clearColor = {
    {{0.0f, 0.0f, 0.0f, 1.0f}},
    {{1.0f, 0.0f}}
  }; //clearColor 
  auto mainLoop = std::make_unique<MainLoop>(MainLoop(swapChain, &externalProgram, &clearColor));

  //MainLoop Functions
  while (!glfwWindowShouldClose(externalProgram->getCurrentWindow()->window)) {
    glfwPollEvents();
   
    //Initiate ECS
    
    for (auto& aetherBody : aether) {
      using namespace Aether;
      std::vector<uint32_t> modifierVector;
      for (auto& modifier : aetherBody.second->modifierVector) {
        switch (modifier) {
        //case WorldAether::TransformM: uniFact[aetherBody.second->uniFactIndex]->TransformM(aetherBody.second);  break;
        //case WorldAether::RotateM:    uniFact[aetherBody.second->uniFactIndex]->RotateM(aetherBody.second);     break;
        //case WorldAether::TeleportM:  uniFact[aetherBody.second->uniFactIndex]->TeleportM(aetherBody.second);   break;
        //case WorldAether::TeleportV:  uniFact[aetherBody.second->uniFactIndex]->TeleportV(aetherBody.second);   break;
        //case WorldAether::RotateV:    uniFact[aetherBody.second->uniFactIndex]->RotateV(aetherBody.second);     break;
        //case WorldAether::TransformV: uniFact[aetherBody.second->uniFactIndex]->TransformV(aetherBody.second);  break;



        case WorldAether::RefreshAsNormalOutline:
        case WorldAether::RefreshAsVerticeOutline:
        
        
        case WorldAether::LoadMesh:
        case WorldAether::LoadTexture: {
          std::unique_ptr<Texture> t = std::make_unique<Texture>(Texture(*aetherBody.second->texture));
          bufferFact->GetTextureImage(aetherBody.second->textureIndex, aetherBody.second->descPoolIndex)->first->Reset(t->imageSize, t->data);
        } break;
        case WorldAether::LoadShader:

        case WorldAether::RefreshShader:
        case WorldAether::RefreshMesh: {
        } 
        case WorldAether::RefreshTexture: {
          bufferFact->GetTextureImage(aetherBody.second->textureIndex, aetherBody.second->descPoolIndex)->second->Reset();
          bufferFact->descPools[aetherBody.second->descPoolIndex]->Activate(
            shaders[aetherBody.second->gfxPipelineIndex]->shaderPipe->gfxVars.descriptorSetLayout);
          modifierVector.push_back(WorldAether::InitiateTextureCopy);
        } break; //RefreshTexture

        case WorldAether::InitiateTextureCopy: {
          mainLoop->AddCpyTimage(bufferFact->GetTextureImage(aetherBody.second->textureIndex, aetherBody.second->descPoolIndex));
          modifierVector.push_back(WorldAether::CopyTexture);
        } break; //InitiateTextureCopy

        case WorldAether::CopyTexture: {
          mainLoop->AddCpyTimage(bufferFact->GetTextureImage(aetherBody.second->textureIndex, aetherBody.second->descPoolIndex));
          modifierVector.push_back(WorldAether::TerminateTextureCopy);
        } break; //WorldAether

        case WorldAether::TerminateTextureCopy: {
          mainLoop->AddCpyTimage(bufferFact->GetTextureImage(aetherBody.second->textureIndex, aetherBody.second->descPoolIndex));
        } break; //TerminateTexture
        }; //modifer
      }; //modifiers
    
      if (aetherBody.second->type == aetherBody.second->WorldBody) {
        uniFact[((WorldBody*)aetherBody.second)->uniFactIndex]->RotateTrue = externalProgram->getCurrentWindow()->rotateModel;
        uniFact[((WorldBody*)aetherBody.second)->uniFactIndex]->CamDegree = &externalProgram->getCurrentWindow()->camDegree;
        uniFact[((WorldBody*)aetherBody.second)->uniFactIndex]->CamPos = &externalProgram->getCurrentWindow()->camPos;
        uniFact[((WorldBody*)aetherBody.second)->uniFactIndex]->FreeMove = externalProgram->getCurrentWindow()->freeMove;
      
        if (((WorldBody*)aetherBody.second)->uniFactIndex == 0) uniFact[((WorldBody*)aetherBody.second)->uniFactIndex]->UpdatePolledInformationModel();
        if (((WorldBody*)aetherBody.second)->uniFactIndex == 1) uniFact[((WorldBody*)aetherBody.second)->uniFactIndex]->UpdatePolledInformationView();
      
        mainLoop->AddVerticeBuffer(&bufferFact->vertexBuffers[((WorldBody*)aetherBody.second)->meshIndex],
          ((WorldBody*)aetherBody.second)->verticeCount, mainLoop->verticeBuffers.size());
        mainLoop->AddIndiceBuffer(&bufferFact->indexBuffers[((WorldBody*)aetherBody.second)->meshIndex],
          ((WorldBody*)aetherBody.second)->indiceCount, mainLoop->indiceBuffers.size());
        mainLoop->AddPushConst(uniFact[((WorldBody*)aetherBody.second)->uniFactIndex]->GetPushConst(), mainLoop->pushConsts.size());

        bufferFact->GetUniformBuffer(0, aetherBody.second->descPoolIndex)->CopyData(uniFact[((WorldBody*)aetherBody.second)->uniFactIndex]->GetUniformBuffer());
      }; //if this
      //Rework as appropriate

      mainLoop->AddGfxPipeline(&shaders[aetherBody.second->gfxPipelineIndex]->shaderPipe, mainLoop->gfxPipelines.size());
      mainLoop->AddPresentDescSet(bufferFact->GetDescriptorSet(0, aetherBody.second->descPoolIndex), mainLoop->pesentDescSets.size());

      aetherBody.second->modifierVector = modifierVector;
    }; //for every aether

    mainLoop->waitSemaphore = imageSemaphores[mainLoop->currentFrameIndex];
    mainLoop->signalSemaphore = presentSemaphores[mainLoop->currentFrameIndex];
    mainLoop->fence = qFences[mainLoop->currentFrameIndex];
    
    mainLoop->cmdBuffer = bufferFact->GetCommandBuffer(mainLoop->currentFrameIndex);

    mainLoop->ActivateSyncedInput();
    mainLoop->ActivateCmdInput();
    mainLoop->TriggerRenderInput();
    mainLoop->TriggerCpyCmd();
    mainLoop->TerminateCmdInput();
    mainLoop->ActivateSyncedOutput();
    mainLoop->ActivateRender();

    mainLoop->ClearMainLoop();
    mainLoop->currentFrameIndex += 1;
    mainLoop->currentFrameIndex %= numFrames;
  }; //while window
  vkDeviceWaitIdle(externalProgram->device);
};