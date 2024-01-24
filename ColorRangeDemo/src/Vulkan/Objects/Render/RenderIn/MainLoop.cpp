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

void MainLoop::ActivateCmdInput() {
  vkResetCommandBuffer(cmdBuffer, 0);
  startCmdBuffer = { };
  startCmdBuffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkBeginCommandBuffer(cmdBuffer, &startCmdBuffer);
}; //ActivateCmdInput
void MainLoop::TerminateCmdInput() {
  vkEndCommandBuffer(cmdBuffer);
}; //TermiateCmdInput
void MainLoop::ActivateSyncedInput() {
  vkWaitForFences(externalProgram->device, 1, &fence, VK_TRUE, UINT64_MAX);
  result = vkAcquireNextImageKHR(
    externalProgram->device, swapChain->swapChain, UINT64_MAX, waitSemaphore, VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    swapChain->Activate(swapChain->Recreate);
    return;
  }; //If Out_Of_Date
  errorHandler->ConfirmSuccess(result, "Getting SwapChain Image in MainLoop");
  vkResetFences(externalProgram->device, 1, &fence);
}; //ActivateSyncedInput
void MainLoop::ActivateSyncedOutput() {
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &waitSemaphore;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmdBuffer;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &signalSemaphore;
  submitInfo.pNext = nullptr;

  result = vkQueueSubmit(externalProgram->gfxQueue, 1, &submitInfo, fence);
  errorHandler->ConfirmSuccess(result, "Sending GPU Draw Command");
}; //ActivateCommandOutput
void MainLoop::ActivateRender() {
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &signalSemaphore;
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
  startRenderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  startRenderPass.renderPass = swapChain->renderPass;
  startRenderPass.framebuffer = swapChain->frameBuffers[imageIndex].framebuffer;
  startRenderPass.renderArea.offset = { 0, 0 };
  startRenderPass.renderArea.extent = swapChain->imageExtent;
  startRenderPass.clearValueCount = static_cast<uint32_t>(clearColor.size());
  startRenderPass.pClearValues = clearColor.data();
  startRenderPass.pNext = nullptr;

  vkCmdBeginRenderPass(cmdBuffer, &startRenderPass, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdSetViewport(cmdBuffer, 0, 1, &cmdViewport);
  vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

  for (uint_fast8_t i = gfxPipelines.size() - 1; i < gfxPipelines.size();--i) {
    vkCmdBindPipeline(
      cmdBuffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS, 
      gfxPipelines[i]->gfxVars.graphicsPipeline);

    if (computeImages.size() > i) {
      vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, gfxPipelines[i]->compVars.pipeline);
      vkCmdDispatch(cmdBuffer, 1, 1, 1);
    }; //if computeImages

    if (verticeBuffers.size() > i) {
      if (verticeBuffers[i]->second->fromStagedBuffer) {
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(
          cmdBuffer, 0, 1,
          verticeBuffers[i]->second->GetBuffer(), offsets);
      } //if verticeBuffers
      else break;
    }; //has_value

    if (indiceBuffers.size() > i) {
      if (indiceBuffers[i]->second->fromStagedBuffer) {
        vkCmdBindIndexBuffer(
          cmdBuffer,
          *indiceBuffers[i]->second->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
      } //index Buffers
      else break;
    }; //has_value

    if (pushConsts.size() > i) {
      if (pushConsts[i].has_value()) {
        vkCmdPushConstants(
          cmdBuffer,
          gfxPipelines[i]->gfxVars.pipelineLayout,
          gfxPipelines[i]->gfxVars.pushConstantRange[0].stageFlags,
          0, gfxPipelines[i]->gfxVars.pushConstantRange[0].size, pushConsts[i].value()
        ); //VkCmdPushCOnst
      }; //PushConsts
    }; //pushConstss  

    if (pesentDescSets.size()> i) {
      if (pesentDescSets[i].has_value()) {
        vkCmdBindDescriptorSets(
          cmdBuffer,
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          gfxPipelines[i]->gfxVars.pipelineLayout, 0, 1,
          pesentDescSets[i].value(), 0, nullptr);
      }; //descSets
    }; //pushConsts

    if (computeDescSets.size() > i) {
      vkCmdBindDescriptorSets(
        cmdBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        gfxPipelines[i]->compVars.pipelineLayout, 0, 1,
        pesentDescSets[i].value(), 0, nullptr);
    }; //computeDesc

    if (indiceBuffers.size()> i && indiceBuffers[i]->second->fromStagedBuffer) {
      vkCmdDrawIndexed(
        cmdBuffer,
        indiceSizes[i], 30, 0, 0, 0);
    } //if inficeBuffers
    else if (verticeBuffers.size()> i && verticeBuffers[i]->second->fromStagedBuffer) {
      vkCmdDraw(cmdBuffer,
        verticeSizes[i], 1, 0, 0);
    } //else if vertices
    else {
      throw std::runtime_error("Unable to Draw: no buffers filled to draw");
    }; //else this

  }; //for Pipelines

  vkCmdEndRenderPass(cmdBuffer);
}; //TriggerRenderInput
void MainLoop::TriggerCpyCmd() {  
  size_t i;
  for (i = gfxPipelines.size() - 1; i < gfxPipelines.size(); --i) {
    if (verticeBuffers.size()>i &&
      !verticeBuffers[i]->second->fromStagedBuffer &&
      verticeBuffers[i]->first->writtenTo) {
      verticeCpy[i].size = verticeBuffers[i]->first->GetSize();
      
      vkCmdCopyBuffer(
        cmdBuffer,
        *verticeBuffers[i]->first->GetBuffer(),
        *verticeBuffers[i]->second->GetBuffer(),
        1, &verticeCpy[i]);
    
      verticeBuffers[i]->first->writtenTo = false;
      verticeBuffers[i]->second->fromStagedBuffer = true;
    }; //if VerticeBuffers

    if (indiceBuffers.size()>i &&
      !indiceBuffers[i]->second->fromStagedBuffer &&
      indiceBuffers[i]->first->writtenTo) {
      indiceCpy[i].size = indiceBuffers[i]->first->GetSize();

      vkCmdCopyBuffer(
        cmdBuffer,
        *indiceBuffers[i]->first->GetBuffer(),
        *indiceBuffers[i]->second->GetBuffer(),
        1, &indiceCpy[i]);

      indiceBuffers[i]->first->writtenTo = false;
      indiceBuffers[i]->second->fromStagedBuffer = true;
    }; //if indiceBuffers

    if (cpyTimages.size()>i) {
      if (cpyTimages[i]->second->readyToCpy) {
        imageCpy[i].bufferOffset = 0;
        imageCpy[i].bufferRowLength = 0;
        imageCpy[i].bufferImageHeight = 0;
        imageCpy[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCpy[i].imageSubresource.mipLevel = 0;
        imageCpy[i].imageSubresource.baseArrayLayer = 0;
        imageCpy[i].imageSubresource.layerCount = 1;
        imageCpy[i].imageOffset = { 0, 0, 0 };
        imageCpy[i].imageExtent = {
          cpyTimages[i]->second->width,
          cpyTimages[i]->second->height, 1 };

        vkCmdCopyBufferToImage(
          cmdBuffer,
          *cpyTimages[i]->first->GetBuffer(),
          *cpyTimages[i]->second->image,
          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          1, &imageCpy[i]
        ); //vkCMdCopyBuffer

        cpyTimages[i]->second->readyToCpy = false;
      } //if readyToCpy
      else {
        VkImageSubresourceRange subRange{};
        VkImageMemoryBarrier barrier{};
        subRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subRange.baseMipLevel = 0;
        subRange.levelCount = 1;
        subRange.baseArrayLayer = 0;
        subRange.layerCount = 1;

        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = cpyTimages[i]->second->oldLayout;
        barrier.newLayout = cpyTimages[i]->second->newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = *cpyTimages[i]->second->image;
        barrier.subresourceRange = subRange;

        if (cpyTimages[i]->second->oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
          cpyTimages[i]->second->newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
          barrier.srcAccessMask = 0;
          barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

          vkCmdPipelineBarrier(
            cmdBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr,
            0, nullptr,
            1, &barrier
          ); //vkCmdPipelineBarrier

          cpyTimages[i]->second->readyToCpy = true;
          cpyTimages[i]->second->oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
          cpyTimages[i]->second->newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        else if (cpyTimages[i]->second->oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
          cpyTimages[i]->second->newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
          
          barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
          barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

          vkCmdPipelineBarrier(
            cmdBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0, 0, nullptr,
            0, nullptr,
            1, &barrier
          ); //vkCmdPipelineBarrier

          cpyTimages[i]->second->oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
          cpyTimages[i]->second->newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
      }; //else
    } //if Stage Ready
  }; //i < size
}; //TriggerCpyCmd
void MainLoop::AddGfxPipeline(std::shared_ptr<GFXPipeline>* pipe, uint_fast8_t index) {
  if (gfxPipelines.size() <= index) { gfxPipelines.resize(index+1); gfxPipelines[index] = *pipe; }
  else { gfxPipelines[index] = *pipe; };
}; //AddGfxPipe
void MainLoop::AddPushConst(PushConst* pushConst, uint_fast8_t index) {
  if (pushConsts.size() <= index) { pushConsts.resize(index + 1); pushConsts[index] = pushConst; }
  else { pushConsts[index] = pushConst; };
}; //AddPushConst
void MainLoop::AddCompDescSet(VkDescriptorSet* set, uint_fast8_t index) {
  if (computeDescSets.size() <= index) { computeDescSets.resize(index + 1); computeDescSets[index] = set; }
  else { computeDescSets[index] = set; };
}; //AddDescSet
void MainLoop::AddPresentDescSet(VkDescriptorSet* set, uint_fast8_t index) {
  if (pesentDescSets.size() <= index) { pesentDescSets.resize(index + 1); pesentDescSets[index] = set; }
  else { pesentDescSets[index] = set; };
}; //AddDescSet
void MainLoop::AddVerticeBuffer(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>* buff, size_t verticeCount, uint_fast8_t index) {
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
void MainLoop::AddIndiceBuffer(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::ModelBuffer*>* buff, size_t verticeCount, uint_fast8_t index) {
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
void MainLoop::AddResetTimage(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::TextureImage*>* buff,Texture* t) {
  resetTimages.push_back(buff);
  resetTextures.push_back(t);
}; //AddResetTimage
void MainLoop::AddCpyTimage(std::pair<CPUBuffer::StageBuffer*, CPUBuffer::TextureImage*>*buff) {
  imageCpy.resize(imageCpy.size()+1);
  cpyTimages.push_back(buff);
}; //AddCpyTimage
void MainLoop::ClearMainLoop() {
  gfxPipelines.clear();

  verticeCpy.clear();
  indiceCpy.clear();
  imageCpy.clear();

  verticeBuffers.clear();
  indiceBuffers.clear();
  resetTextures.clear();
  resetTimages.clear();
  cpyTimages.clear();
  computeImages.clear();
  indiceSizes.clear();
  verticeSizes.clear();

  pesentDescSets.clear();
  computeDescSets.clear();
  pushConsts.clear();

}; //ClearMainLoop