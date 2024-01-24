#pragma once

#include <Vulkan/Objects/Render/RenderIn.h>

namespace Shaders {
  struct GfxShader {
    std::shared_ptr<RenderIn::GFXPipeline> shaderPipe;
    GfxShader() = default;
  }; //Shader

  struct LensShader : GfxShader {
    enum ShaderTypes {
      Default = 0
    }; //ShaderTypes

    LensShader(ShaderTypes shaderType, RenderIn::SwapChain* swapChain, std::shared_ptr<ExternalProgram>* externalProgram) {
      using namespace RenderIn;
      shaderPipe = std::make_shared<GFXPipeline>(GFXPipeline(externalProgram));

      switch (shaderType) {
      default: throw std::runtime_error("Rong Shader Type: OutlineShader");
      case Default: {
        shaderPipe->AddShaderFile(GFXPipeline::Vertex, "shaders/SPIR-5/2DFrame.vert.spv");
        shaderPipe->AddShaderFile(GFXPipeline::Fragment, "shaders/SPIR-5/2DFrame.frag.spv");
        shaderPipe->IsStencil(false);
        shaderPipe->AddImageSampler(new GFXPipeline::ShaderType(GFXPipeline::Fragment), 1, 0);
        shaderPipe->Activate(swapChain->renderPass);
      } return;
      }//switch
    }; //LensShaders
  }; //CameraShader

  struct ModelShader : GfxShader {
    enum ShaderTypes {
      Default = 0,
      Instanced
    }; //ShaderTypes

    ModelShader(ShaderTypes shaderType, RenderIn::SwapChain* swapChain, std::shared_ptr<ExternalProgram>* externalProgram) {
      using namespace RenderIn;
      shaderPipe = std::make_shared<GFXPipeline>(GFXPipeline(externalProgram));

      switch (shaderType) {
      default: throw std::runtime_error("Rong Shader Type: Default Model");
      case Default: {
        shaderPipe->AddShaderFile(GFXPipeline::Vertex, "shaders/SPIR-5/vert.spv");
        shaderPipe->AddShaderFile(GFXPipeline::Fragment, "shaders/SPIR-5/frag.spv");
        //shaderPipe->AddShaderFile(shaderPipe->Compute, "shaders/SPIR-5/Filters/grayscale.comp.spv");
        shaderPipe->AddShaderBinding(offsetof(Vertex, color));
        shaderPipe->AddShaderBinding(offsetof(Vertex, pos));
        shaderPipe->AddShaderBinding(offsetof(Vertex, vertNorm));
        shaderPipe->AddShaderBinding(offsetof(Vertex, texPos));
        shaderPipe->AddBindingDesc(sizeof(Vertex));
        shaderPipe->AddPushConst(new GFXPipeline::ShaderType(GFXPipeline::Fragment), 1);
        shaderPipe->AddUniformBuffer(new GFXPipeline::ShaderType(GFXPipeline::Vertex), 1, 0);
        shaderPipe->AddImageSampler(new GFXPipeline::ShaderType(GFXPipeline::Fragment), 1, 1);
        //shaderPipe->AddStorageImage(&compShader, 1, 0);
        //shaderPipe->AddStorageImage(&compShader, 1, 1);
        shaderPipe->SetBlendingAttachment(GFXPipeline::Partial);
        shaderPipe->Activate(swapChain->renderPass);
      case Instanced: {
        shaderPipe->AddShaderFile(GFXPipeline::Vertex, "shaders/SPIR-5/vert.spv");
        shaderPipe->AddShaderFile(GFXPipeline::Fragment, "shaders/SPIR-5/frag.spv");
        //shaderPipe->AddShaderFile(shaderPipe->Compute, "shaders/SPIR-5/Filters/grayscale.comp.spv");
        shaderPipe->AddShaderBinding(offsetof(QuadInstance, color));
        shaderPipe->AddShaderBinding(offsetof(QuadInstance, firstPos));
        shaderPipe->AddShaderBinding(offsetof(QuadInstance, scndPos));
        shaderPipe->AddShaderBinding(offsetof(QuadInstance, thirdPos));
        shaderPipe->AddShaderBinding(offsetof(QuadInstance, fourthPos));
        shaderPipe->AddShaderBinding(offsetof(QuadInstance, faceNorm));
        shaderPipe->AddShaderBinding(offsetof(QuadInstance, texPos));
        shaderPipe->AddBindingDesc(sizeof(QuadInstance), VK_VERTEX_INPUT_RATE_INSTANCE);
        shaderPipe->AddPushConst(new GFXPipeline::ShaderType(GFXPipeline::Fragment), 1);
        shaderPipe->AddUniformBuffer(new GFXPipeline::ShaderType(GFXPipeline::Vertex), 1, 0);
        shaderPipe->AddImageSampler(new GFXPipeline::ShaderType(GFXPipeline::Fragment), 1, 1);
        //shaderPipe->AddStorageImage(&compShader, 1, 0);
        //shaderPipe->AddStorageImage(&compShader, 1, 1);
        shaderPipe->SetBlendingAttachment(GFXPipeline::Partial);
        shaderPipe->Activate(swapChain->renderPass);
      } break;
      } return;
      }; //switch
    }; //DefaultShader
  }; //DefaultShader

  struct OutlineShader : GfxShader {
    enum ShaderTypes {
      VerticeOutline = 0,
      NormalOutline
    }; //ShaderTypes
  
    OutlineShader(ShaderTypes shaderType, RenderIn::SwapChain* swapChain, std::shared_ptr<ExternalProgram>* externalProgram) {
      using namespace RenderIn;
      shaderPipe = std::make_shared<GFXPipeline>(GFXPipeline(externalProgram));

      switch (shaderType) {
      default: throw std::runtime_error("Rong Shader Type: OutlineShader");
      case VerticeOutline: {
        shaderPipe->AddShaderFile(GFXPipeline::Vertex, "shaders/SPIR-5/Outlines/outlineVertice/outlineVertice.vert.spv");
        shaderPipe->AddShaderFile(GFXPipeline::Fragment, "shaders/SPIR-5/Outlines/outlineVertice/outlineVertice.frag.spv");
        shaderPipe->AddShaderBinding(offsetof(Vertex, color));
        shaderPipe->AddShaderBinding(offsetof(Vertex, pos));
        shaderPipe->AddShaderBinding(offsetof(Vertex, vertNorm));
        shaderPipe->AddShaderBinding(offsetof(Vertex, texPos));
        shaderPipe->AddBindingDesc(sizeof(Vertex));
        shaderPipe->AddPushConst(new GFXPipeline::ShaderType(GFXPipeline::Fragment), 1);
        shaderPipe->IsStencil(true);
        shaderPipe->AddUniformBuffer(new GFXPipeline::ShaderType(GFXPipeline::Vertex), 1, 0);
        shaderPipe->AddImageSampler(new GFXPipeline::ShaderType(GFXPipeline::Fragment), 1, 1);
        shaderPipe->SetBlendingAttachment(GFXPipeline::None);
        shaderPipe->Activate(swapChain->renderPass);
      } return;
      case NormalOutline: {
        shaderPipe->AddShaderFile(GFXPipeline::Vertex, "shaders/SPIR-5/Outlines/outlineNormal/outlineVertice.vert.spv");
        shaderPipe->AddShaderFile(GFXPipeline::Fragment, "shaders/SPIR-5/Outlines/outlineNormal/outlineVertice.frag.spv");
        shaderPipe->AddShaderBinding(offsetof(Vertex, color));
        shaderPipe->AddShaderBinding(offsetof(Vertex, pos));
        shaderPipe->AddShaderBinding(offsetof(Vertex, vertNorm));
        shaderPipe->AddShaderBinding(offsetof(Vertex, texPos));
        shaderPipe->AddBindingDesc(sizeof(Vertex));
        shaderPipe->AddPushConst(new GFXPipeline::ShaderType(GFXPipeline::Fragment), 1);
        shaderPipe->IsStencil(true);
        shaderPipe->AddUniformBuffer(new GFXPipeline::ShaderType(GFXPipeline::Vertex), 1, 0);
        shaderPipe->AddImageSampler(new GFXPipeline::ShaderType(GFXPipeline::Fragment), 1, 1);
        shaderPipe->SetBlendingAttachment(GFXPipeline::None);
        shaderPipe->Activate(swapChain->renderPass);
      } return;
      }; //outlines
    }; //DefaultShader
  }; //DefaultShader

}; //Shaders
