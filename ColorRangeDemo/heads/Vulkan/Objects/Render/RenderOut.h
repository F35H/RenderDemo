#pragma once

#include "vulkan/GeneralIncludes.h"
#include "vulkan/Objects/ModelTypes/ModelFactory.h"


namespace RenderOut {

  struct ErrorHandler {
    enum DebugFunction {
      DebugMessengerInfo = 0,
      ValidationLayerSupport,
      LogicalDeviceLayers,
      DebugMessengerCreation
    }; //DebugFunction

    void ConfirmSuccess(VkResult result, std::string section) {
      switch (result) {
      case VK_ERROR_UNKNOWN:
      default:
        throw std::runtime_error("Unknown Error! " + section);
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

  void resizeWindowCallback(GLFWwindow* window, int width, int height) {
    auto currentWindow = &externalProgram->getCurrentWindow();
    currentWindow->windowResize = true;
  } //ResizeCallback

  void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window currentWindow = externalProgram->getCurrentWindow();
    
    if (action == GLFW_RELEASE) {
      switch (key) {
      case GLFW_KEY_LEFT:
      case GLFW_KEY_A:
        currentWindow.currentModel += 1;
        break;
      case GLFW_KEY_RIGHT:
      case GLFW_KEY_D:
        currentWindow.currentModel -= 1;
        break;
      case GLFW_KEY_R:
        currentWindow.currentModel = true;
        break;
      }; //switch


      if (currentWindow.currentModel < 0) { currentWindow.currentModel = currentWindow.allModels.size() - 1; };
      if (currentWindow.currentModel > currentWindow.allModels.size() - 1) { currentWindow.currentModel = 0; };
    }; //if (action == GLFW_RELEASE)
  }; //keyboardCallback

  struct Window {
    GLFWwindow* window;
    VkSurfaceKHR surface;

    std::string windowName = "RenderAgent";
    uint32_t winWidth = 800;
    uint32_t winHeight = 800;

    int windowResize = false;

    bool rotateModel = false;
    int currentModel = 0;
  public:
    Window(VkInstance instance) {
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

      window = glfwCreateWindow(
        winWidth, 
        winHeight,
        windowName.c_str(),
        nullptr, nullptr
      ); //window

      glfwSetFramebufferSizeCallback(window, resizeWindowCallback);
      glfwSetKeyCallback(window, keyboardCallback);
      

      //Create Surface
      VkWin32SurfaceCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
      createInfo.hwnd = glfwGetWin32Window(window);
      createInfo.hinstance = GetModuleHandle(nullptr);

      VkWin32SurfaceCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
      createInfo.hwnd = glfwGetWin32Window(window);
      createInfo.hinstance = GetModuleHandle(nullptr);


      errorHandler->ConfirmSuccess(
        vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, surface),
        "Creating Window Surface");

      glfwCreateWindowSurface(instance, window, nullptr, surface);
    };
    ~Window() {
      glfwDestroyWindow(window);
    }
  }; //Window

  struct ExternalProgram {
    std::vector<Window> windows;
    int currentWindow;

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkInstance vulkanInstance;

    uint32_t graphicsFamily = NULL;
    uint32_t presentFamily = NULL;

    VkPhysicalDeviceMemoryProperties memProperties;
    VkFormatProperties               formatProperties;
    VkSurfaceCapabilitiesKHR         scAbilities;
    std::vector<VkSurfaceFormatKHR>  scFormats;
    std::vector<VkPresentModeKHR>    scModes;


    VkFormat                         depthBufferFormat;

    ExternalProgram(int winNum) {      
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
      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;

      uint32_t glfwExtensionCount = 0;
      const char** glfwExtensions;
      glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

      std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

      if (_DEBUG) {
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      } //VkParams::DEBUG

      createInfo.enabledExtensionCount = extensions.size();
      createInfo.ppEnabledExtensionNames = extensions.data();

      createInfo.enabledLayerCount = 0;
      createInfo.pNext = nullptr;


      VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
      if (_DEBUG) {
        errorHandler->AttachDebug(ErrorHandler::ValidationLayerSupport, nullptr, nullptr, nullptr);
        errorHandler->AttachDebug(ErrorHandler::DebugMessengerInfo, &createInfo, nullptr, &debugInfo);
        errorHandler->AttachDebug(ErrorHandler::DebugMessengerCreation, nullptr, nullptr, nullptr);
      }; //VkPArams::Debug


      errorHandler->ConfirmSuccess(vkCreateInstance(&createInfo, nullptr, &vulkanInstance), "Creating Vulkan Instance!");
      
      
      //Create Windows and Surfaces
      windows.emplace_back(Window(vulkanInstance));
      currentWindow = 0;

      for (int i = 0; i <= winNum; ++i) {
        windows.emplace_back(Window(vulkanInstance));
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
          for (Window window : windows) {
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, window.surface, &presentSupport);
            if (presentSupport == false) break;
          }; //windows

          if (presentSupport) {
            presentFamily = i;
          }; //if GraphicsBitSet

          if (graphicsFamily != NULL && presentFamily != NULL) break;

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

        if (graphicsFamily != NULL && presentFamily != NULL && requiredExtensions.empty()) {
          physicalDevice = device;
          break;
        } //if device compatible
      } //for device in devices


      errorHandler->ConfirmSuccess((physicalDevice != VK_NULL_HANDLE)
        ? VK_SUCCESS : VK_ERROR_UNKNOWN, "Failed to Find a Suitable GPU");

      vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);



      //CREATE LOGICAL DEVICE
      std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
      std::set<uint32_t> uniqueQueueFamilies = { graphicsFamily, presentFamily };

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

      if (_DEBUG) 
        errorHandler->AttachDebug(
          ErrorHandler::LogicalDeviceLayers, nullptr, &deviceInfo, nullptr);

      errorHandler->ConfirmSuccess(
        vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device),
        "Creating Logical Device");


      //Get DepthBuffer Format
      for (VkFormat format : {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}) {
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




      //Move This
      if (_DEBUG) {
        vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
        vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);
      }; // if DEBUG
    

    }; //ExternalInterface




    Window* getCurrentWindow() {
      return &windows[currentWindow];
    }; //getCurrentWindow



  }; //ExternalInterface

  static ExternalProgram*   externalProgram;
  static ErrorHandler*      errorHandler;
  //static ModelFactory*      modelFactory;
}; //RenderIn