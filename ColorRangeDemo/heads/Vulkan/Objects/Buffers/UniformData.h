#pragma once

#include "Vulkan/Objects/Render/RenderOut.h"

struct PushConst {
  glm::mat4 matProp;
  //[0][0] //Color Gamut

  glm::mat4 lighting;
  //[0][0]    //Lighting Choice
  //[1][0]    //Ambient Light Strength
  //[1][1-3]  //LightColor
}; //PushConst

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
}; //UniformBufferObject

namespace UniformData {
  class Uniform {
  public:
    void* uniform;

    void* GetUniform() {
      return uniform;
    }; //GetUniform
  }; //Polyhedras

  struct Properties : Uniform {
    Properties() {
      uniform = new PushConst();
    }; //Properties
  }; //Material

  //Note for Later - Look into how to move this assignment onto the GPU instead;
  struct Position : Uniform {
    Position(float aspect) {
      auto Uniform = new UniformBufferObject();
      Uniform->model = glm::mat4(1.0f);
      Uniform->view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      Uniform->proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
      uniform = Uniform;
    }; //Material
  }; //Material
}; //Uniforms


struct UniformFactory {
  std::chrono::steady_clock::time_point previousTime;
  float degreeIncrement = 10;
  float degree = 720;
  float degreeLimit = 720;
  float delta;

  std::vector<UniformData::Uniform*> Uniforms;

  bool RotateTrue       = false;
  bool FreeMove         = false;
  glm::vec3* CamPos;
  glm::vec3* CamDegree;

  enum UniformType {
    Properties = 0,
    Position
  }; //Uniforms

  enum UniformProperties {
    LightingMisc = 0,
    AmbientLighting,
    MaterialMisc
  }; //UniformProperties

  UniformFactory() = default;
  UniformFactory(float aspect) {
    Uniforms = {
      new UniformData::Properties(),
      new UniformData::Position(aspect)
    }; //Uniforms
  }; //Uniform Factory

  void Activate(float aspect) {
    Uniforms = {
      new UniformData::Properties(),
      new UniformData::Position(aspect)
    }; //Uniforms
  }; //Activate

  void UpdateRenderProperties(UniformProperties prop, glm::vec4 properties) {
    switch (prop) {
    case MaterialMisc: {
      PushConst* Properties = static_cast<PushConst*>(Uniforms[UniformType::Properties]->uniform);
      Properties->matProp[0] = properties;
    }
                     return;
    case LightingMisc: {
      PushConst* Properties = static_cast<PushConst*>(Uniforms[UniformType::Properties]->uniform);
      Properties->lighting[0] = properties;
    } //LightingMisc
                     return;
    case AmbientLighting: {
      PushConst* Properties = static_cast<PushConst*>(Uniforms[UniformType::Properties]->uniform);
      Properties->lighting[1] = properties;
    } //LightingMisc
                        return;
    }; //switch
  }; //UpdateRenderProperties

  void UpdatePolledInformationView() {
    auto cT = std::chrono::high_resolution_clock::now();
    delta = std::chrono::duration<float,
      std::chrono::seconds::period>(previousTime -
        cT).count();

    previousTime = cT;


    if (RotateTrue) {
    auto rot = glm::mat4_cast(
      glm::normalize(glm::angleAxis(glm::radians(90.f * delta), glm::vec3(0, 1, 0))));

      static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform)->model *= rot;

      if (degree < degreeLimit) { degree += delta * degreeIncrement; };
      if (degree >= degreeLimit) { degree = 0; };
    }; //if (RotateTrue)
    
    if (FreeMove) {
      auto yRot = glm::mat4_cast(glm::normalize(glm::angleAxis(glm::radians(CamDegree->y * delta), glm::vec3(0.0f, 1.0f, 0.0f))));
      auto xRot = glm::mat4_cast(glm::normalize(glm::angleAxis(glm::radians(CamDegree->x * delta), glm::vec3(1.0f, 0.0f, 0.0f))));
      auto zRot = glm::mat4_cast(glm::normalize(glm::angleAxis(glm::radians(CamDegree->z * delta), glm::vec3(1.0f, 0.0f, 1.0f))));
      auto rot = yRot * xRot * zRot;
      glm::translate(static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform)->model, (*CamPos * delta));
      static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform)->model *= rot;
    }; //FreeMove

  }; //UpdatePolledInformation

  void UpdatePolledInformationModel() {
    auto cT = std::chrono::high_resolution_clock::now();
    delta = std::chrono::duration<float,
      std::chrono::seconds::period>(previousTime -
        cT).count();

    previousTime = cT;


    if (RotateTrue) {
      auto rot = glm::mat4_cast(
        glm::normalize(glm::angleAxis(glm::radians(90.f * delta), glm::vec3(0, 1, 0))));

      static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform)->view *= rot;

      if (degree < degreeLimit) { degree += delta * degreeIncrement; };
      if (degree >= degreeLimit) { degree = 0; };
    }; //if (RotateTrue)

    if (FreeMove) {
      auto yRot = glm::mat4_cast(glm::normalize(glm::angleAxis(glm::radians(CamDegree->y * delta), glm::vec3(0.0f, 1.0f, 0.0f))));
      auto xRot = glm::mat4_cast(glm::normalize(glm::angleAxis(glm::radians(CamDegree->x * delta), glm::vec3(1.0f, 0.0f, 0.0f))));
      auto zRot = glm::mat4_cast(glm::normalize(glm::angleAxis(glm::radians(CamDegree->z * delta), glm::vec3(1.0f, 0.0f, 1.0f))));
      auto rot = yRot * xRot * zRot;
      glm::translate(static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform)->view, (*CamPos * delta));
      static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform)->view *= rot;
    }; //FreeMove

  }; //UpdatePolledInformation

  UniformBufferObject* GetUniformBuffer() {
    return static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform);
  }; //GetUniform

  PushConst* GetPushConst() {
    return static_cast<PushConst*>(Uniforms[UniformType::Properties]->uniform);
  }; //GetUniform
}; //UniformFactory


//struct UniformFactory {
//  std::shared_ptr<ExternalProgram> externalProgram;
//  VkResult result;
//
//  std::vector<PushConst> pushConsts;
//  std::vector<UniformBufferObject> uniBuffers;
//  
//  float aspectRatio;
//  UniformFactory() = default;
//
//  void Activate(float aspect) {
//    aspectRatio = aspect;
//  }; //Activate
//
//  void CreatePushConst() {
//    pushConsts.resize(pushConsts.size() + 1);
//    pushConsts[pushConsts.size() - 1] = PushConst();
//  }; //CreatePushConst
//
//  void CreateUBO() {
//    uniBuffers.resize(uniBuffers.size() + 1);
//    uniBuffers[uniBuffers.size() - 1].model = glm::mat4(1.0f);
//    uniBuffers[uniBuffers.size() - 1].view = glm::lookAt(
//      glm::vec3(0.0f, 0.0f, 3.0f), 
//      glm::vec3(0.0f, 0.0f, 0.0f), 
//      glm::vec3(0.0f, 1.0f, 0.0f));
//    uniBuffers[uniBuffers.size() - 1].proj = glm::perspective(
//      glm::radians(45.0f), 
//      aspectRatio, 0.1f, 10.0f);
//  }; //CreaeUBO
//
//  void UpdateView(uint_fast8_t index, glm::mat4 matrix) {
//    uniBuffers[index].view = matrix;
//  }; //UpdateView
//  void UpdateModel(uint_fast8_t index, glm::mat4 matrix) {
//    uniBuffers[index].model = matrix;
//  }; //UpdateModel
//
//  void UpdatePushConst() {
//
//  }; //UpdateUniBuff
//}; //newUniformFactory