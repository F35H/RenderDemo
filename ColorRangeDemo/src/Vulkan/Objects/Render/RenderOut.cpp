#include <Vulkan/VulkanRender.h>

using namespace RenderIn;

ExternalProgram* externalProgram;

void resizeWindowCallback(GLFWwindow* window, int width, int height) {
  auto currentWindow = externalProgram->getCurrentWindow();
  currentWindow->windowResize = true;
} //ResizeCallback

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  Window* currentWindow = externalProgram->getCurrentWindow();
  
  if (action == GLFW_RELEASE) {
    switch (key) {
    case GLFW_KEY_LEFT:
      if (currentWindow->freeMove) {
        currentWindow->camDegree.y += 45;
      }
      else {
        currentWindow->modelIndex += 1;
      }; //else freeMove
      break;

    case GLFW_KEY_RIGHT:
      if (currentWindow->freeMove) {
        currentWindow->camDegree.y -= 45;
      }
      else {
        currentWindow->modelIndex += 1;
      }; //else freeMove
      break;

    case GLFW_KEY_UP:
      if (currentWindow->freeMove) {
        if (currentWindow->camPos.x > currentWindow->camPos.z) currentWindow->camDegree.z += 45;
        if (currentWindow->camPos.x < currentWindow->camPos.z) currentWindow->camDegree.x += 45;
      }
      else {
        currentWindow->modelIndex += 1;
      }; //else freeMove
      break;

    case GLFW_KEY_DOWN:
      if (currentWindow->freeMove) {
        if (currentWindow->camPos.x > currentWindow->camPos.z) currentWindow->camDegree.z -= 45;
        if (currentWindow->camPos.x < currentWindow->camPos.z) currentWindow->camDegree.x -= 45;
      }
      else {
        currentWindow->modelIndex += 1;
      }; //else freeMove
      break;

    case GLFW_KEY_A:
      if (currentWindow->freeMove) {
        currentWindow->camPos.x += 1;
      }
      else {
        currentWindow->modelIndex += 1;
      }; //else freeMove
      break;

    case GLFW_KEY_D:
      if (currentWindow->freeMove) {
        currentWindow->camPos.x -= 1;
      }
      else {
        currentWindow->modelIndex += 1;
      }; //else freeMove
      break;

    case GLFW_KEY_W:
      if (currentWindow->freeMove) {
        currentWindow->camPos.z += 1;
      };
      break;

    case GLFW_KEY_S:
      if (currentWindow->freeMove) {
        currentWindow->camPos.z -= 1;
      };
      break;

    case GLFW_KEY_SPACE:
      if (currentWindow->freeMove) {
        currentWindow->camPos.y += 1;
      };
      break;

    case GLFW_KEY_LEFT_SHIFT:
      if (currentWindow->freeMove) {
        currentWindow->camPos.y -= 1;
      };
      break;

    //Trigger Model Rotation
    case GLFW_KEY_R:
      if (currentWindow->rotateModel) currentWindow->rotateModel = false;
      else { currentWindow->rotateModel = true; };
      break;

    //Trigger Free Movement
    case GLFW_KEY_M: {
      if (currentWindow->freeMove) currentWindow->freeMove = false;
      else { currentWindow->freeMove = true; };
      break;
    }; //GLFW_KEY_M

    }; //switch


    if (currentWindow->modelIndex < 0) { currentWindow->modelIndex = currentWindow->allModels.size() - 1; };
    if (currentWindow->modelIndex > currentWindow->allModels.size() - 1) { currentWindow->modelIndex = 0; };

    currentWindow->currentModel = std::make_shared<Polytope>(currentWindow->allModels[currentWindow->modelIndex]);
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

ExternalProgram::ExternalProgram(int winNum) {
  VkApplicationInfo appInfo = {
    VK_STRUCTURE_TYPE_APPLICATION_INFO, //sType
    nullptr,                            //pNext
    "Render Engine",                    //pApplicationName
    VK_MAKE_VERSION(1,0,0),             //applicationVersion
    "No Engine",                        //Engine Name
    VK_MAKE_VERSION(1,0,0),             //engineVersion
    VK_API_VERSION_1_0                  //API Version
  }; //AppInfo


  const std::vector<const char*>deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  }; //DeviceExtensions


  //Create Instance
  glfwInit();

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef _DEBUG 
  extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

  createInfo.enabledExtensionCount = extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();

  createInfo.enabledLayerCount = 0;
  createInfo.pNext = nullptr;


  VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
#ifdef _DEBUG
  errorHandler->AttachDebug(RenderOut::ErrorHandler::ValidationLayerSupport, nullptr, nullptr, nullptr);
  errorHandler->AttachDebug(RenderOut::ErrorHandler::DebugMessengerInfo, &createInfo, nullptr, &debugInfo);
  errorHandler->AttachDebug(RenderOut::ErrorHandler::DebugMessengerCreation, nullptr, nullptr, nullptr);
#endif


  errorHandler->ConfirmSuccess(vkCreateInstance(&createInfo, nullptr, &vulkanInstance), "Creating Vulkan Instance!");


  //Create Windows and Surfaces
  currentWindow = 0;
  for (int i = 0; i < winNum; ++i) {
    windows.emplace_back(new Window(vulkanInstance));
  }; //for winNum


  //PICK PHYSICAL DEVICE
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

  errorHandler->ConfirmSuccess((deviceCount != 0)
    ? VK_SUCCESS : VK_ERROR_UNKNOWN, "Finding GPUs with Vulkan Support");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, devices.data());

  for (const auto& device : devices) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilyProperties) {
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        graphicsFamily = i;
      }; //if GraphicsBitSet

      VkBool32 presentSupport = false;
      for (int i = (windows.size() - 1); i >= 0; --i) {
        result = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, windows[i]->surface, &presentSupport);
        errorHandler->ConfirmSuccess(result, "Getting Physical Device Surface Support!");
        if (presentSupport == false) break;
      }; //windows

      if (presentSupport) {
        presentFamily = i;
      }; //if GraphicsBitSet

      if (graphicsFamily.has_value() && presentFamily.has_value()) break;

      ++i;
    } //for queueFamily in queueFamilyProperties


    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }; //for extension in Extension

    if (graphicsFamily.has_value() && presentFamily.has_value() && requiredExtensions.empty()) {
      physicalDevice = device;
      break;
    } //if device compatible
  } //for device in devices

  result = (physicalDevice != VK_NULL_HANDLE)
    ? VK_SUCCESS : VK_ERROR_UNKNOWN;
  errorHandler->ConfirmSuccess(result, "Finding Suitable GPU");

  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);


  result = VK_SUCCESS;

  //CREATE LOGICAL DEVICE
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = { graphicsFamily.value(), presentFamily.value() };

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  } //for queueFamily

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo deviceInfo{};
  deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceInfo.pEnabledFeatures = &deviceFeatures;
  deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
  deviceInfo.enabledLayerCount = 0;

#ifdef _DEBUG
  errorHandler->AttachDebug(
    RenderOut::ErrorHandler::LogicalDeviceLayers, nullptr, &deviceInfo, nullptr);
#endif

  errorHandler->ConfirmSuccess(
    vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device),
    "Creating Logical Device");


  //Get DepthBuffer Format
  for (VkFormat format : {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT}) {
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

    if (VK_IMAGE_TILING_OPTIMAL == VK_IMAGE_TILING_LINEAR
      && (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
      depthBufferFormat = format;
      break;
    }
    else if (VK_IMAGE_TILING_OPTIMAL == VK_IMAGE_TILING_OPTIMAL
      && (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
      depthBufferFormat = format;
      break;
    } //if correct format;
  } //for VkFormat

  if (!depthBufferFormat) errorHandler->ConfirmSuccess(
    VK_ERROR_FORMAT_NOT_SUPPORTED,
    "Failed to Find Supported Depth Buffer Format");


#ifdef _DEBUG 
  vkGetDeviceQueue(device, graphicsFamily.value(), 0, &gfxQueue);
  vkGetDeviceQueue(device, presentFamily.value(), 0, &presentQueue);
#endif
}; //ExternalInterface
void ExternalProgram::Activate() {
  externalProgram = this;
}; //Activate