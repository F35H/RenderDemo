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

  char RotateTrue = 0;

  enum UniformType {
    Properties = 0,
    Position
  }; //Uniforms

  enum UniformProperties {
    LightingMisc = 0,
    AmbientLighting,

    MaterialMisc
  }; //UniformProperties

  UniformFactory(float aspect) {
    Uniforms = {
      new Uniforms::Properties(),
      new Uniforms::Position(aspect)
    }; //Uniforms
  }; //Uniform Factory

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

  void UpdatePolledInformation() {
    auto cT = std::chrono::high_resolution_clock::now();

    delta = std::chrono::duration<float,
      std::chrono::seconds::period>(previousTime -
        cT).count();

    previousTime = cT;

    if (RotateTrue) {
      if (degree <= degreeLimit / 2 && RotateTrue) {
        static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform)->model =
          glm::rotate(glm::mat4(1.0f), glm::radians(degree), glm::vec3(0.0f, 0.0f, 1.0f));
      }
      if (RotateTrue) {
        static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform)->model =
          glm::rotate(glm::mat4(1.0f), glm::radians(degree), glm::vec3(1.0f, 1.0f, 0.0f));
      }

      if (degree < degreeLimit) { degree += delta * degreeIncrement; };
      if (degree >= degreeLimit) { degree = 0; };
    }; //if (RotateTrue)

  }; //UpdatePolledInformation

  UniformBufferObject* GetUniformBuffer() {
    return static_cast<UniformBufferObject*>(Uniforms[UniformType::Position]->uniform);
  }; //GetUniform

  PushConst* GetProperties() {
    return static_cast<PushConst*>(Uniforms[UniformType::Properties]->uniform);
  }; //GetUniform
}; //UniformFactory


