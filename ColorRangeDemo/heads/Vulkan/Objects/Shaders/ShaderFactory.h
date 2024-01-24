#pragma once

#include <Vulkan/Objects/Shaders/Shaders.h>

struct ShaderFactory {
  std::vector<Shaders::GfxShader*> shaders;

  ShaderFactory() {

  }; //ShaderFactory
}; //ShaderFactory