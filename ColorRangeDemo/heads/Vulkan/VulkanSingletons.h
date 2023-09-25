#pragma once




#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <optional>
#include <set>

#include <Vulkan/Objects/Factories.h>

static struct VkParams {
  //Application Params
  static inline constexpr const char* winName = "Color Experiment";
  static inline constexpr int framesPerLoop = 2;

  static inline constexpr VkApplicationInfo appInfo = {
    VK_STRUCTURE_TYPE_APPLICATION_INFO, //sType
    nullptr,                            //pNext
    "Hello Triangle",                   //pApplicationName
    VK_MAKE_VERSION(1,0,0),             //applicationVersion
    "No Engine",                        //Engine Name
    VK_MAKE_VERSION(1,0,0),             //engineVersion
    VK_API_VERSION_1_0                  //API Version
  }; //AppInfo

  //Debug Params
#ifdef NDEBUG
  static inline constexpr char DEBUG = 0;
#else
  static inline constexpr char DEBUG = 1;
#endif

  static inline enum {
    winWidth = 800,
    winHeight = 800
  }; //enum
  
  //Application States
  static int resizeFrameBuffer;
  static int currentPolytope;

  //Application Variables
  static std::vector<VkClearValue> clearColor;
}; //VkParams

static struct GLFW {
  static GLFWwindow* window;
  static void GenerateWindow(void);
  static void TerminateWindow(void);
  static void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
  static std::pair<const char**, uint32_t> getGLFWExtensions(VkInstanceCreateInfo* createInfo);
  static bool Ready();
}; //GLFW

struct VkWindow {
  enum DebugFunction {
    ValidationLayerSupport,
    DebugMessengerInfo,
    DebugMessengerCreation,
    LogicalDeviceLayers
  }; //DebugFunction

  enum SwapChainFunction {
    CleanUpSwapChain,
    RecreateSwapChain,
    CreateRenderPass,
    CreateSwapChain
  }; //SwapChainFunction

  enum GraphicsPipelineManagement {
    GeneratePipeline,
    ManageShader
  }; //GraphicsPipelineManagement

  enum PolygonVerticeManagement {
    CreateBuffer,
    CopyBuffer,
    CreatePolygon
  }; //PolygonVerticeManagement

  //VkInterface
  static VkInstance instance;
  static VkSurfaceKHR surface;
  static VkDevice device;
  static VkPhysicalDevice physDevice;
  static VkRenderPass renderPass;
  static VkSwapchainKHR swapChain;

  //Polygons
  static std::vector<Polyhedra*> polygons;

  //Buffers
  static std::vector<VkBuffer> indiceBuffer;
  static std::vector<VkBuffer> verticeBuffer;
  static std::vector<VkDeviceMemory> indiceBufferMemory;
  static std::vector<VkDeviceMemory> vertexBufferMemory;
  static std::vector<VkFramebuffer> frameBuffers;
  static std::vector<VkCommandBuffer> commandBuffers;

  //Sync Objects
  static std::vector<VkSemaphore> imageSemaphores;
  static std::vector<VkSemaphore> renderSemaphores;
  static std::vector<VkFence>  queueFences;

  //Other
  static std::optional<uint32_t> graphicsFamily;
  static std::optional<uint32_t> presentFamily;
  static VkQueue graphicsQueue;
  static VkQueue presentQueue;
  static VkCommandPool commandPool;
  static VkPipelineLayout pipelineLayout;
  static VkPipeline graphicsPipeline;
  static VkExtent2D swapChainExtent;
  static VkResult rslt;
  static VkPhysicalDeviceMemoryProperties memProperties;

  VkWindow(UIParams::MenuSelection colorGamut);

private:
  void VkDebugCheck(DebugFunction debugFunc, VkInstanceCreateInfo* instanceCreateInfo, VkDeviceCreateInfo* createInfo, VkDebugUtilsMessengerCreateInfoEXT* debugInfo);
  void VkSwapChainManagement(SwapChainFunction swapFunc);
  void VkManagePolygonBuffers(PolygonVerticeManagement func, Polyhedra* polygon, VkBuffer* verticeBuffer, VkBuffer* indiceBuffer, int index);
  void VkManageGraphicsPipeline(GraphicsPipelineManagement pipeline, std::string* shader, VkShaderModule* shaderModule);
  void VkManageMemoryProperties(
    VkMemoryRequirements* memRequirements, 
    VkMemoryPropertyFlags* allocProperties,
    VkMemoryAllocateInfo* allocInfo
  ); //VkManageMemoryProperties
  void VkTerminateWindow();
  void ErrorHandler(VkResult vulkan, std::string section);
  
}; //VkWindow