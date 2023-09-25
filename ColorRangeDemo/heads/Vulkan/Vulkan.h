#pragma once

#include "Objects/Render/RenderIn.h"

struct Vulkan {
  Vulkan() {
    using namespace RenderIn;
    using namespace RenderOut;

    //RenderOut
    errorHandle     = new ErrorHandler();
    externalProgram = new ExternalProgram();
    
    //RenderIn

  }; //Vulkan
}; //Vulkan