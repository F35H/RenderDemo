#pragma once

#include "ConsoleUI/UISingletons.h"
#include "Objects/Render/RenderIn.h"

const inline INT numFrames = 3;

namespace Vulkan{
  struct VulkanRender {
  VulkanRender() {
    using namespace RenderIn;
    using namespace CPUBuffer;

    //Put Vertex in a Namespace
    //Have mainloop be able to accept multiple semaphores

    //RenderOut
    errorHandler    = std::make_unique<ErrorHandler>(ErrorHandler());
    externalProgram = new ExternalProgram(1);
    auto polyFact = std::make_unique<PolygonFactory>(PolygonFactory());
    SwapChain* swapChain = new SwapChain(); //Compiler Error, don't make this a smart pointer
    auto gfxPipe = std::make_shared<GFXPipeline>(GFXPipeline());
    auto uniFact = std::make_unique<UniformFactory>(UniformFactory());
    auto bufferFact = std::make_shared<BufferFactory>(BufferFactory());
    auto syncFact = std::make_unique<SyncFactory>(SyncFactory());
    auto mainLoop = std::make_unique<MainLoop>(MainLoop());

    externalProgram->getCurrentWindow()->allModels = polyFact->GetPolyhedra();
    externalProgram->getCurrentWindow()->currentModel = std::make_shared<Polyhedra>(externalProgram->getCurrentWindow()->allModels[0]);

    //Generate
    swapChain->Activate(swapChain->Create);
    gfxPipe->AddShaderFile(gfxPipe->Vertex, "shaders/SPIR-5/vert.spv");
    gfxPipe->AddShaderFile(gfxPipe->Fragment, "shaders/SPIR-5/frag.spv");
    gfxPipe->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, pos));
    gfxPipe->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, color));
    gfxPipe->AddShaderBinding(gfxPipe->Vertex, offsetof(Vertex, norm));
    gfxPipe->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
    gfxPipe->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
    gfxPipe->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
    gfxPipe->AddShaderBinding(gfxPipe->Fragment, sizeof(Vertex));
    gfxPipe->AddPushConst();
    gfxPipe->Activate(swapChain->renderPass);

    uniFact->Actiate(static_cast<float>(swapChain->imageExtent.width)/ static_cast<float>(swapChain->imageExtent.height));

    switch (UIValues::gamutLineage) {
    case UILineageEnums::GamutLineage::CIE:
      switch (UIValues::gamutChoice) {
      }; //GamutChoice
      break;
    case UILineageEnums::GamutLineage::ComputerVision:
      switch (UIValues::gamutChoice) {
      case UIRenderEnums::Gamuts::CVLineage::IHLS:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(5.0f, 0.0f, 0.0f, 0.0f));
        break;
      }; //GamutChoice
      break;
    case UILineageEnums::GamutLineage::RGB:
      switch (UIValues::gamutChoice) {
      case UIRenderEnums::Gamuts::RGBLineage::RGB:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
        break;
      case UIRenderEnums::Gamuts::RGBLineage::HWB:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(4.0f, 0.0f, 0.0f, 0.0f));
        break;
      case UIRenderEnums::Gamuts::RGBLineage::HSV:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(3.0f, 0.0f, 0.0f, 0.0f));
        break;
      case UIRenderEnums::Gamuts::RGBLineage::HSL:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(2.0f, 0.0f, 0.0f, 0.0f));
        break;
      case UIRenderEnums::Gamuts::RGBLineage::CMY:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(6.0f, 0.0f, 0.0f, 0.0f));
        break;
      }; //GamutChoice
      break;
    case UILineageEnums::GamutLineage::Television:
      switch (UIValues::gamutChoice) {
      case UIRenderEnums::Gamuts::TVLineage::YIQ:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(7.0f, 0.0f, 0.0f, 0.0f));
        break;
      case UIRenderEnums::Gamuts::TVLineage::YDbDr:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(8.0f, 0.0f, 0.0f, 0.0f));
        break;
      case UIRenderEnums::Gamuts::TVLineage::YUV:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(9.0f, 0.0f, 0.0f, 0.0f));
        break;
      case UIRenderEnums::Gamuts::TVLineage::YUVHD:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(10.0f, 0.0f, 0.0f, 0.0f));
        break;
      case UIRenderEnums::Gamuts::TVLineage::BT709:
        uniFact->UpdateRenderProperties(uniFact->MaterialMisc, glm::vec4(11.0f, 0.0f, 0.0f, 0.0f));
        break;
      }; //GamutChoice
      break;
    }; //switch
    float shadLin;
    switch (UIValues::shadingLineage) {
    case UILineageEnums::ShadingLineage::NoShading:
      shadLin = 0.f;
      break;
    case UILineageEnums::ShadingLineage::NPR:
      switch (UIValues::shadingChoice) {
      case UIRenderEnums::Shading::NPR::GOOCH:
        shadLin = 5.f;
        break;
      case UIRenderEnums::Shading::NPR::GOOCHPHONG:
        shadLin = 7.f;
        break;
      case UIRenderEnums::Shading::NPR::TOON:
        shadLin = 6.f;
        break;
      }; //Shading Choice
      break;
    case UILineageEnums::ShadingLineage::Traditional:
      switch (UIValues::shadingChoice) {
      case UIRenderEnums::Shading::Traditional::FLAT:
        shadLin = 1.f;
        break;
      case UIRenderEnums::Shading::Traditional::GOURAND:
        shadLin = 2.f;
        break;
      case UIRenderEnums::Shading::Traditional::PHONG:
        shadLin = 3.f;
        break;
      case UIRenderEnums::Shading::Traditional::BLINPHONG:
        shadLin = 4.f;
        break;
      }; //Shading Choice
      break;
    }; //shadingLineage
    float filtLin;
    switch (UIValues::filterLineage) {
    case UILineageEnums::FilterLineage::NoFilter:
      filtLin = 0.f;
      break;
    case UILineageEnums::FilterLineage::ColorDificiency:
      switch (UIValues::filterChoice) {
      case UIRenderEnums::Filter::ColorDefficiency::DEUTERANOPIA:
        filtLin = 1.f;
        break;
      case UIRenderEnums::Filter::ColorDefficiency::GRAYSCALE:
        filtLin = 4.f;
        break;
      case UIRenderEnums::Filter::ColorDefficiency::TRITANOPIA:
        filtLin = 3.f;
        break;
      case UIRenderEnums::Filter::ColorDefficiency::PROTENOPIA:
        filtLin = 2.f;
        break;
      }; //switch FilterChoice
    }; //FilterLineage

    uniFact->UpdateRenderProperties(uniFact->LightingMisc, glm::vec4(shadLin, filtLin, 0.0f, 0.0f));
    uniFact->UpdateRenderProperties(uniFact->AmbientLighting, glm::vec4(0.1f, 1.0f, 1.0f, 1.0f));

    std::vector<VkSemaphore> imageSemaphores;
    std::vector<VkSemaphore> presentSemaphores;
    std::vector<VkFence> qFences;

    int i = 0;
    for (;i != numFrames;++i) {
      imageSemaphores.emplace_back(syncFact->AddSemaphore());
      presentSemaphores.emplace_back(syncFact->AddSemaphore());
      qFences.emplace_back(syncFact->AddFence());
    }; //for everyFrmae

    bufferFact->AddUniformBuffers(numFrames, gfxPipe->descriptorSetLayout);
    bufferFact->AddCmdBuffers(numFrames);

    for (auto model : externalProgram->getCurrentWindow()->allModels) {
      bufferFact->AddVerticeBuffer(&model);
      bufferFact->AddIndiceBuffer(&model);
    }; //Creating Buffers

    std::vector<VkClearValue> clearColor = {
      {{0.0f, 0.0f, 0.0f, 1.0f}},
      {{1.0f, 0.0f}}
    }; //clearColor

    mainLoop->bufferFactory = bufferFact;
    mainLoop->swapChain = swapChain;
    mainLoop->gfxPipeline = gfxPipe;

    //MainLoop Functions
    while (!glfwWindowShouldClose(externalProgram->getCurrentWindow()->window)) {
      glfwPollEvents();
      mainLoop->waitSemaphores = {imageSemaphores[mainLoop->currentFrameIndex]};
      mainLoop->signalSemaphores = {presentSemaphores[mainLoop->currentFrameIndex]};
      mainLoop->fences = {qFences[mainLoop->currentFrameIndex]};

      uniFact->UpdatePolledInformation();
      bufferFact->uniformBuffers[
        mainLoop->currentFrameIndex].CopyData(uniFact->GetUniformBuffer());

      mainLoop->ActivateSyncedInput();
      mainLoop->ActivateCmdInput(uniFact->GetPushConst(), clearColor);
      mainLoop->ActivateSyncedOutput();
      mainLoop->ActivateRender();

      mainLoop->currentFrameIndex += 1;
      mainLoop->currentFrameIndex %= numFrames;
    }; //while window
    vkDeviceWaitIdle(externalProgram->device);

  }; //Vulkan
}; //Vulkan

}; //Vulkan