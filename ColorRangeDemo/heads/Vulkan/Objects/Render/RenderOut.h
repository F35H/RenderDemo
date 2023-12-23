#pragma once

#include "vulkan/Objects/ModelTypes/ModelFactory.h"

void errorCallback(int error, const char* description);

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
} //Debug Callback

namespace RenderOut {
  struct ErrorHandler {

  enum DebugFunction {
    DebugMessengerInfo = 0,
    ValidationLayerSupport,
    LogicalDeviceLayers,
    DebugMessengerCreation
  }; //DebugFunction

  ErrorHandler() {
    glfwSetErrorCallback(errorCallback);
  }; //ErrorHandler

  void ConfirmSuccess(VkResult result, std::string section) {
    switch (result) {
    case VK_SUBOPTIMAL_KHR:
      std::cout << "VK_SUBOPTIMAL_KHR " << section << "\n";
    case VK_SUCCESS:
      return;
    case VK_ERROR_UNKNOWN:
    default:
      throw std::runtime_error("Unknown Error! " + section + std::to_string(static_cast<int>(result)));
    case VK_NOT_READY:
      throw std::runtime_error("VK_NOT_READY! " + section);
    case VK_TIMEOUT:
      throw std::runtime_error("VK_TIMEOUT! " + section);
    case VK_EVENT_SET:
      throw std::runtime_error("VK_EVENT_SET! " + section);
    case VK_INCOMPLETE:
      throw std::runtime_error("VK_INCOMPLETE! " + section);
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      throw std::runtime_error("VK_ERROR_OUT_OF_HOST_MEMORY! " + section);
    case VK_ERROR_INITIALIZATION_FAILED:
      throw std::runtime_error("VK_ERROR_INITIALIZATION_FAILED! " + section);
    case VK_ERROR_DEVICE_LOST:
      throw std::runtime_error("VK_ERROR_DEVICE_LOST! " + section);
    case VK_ERROR_MEMORY_MAP_FAILED:
      throw std::runtime_error("VK_ERROR_MEMORY_MAP_FAILED! " + section);
    case VK_ERROR_LAYER_NOT_PRESENT:
      throw std::runtime_error("VK_ERROR_LAYER_NOT_PRESENT! " + section);
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      throw std::runtime_error("VK_ERROR_EXTENSION_NOT_PRESENT! " + section);
    case VK_ERROR_FEATURE_NOT_PRESENT:
      throw std::runtime_error("VK_ERROR_FEATURE_NOT_PRESENT! " + section);
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      throw std::runtime_error("VK_ERROR_INCOMPATIBLE_DRIVER! " + section);
    case VK_ERROR_TOO_MANY_OBJECTS:
      throw std::runtime_error("VK_ERROR_TOO_MANY_OBJECTS! " + section);
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      throw std::runtime_error("VK_ERROR_FORMAT_NOT_SUPPORTED! " + section);
    case VK_ERROR_FRAGMENTED_POOL:
      throw std::runtime_error("VK_ERROR_FRAGMENTED_POOL! " + section);
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      throw std::runtime_error("VK_ERROR_OUT_OF_DEVICE_MEMORY! " + section);
    }; //switch (result)
  }; //ErrorHandler

  void AttachDebug(DebugFunction debugFunc, VkInstanceCreateInfo* instanceCreateInfo, VkDeviceCreateInfo* createInfo, VkDebugUtilsMessengerCreateInfoEXT* debugInfo) {
    static std::vector<const char*> layerName = { "VK_LAYER_KHRONOS_validation" };

    switch (debugFunc) {
    case DebugMessengerInfo: {
      instanceCreateInfo->enabledLayerCount = layerName.size();
      instanceCreateInfo->ppEnabledLayerNames = layerName.data();

      debugInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
      debugInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
      debugInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      debugInfo->pfnUserCallback = debugCallback;

      instanceCreateInfo->pNext = debugInfo;
      return;
    } //DebugMessengerInfo 

    case ValidationLayerSupport: {
      uint32_t layerCount;
      vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

      std::vector<VkLayerProperties> availableLayers(layerCount);
      vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

      bool layerFound = false;
      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName[0], layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        } //if layer found
      } //for all available layers

      if (!layerFound) {
        throw std::runtime_error("Layer Not Compatible!");
      } // if layerFound

      return;
    }; //ValidationLayerSupport

    case LogicalDeviceLayers: {
      createInfo->enabledLayerCount = layerName.size();
      createInfo->ppEnabledLayerNames = layerName.data();
      return;
    } //Logic DeviceLayers

    case DebugMessengerCreation: {

    }; //DebugMessengerCreation
    }; //switch(debugFunc)
  }; //AttachDebug
}; //ErrorHandler
};

static std::unique_ptr <RenderOut::ErrorHandler>      errorHandler;

struct Window {
  GLFWwindow* window;
  VkSurfaceKHR surface;

  std::string windowName = "RenderAgent";
  uint32_t winWidth = 800;
  uint32_t winHeight = 800;

  int windowResize = false;

  glm::vec3 camDegree = glm::vec3(0.0f);
  glm::vec3 camPos = glm::vec3(0.0f);

  bool freeMove = false;
  bool rotateModel = false;
  int modelIndex = 0;

  std::vector<Polytope> allModels;
  std::shared_ptr<Polytope> currentModel;
public:

  void AddKeyboardCallBacks();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  Window(VkInstance instance) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(
      winWidth,
      winHeight,
      windowName.c_str(),
      nullptr, nullptr
    ); //window

    AddKeyboardCallBacks();

    //Create Surface
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = glfwGetWin32Window(window);
    createInfo.hinstance = GetModuleHandle(nullptr);

    errorHandler->ConfirmSuccess(
      vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface),
      "Creating Window Surface");

    glfwCreateWindowSurface(instance, window, nullptr, &surface);
  }; //Window

  bool IsMinimized() {
    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    if (frameWidth != 0 && frameHeight != 0) return false;
    glfwWaitEvents();
    return true;
  }; //IsMinimized

  ~Window() {
    glfwDestroyWindow(window);
  }
}; //Window

struct ExternalProgram {
  std::vector<Window*> windows;
  int currentWindow;

  VkDevice device;
  VkPhysicalDevice physicalDevice;
  VkInstance vulkanInstance;

  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  VkPhysicalDeviceMemoryProperties memProperties;
  VkFormatProperties               formatProperties;
  VkSurfaceCapabilitiesKHR         scAbilities;
  std::vector<VkSurfaceFormatKHR>  scFormats;
  std::vector<VkPresentModeKHR>    scModes;

  VkQueue                          gfxQueue;
  VkQueue                          presentQueue;
  VkFormat                         depthBufferFormat;

  VkResult result;

  ExternalProgram(int winNum);

  void Activate();

  Window* getCurrentWindow() {
    return windows[currentWindow];
  }; //getCurrentWindow
}; //ExternalInterface

void resizeWindowCallback(GLFWwindow* window, int width, int height); 
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods); 
void inline Window::AddKeyboardCallBacks() {
  glfwSetFramebufferSizeCallback(window, resizeWindowCallback);
  glfwSetKeyCallback(window, keyboardCallback);
}; //AddKeyboardCallbacks
