#pragma once

#include "Vulkan/Objects/Shaders/ShaderFactory.h"

namespace Aether {
  struct WorldAether {
    std::vector<uint32_t> modifierVector = {};
    std::string name = {};
    glm::mat4 aetherPos = {};
    glm::vec3 aetherRot = {};
    glm::vec3 aetherTrans = {};

    std::string* texture;
    uint32_t textureIndex;
    std::string* fragShader;
    std::string* vertShader;
    std::string* compShader;
    
    uint32_t descPoolIndex;
    uint32_t gfxPipelineIndex;

    uint32_t type;

    enum types {
      WorldBody = 0,
      LensBody,
    }; //types

    enum modifiers {
      //MovementModel

      TransformM = 0,
      RotateM,
      TeleportM,

      //MovementView
      RotateV,
      TransformV,
      TeleportV,

      //Outline

      RefreshAsNormalOutline,
      RefreshAsVerticeOutline,

      //RegularModel

      RefreshAsDefaultModel,

      //Loading

      LoadMesh,
      LoadTexture,
      LoadShader,

      //Refresh

      RefreshMesh,
      RefreshTexture,
      RefreshShader,

      //Copy

      InitiateTextureCopy,
      CopyTexture,
      TerminateTextureCopy

    }; //modifiers

    enum shaderModifers {
    }; //modifiers
  }; //WorldAether

  struct WorldBody : WorldAether {
    glm::mat4 view;
    std::string* mesh;
    uint32_t uniFactIndex;
    uint32_t compImageIndex;
    uint32_t meshIndex;

    uint32_t indiceCount;
    uint32_t verticeCount;

    WorldBody() = default;
  }; //WorldBody

  struct WorldLens : WorldAether {
    glm::mat4 perspective;

    WorldLens() {

    }; //WorldLens
  }; //Camera

}; //namespace Aether