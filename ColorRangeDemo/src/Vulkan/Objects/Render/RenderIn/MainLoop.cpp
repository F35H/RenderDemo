#include <Vulkan/Objects/Render/RenderIn.h>

using namespace RenderIn;

MainLoop::MainLoop(SwapChain* swapChainCtor, std::shared_ptr<ExternalProgram>* eProgram, std::vector<VkClearValue>* clear) {
  externalProgram = *eProgram;
  clearColor = *clear;

  startCmdBuffer = {};
  cmdViewport = {};
  scissor = {};
  presentInfo = {};
  gfxPipelines = {};
  signalSemaphores = {};
  waitSemaphores = {};
  fences = {};

  swapChain = swapChainCtor;

  cmdViewport.x = 0.0f;
  cmdViewport.y = 0.0f;
  cmdViewport.width = static_cast<float>(swapChain->imageExtent.width);
  cmdViewport.height = static_cast<float>(swapChain->imageExtent.height);
  cmdViewport.minDepth = 0.0f;
  cmdViewport.maxDepth = 1.0f;

  scissor.offset = { 0, 0 };
  scissor.extent = swapChain->imageExtent;
}; //MainLoop
void MainLoop::ActivateSyncedInput() {
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
void MainLoop::ActivateCmdInput() {
  TriggerRenderInput();
  TriggerCpyCmd();
}; //ActivateCommandInput
void MainLoop::ActivateSyncedOutput() {
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
void MainLoop::ActivateRender() {
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
void MainLoop::TriggerRenderInput() {
  if (!recurseIndex) recurseIndex = presentCmdBuffers.size() - 1;
  if (recurseIndex.value() > presentCmdBuffers.size()) { recurseIndex.reset(); return; };

  //Begin
  vkResetCommandBuffer(presentCmdBuffers[recurseIndex.value()], 0);

  startRenderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  startRenderPass.renderPass = swapChain->renderPass;
  startRenderPass.framebuffer = swapChain->frameBuffers[imageIndex].framebuffer;
  startRenderPass.renderArea.offset = { 0, 0 };
  startRenderPass.renderArea.extent = swapChain->imageExtent;
  startRenderPass.clearValueCount = static_cast<uint32_t>(clearColor.size());
  startRenderPass.pClearValues = clearColor.data();
  startRenderPass.pNext = nullptr;

  startCmdBuffer = { };
  startCmdBuffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  result = vkBeginCommandBuffer(presentCmdBuffers[recurseIndex.value()], &startCmdBuffer);
  errorHandler->ConfirmSuccess(result, "Beginning CmdBuffer");

  vkCmdBeginRenderPass(presentCmdBuffers[recurseIndex.value()], &startRenderPass, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdSetViewport(presentCmdBuffers[recurseIndex.value()], 0, 1, &cmdViewport);
  vkCmdSetScissor(presentCmdBuffers[recurseIndex.value()], 0, 1, &scissor);

  for (uint_fast8_t i = gfxPipelines.size() - 1; i < gfxPipelines.size();--i) {
    vkCmdBindPipeline(
      presentCmdBuffers[recurseIndex.value()], 
      VK_PIPELINE_BIND_POINT_GRAPHICS, 
      gfxPipelines[i]->graphicsPipeline);

    if (verticeBuffers.size() > i) {
      if (verticeBuffers[i]->second.cpyBool) {
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(
          presentCmdBuffers[recurseIndex.value()], 0, 1,
          verticeBuffers[i]->second.GetBuffer(), offsets);
      } //if verticeBuffers
      else break;
    }; //has_value

    if (indiceBuffers.size() > i) {
      if (indiceBuffers[i]->second.cpyBool) {
        vkCmdBindIndexBuffer(
          presentCmdBuffers[recurseIndex.value()],
          *indiceBuffers[i]->second.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
      } //index Buffers
      else break;
    }; //has_value

    if (pushConsts.size() > i) {
      if (pushConsts[i].has_value()) {
        vkCmdPushConstants(
          presentCmdBuffers[recurseIndex.value()],
          gfxPipelines[i]->pipelineLayout,
          gfxPipelines[i]->pushConstantRange[0].stageFlags,
          0, gfxPipelines[i]->pushConstantRange[0].size, pushConsts[i].value()
        ); //VkCmdPushCOnst
      }; //PushConsts
    }; //pushConstss  

    if (descSets.size()> i) {
      if (descSets[i].has_value()) {
        vkCmdBindDescriptorSets(
          presentCmdBuffers[recurseIndex.value()],
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          gfxPipelines[i]->pipelineLayout, 0, 1,
          &descSets[i].value(), 0, nullptr);
      }; //descSets
    }; //pushConsts

    if (indiceBuffers.size()> i && indiceBuffers[i]->second.cpyBool) {
      vkCmdDrawIndexed(
        presentCmdBuffers[recurseIndex.value()],
        indiceSizes[i], 1, 0, 0, 0);
    } //if inficeBuffers
    else if (verticeBuffers.size()> i && verticeBuffers[i]->second.cpyBool) {
      vkCmdDraw(presentCmdBuffers[recurseIndex.value()],
        verticeSizes[i], 1, 0, 0);
    } //else if vertices
    else {
      throw std::runtime_error("Unable to Draw: no buffers filled to draw");
    }; //else this

  }; //for Pipelines

  vkCmdEndRenderPass(presentCmdBuffers[recurseIndex.value()]);
  result = vkEndCommandBuffer(presentCmdBuffers[recurseIndex.value()]);
  errorHandler->ConfirmSuccess(result, "Ending CmdBuffer");

  recurseIndex.value() -= 1;
  TriggerRenderInput();
}; //TriggerRenderInput
void MainLoop::TriggerCpyCmd() {
  if (!recurseIndex) recurseIndex = cpyCmdBuffers.size() - 1;
  if (recurseIndex.value() > cpyCmdBuffers.size()) { recurseIndex.reset(); return; };

  vkResetCommandBuffer(cpyCmdBuffers[recurseIndex.value()], 0);

  startCmdBuffer = { };
  startCmdBuffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkBeginCommandBuffer(cpyCmdBuffers[recurseIndex.value()], &startCmdBuffer);
  
  size_t i;
  for (i = gfxPipelines.size() - 1; i < gfxPipelines.size(); --i) {
    if (verticeBuffers.size()>i) {
      verticeCpy[i].size = verticeBuffers[i]->first.GetSize();
      
      vkCmdCopyBuffer(
        cpyCmdBuffers[recurseIndex.value()],
        *verticeBuffers[i]->first.GetBuffer(),
        *verticeBuffers[i]->second.GetBuffer(),
        1, &verticeCpy[i]);
    
      verticeBuffers[i]->second.cpyBool = true;
    }; //if VerticeBuffers

    if (indiceBuffers.size()>i) {
      indiceCpy[i].size = indiceBuffers[i]->first.GetSize();

      vkCmdCopyBuffer(
        cpyCmdBuffers[recurseIndex.value()],
        *indiceBuffers[i]->first.GetBuffer(),
        *indiceBuffers[i]->second.GetBuffer(),
        1, &indiceCpy[i]);

      indiceBuffers[i]->second.cpyBool = true;
    }; //if indiceBuffers
  }; //i < size

  for (i = swapChain->texImages.size(); i < swapChain->texImages.size();--i) {
    vkCmdCopyBufferToImage(
      cpyCmdBuffers[recurseIndex.value()],
      *textureStages[i].GetBuffer(),
      swapChain->texImages[i],
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1, &swapChain->texCpy[i]
    ); //vkCMdCopyBuffer
  }; //swapChain texImages

  vkEndCommandBuffer(cpyCmdBuffers[recurseIndex.value()]);
  recurseIndex.value() -= 1;
  TriggerCpyCmd();
}; //TriggerCpyCmd
void MainLoop::AddGfxPipeline(std::shared_ptr<GFXPipeline>* pipe, uint_fast8_t index) {
  if (gfxPipelines.size() <= index) { gfxPipelines.resize(index+1); gfxPipelines[index] = *pipe; }
  else { gfxPipelines[index] = *pipe; };
}; //AddGfxPipe
void MainLoop::AddCpyCmdBuffer(VkCommandBuffer buff, uint_fast8_t index) {
  if (cpyCmdBuffers.size() <= index) { cpyCmdBuffers.resize(index + 1); cpyCmdBuffers[index] = buff; }
  else { cpyCmdBuffers[index] = buff; };
}; //AddCpyCmdBuffer;
void MainLoop::AddPresentCmdBuffer(VkCommandBuffer buff, uint_fast8_t index) {
  if (presentCmdBuffers.size() <= index) { presentCmdBuffers.resize(index + 1); presentCmdBuffers[index] = buff; }
  else { presentCmdBuffers[index] = buff; };
}; //AddPresentCmdBuffer;
void MainLoop::AddPushConst(PushConst* pushConst, uint_fast8_t index) {
  if (pushConsts.size() <= index) { pushConsts.resize(index + 1); pushConsts[index] = pushConst; }
  else { pushConsts[index] = pushConst; };
}; //AddPushConst
void MainLoop::AddDescSet(VkDescriptorSet set, uint_fast8_t index) {
  if (descSets.size() <= index) { descSets.resize(index + 1); descSets[index] = set; }
  else { descSets[index] = set; };
}; //AddDescSet
void MainLoop::AddVerticeBuffer(std::pair<CPUBuffer::StageBuffer, CPUBuffer::ModelBuffer>* buff, size_t verticeCount, uint_fast8_t index) {
  if (verticeBuffers.size() <= index) {
    verticeCpy.resize(index + 1);
    verticeBuffers.resize(index + 1);
    verticeSizes.resize(index + 1);
    verticeBuffers[index] = buff;
    verticeSizes[index] = verticeCount;
  }
  else {
    verticeBuffers[index] = buff;
    verticeSizes[index] = verticeCount;
  };
}; //AddVerticeBuffer
void MainLoop::AddIndiceBuffer(std::pair<CPUBuffer::StageBuffer, CPUBuffer::ModelBuffer>* buff, size_t verticeCount, uint_fast8_t index) {
  if (indiceBuffers.size() <= index) {
    indiceCpy.resize(index + 1);
    indiceBuffers.resize(index + 1);
    indiceSizes.resize(index + 1);
    indiceBuffers[index] = buff;
    indiceSizes[index] = verticeCount;
  }
  else {
    indiceBuffers[index] = buff;
    indiceSizes[index] = verticeCount;
  };
}; //AddVerticeBuffer
