#include <Vulkan/Objects/Render/RenderOut.h>

void resizeWindowCallback(GLFWwindow* window, int width, int height) {
  auto currentWindow = externalProgram->getCurrentWindow();
  currentWindow->windowResize = true;
} //ResizeCallback

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  auto currentWindow = externalProgram->getCurrentWindow();
  if (action == GLFW_RELEASE) {
    switch (key) {
    case GLFW_KEY_LEFT:
    case GLFW_KEY_A:
      currentWindow->modelIndex += 1;
      break;
    case GLFW_KEY_RIGHT:
    case GLFW_KEY_D:
      currentWindow->modelIndex -= 1;
      break;
    case GLFW_KEY_R:
      currentWindow->rotateModel = -true;
      break;
    }; //switch

    if (currentWindow->modelIndex < 0) { currentWindow->modelIndex = currentWindow->allModels.size() - 1; };
    if (currentWindow->modelIndex > currentWindow->allModels.size() - 1) { currentWindow->modelIndex = 0; };

    currentWindow->currentModel = std::make_shared<Polyhedra>(currentWindow->allModels[currentWindow->modelIndex]);
  }; //if (action == GLFW_RELEASE)
}; //keyboardCallback

void errorCallback(int error, const char* description) {
  switch (error) {
  default:
    throw std::runtime_error("GLFW UNKNOWN ERROR! " + std::string(description));
  case GLFW_NOT_INITIALIZED:
    throw std::runtime_error("GLFW_NOT_INITIALIZED! " + std::string(description));
  case GLFW_NO_CURRENT_CONTEXT:
    throw std::runtime_error("GLFW_NO_CURRENT_CONTEXT! " + std::string(description));
  case GLFW_INVALID_ENUM:
    throw std::runtime_error("GLFW_INVALID_ENUM! " + std::string(description));
  case GLFW_INVALID_VALUE:
    throw std::runtime_error("GLFW_INVALID_VALUE! " + std::string(description));
  case GLFW_OUT_OF_MEMORY:
    throw std::runtime_error("GLFW_OUT_OF_MEMORY! " + std::string(description));
  case GLFW_API_UNAVAILABLE:
    throw std::runtime_error("GLFW_API_UNAVAILABLE! " + std::string(description));
  case GLFW_VERSION_UNAVAILABLE:
    throw std::runtime_error("GLFW_VERSION_UNAVAILABLE! " + std::string(description));
  case GLFW_PLATFORM_ERROR:
    throw std::runtime_error("GLFW_PLATFORM_ERROR! " + std::string(description));
  case GLFW_FORMAT_UNAVAILABLE:
    throw std::runtime_error("GLFW_FORMAT_UNAVAILABLE! " + std::string(description));
  }; //errorSwitch
}; //errorCallback