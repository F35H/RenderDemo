#include <Vulkan/VulkanSingletons.h>


//VkWindowObjects
//Initialization
VkInstance VkWindow::instance;
VkSurfaceKHR VkWindow::surface;
VkDevice VkWindow::device;
VkPhysicalDevice VkWindow::physDevice;
VkRenderPass VkWindow::renderPass;
VkSwapchainKHR VkWindow::swapChain;

//Polygons
std::vector<Polyhedra> VkWindow::polygons;

//Pools
VkCommandPool VkWindow::commandPool;
VkDescriptorPool descriptorPool;

//Buffers
std::vector<VkBuffer> VkWindow::indiceBuffer;
std::vector<VkBuffer> VkWindow::verticeBuffer;
std::vector<VkDeviceMemory> VkWindow::indiceBufferMemory;
std::vector<VkDeviceMemory> VkWindow::vertexBufferMemory;
std::vector<VkFramebuffer> VkWindow::frameBuffers;
std::vector<VkCommandBuffer> VkWindow::commandBuffers;
std::vector<VkDescriptorSet> descriptorSets;

std::vector<VkBuffer> uniformBuffers;
std::vector<VkDeviceMemory> uniformBuffersMemory;
std::vector<void*> uniformBuffersMapped;


//Pipeline Objects
//static VkDescriptorSetLayout descriptorSetLayout;
//VkPipelineLayout VkWindow::pipelineLayout;
RenderIn::GFXPipeline VkWindow::graphicsPipeline;


//Sync Objects
std::vector<VkSemaphore> VkWindow::imageSemaphores;
std::vector<VkSemaphore> VkWindow::renderSemaphores;
std::vector<VkFence>  VkWindow::queueFences;

//Other
std::optional<uint32_t> VkWindow::graphicsFamily;
std::optional<uint32_t> VkWindow::presentFamily;
VkQueue VkWindow::graphicsQueue;
VkQueue VkWindow::presentQueue;
VkExtent2D VkWindow::swapChainExtent;
VkPhysicalDeviceMemoryProperties VkWindow::memProperties;
VkResult VkWindow::rslt;


//VkParams
int VkParams::resizeFrameBuffer = false;
int VkParams::currentPolytope = PolygonFactory::Triangle;

std::vector<VkClearValue> VkParams::clearColor = {
  {{0.0f, 1.0f, 0.0f, 1.0f}},   //Background
  {{1.0f, 0.0f}}                //DepthBuffer 
}; //clearColor

//Uniforms
static UniformFactory* uniFactory;

/* CALLBACK FUNCTIONS */
//static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
//  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
//
//  return VK_FALSE;
//} //Debug Callback
static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
  VkParams::resizeFrameBuffer = true;
} //Framebuffer Resize Callback;
static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_RELEASE) {
    switch (key) {
    case GLFW_KEY_LEFT:
    case GLFW_KEY_A:
      VkParams::currentPolytope += 1;
      break;
    case GLFW_KEY_RIGHT:
    case GLFW_KEY_D:
      VkParams::currentPolytope -= 1;
      break;
    case GLFW_KEY_R:
      uniFactory->RotateTrue = ~uniFactory->RotateTrue;
      break;
    }; //switch


    if (VkParams::currentPolytope < 0) { VkParams::currentPolytope = VkWindow::polygons.size() - 1; };
    if (VkParams::currentPolytope > VkWindow::polygons.size() - 1) { VkParams::currentPolytope = 0; };
  }; //if (action == GLFW_RELEASE)
}; //keyboardCallback

GLFWwindow* GLFW::window;
void GLFW::GenerateWindow(void) {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  window = glfwCreateWindow(VkParams::winWidth, VkParams::winHeight, VkParams::winName, nullptr, nullptr);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  glfwSetKeyCallback(window, keyboardCallback);

}; //GenerateWindow
void GLFW::TerminateWindow(void) {
  glfwDestroyWindow(window);
  glfwTerminate();
}; //TerminateWindow
void GLFW::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) { 
  switch (glfwCreateWindowSurface(instance, window, nullptr, surface)) {
  case VK_SUCCESS:
    return;
  case VK_ERROR_INITIALIZATION_FAILED:
    throw std::runtime_error("Initialization Failed, Surface Creation");
  case VK_ERROR_EXTENSION_NOT_PRESENT:
    throw std::runtime_error("Extension Not Present, Surface Creation");
  case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
    throw std::runtime_error("Surface Not Destroyed, Surface Creation");
  default:
    throw std::runtime_error("Unknown Error! Surface Creation");
  } //switch
}; //createWindowSurface;
std::pair<const char**, uint32_t> GLFW::getGLFWExtensions(VkInstanceCreateInfo* createInfo) {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  
  return std::pair<const char**, uint32_t>(glfwExtensions, glfwExtensionCount);
}; //setGLFWExtensions
bool GLFW::Ready() {
  glfwPollEvents();
  return !glfwWindowShouldClose(window);
}; //Ready

//VkWindow Functions
void VkWindow::ErrorHandler(VkResult result, std::string section) {
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

VkWindow::VkWindow() {
  const std::vector<const char*>deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  }; //DeviceExtensions

  GLFW::GenerateWindow();
  ////INITIALIZE
  //{
  //  VkInstanceCreateInfo createInfo{};
  //  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  //  createInfo.pApplicationInfo = &VkParams::appInfo;

  //  auto extensionPair = GLFW::getGLFWExtensions(&createInfo);
  //  std::vector<const char*> extensions(extensionPair.first, extensionPair.first + extensionPair.second);

  //  if (VkParams::DEBUG) {
  //    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  //  } //VkParams::DEBUG

  //  createInfo.enabledExtensionCount = extensions.size();
  //  createInfo.ppEnabledExtensionNames = extensions.data();

  //  createInfo.enabledLayerCount = 0;
  //  createInfo.pNext = nullptr;


  //  VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
  //  if (VkParams::DEBUG) {
  //    VkDebugCheck(ValidationLayerSupport, nullptr, nullptr, nullptr);
  //    VkDebugCheck(DebugMessengerInfo, &createInfo, nullptr, &debugInfo);
  //    VkDebugCheck(DebugMessengerCreation, nullptr, nullptr, nullptr);
  //  }; //VkPArams::Debug


  //  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) throw std::runtime_error("failed to create instance!");
  //} //Initialize

  ////CREATE SURFACE
  //{
  //  VkWin32SurfaceCreateInfoKHR createInfo{};
  //  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  //  createInfo.hwnd = glfwGetWin32Window(GLFW::window);
  //  createInfo.hinstance = GetModuleHandle(nullptr);

  //  if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) throw std::runtime_error("Surface Creation Failed!");

  //  GLFW::CreateWindowSurface(instance, &surface);
  //}

  ////PICK PHYSICAL DEVICE
  //{
  //  uint32_t deviceCount = 0;
  //  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  //  if (deviceCount == 0) throw std::runtime_error("failed to find GPUs with Vulkan support!");

  //  std::vector<VkPhysicalDevice> devices(deviceCount);
  //  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  //  for (const auto& device : devices) {
  //    uint32_t queueFamilyCount = 0;
  //    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  //    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  //    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

  //    int i = 0;
  //    for (const auto& queueFamily : queueFamilyProperties) {
  //      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
  //        graphicsFamily = i;
  //      }; //if GraphicsBitSet

  //      VkBool32 presentSupport = false;
  //      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

  //      if (presentSupport) {
  //        presentFamily = i;
  //      }; //if GraphicsBitSet

  //      if (graphicsFamily.has_value() && presentFamily.has_value()) break;

  //      ++i;
  //    } //for queueFamily in queueFamilyProperties


  //    uint32_t extensionCount;
  //    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  //    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  //    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

  //    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
  //    for (const auto& extension : availableExtensions) {
  //      requiredExtensions.erase(extension.extensionName);
  //    }; //for extension in Extension

  //    if (graphicsFamily.has_value() && presentFamily.has_value() && requiredExtensions.empty()) {
  //      physDevice = device;
  //      break;
  //    } //if device compatible
  //  } //for device in devices

  //  if (physDevice == VK_NULL_HANDLE) throw std::runtime_error("failed to find a suitable GPU!");
  //
  //  vkGetPhysicalDeviceMemoryProperties(physDevice, &memProperties);
  //} //Pick Physical Device

  ////CREATE LOGICAL DEVICE
  //{
  //  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  //  std::set<uint32_t> uniqueQueueFamilies = { graphicsFamily.value(), presentFamily.value() };

  //  float queuePriority = 1.0f;
  //  for (uint32_t queueFamily : uniqueQueueFamilies) {
  //    VkDeviceQueueCreateInfo queueCreateInfo{};
  //    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  //    queueCreateInfo.queueFamilyIndex = queueFamily;
  //    queueCreateInfo.queueCount = 1;
  //    queueCreateInfo.pQueuePriorities = &queuePriority;
  //    queueCreateInfos.push_back(queueCreateInfo);
  //  }

  //  VkPhysicalDeviceFeatures deviceFeatures{};

  //  VkDeviceCreateInfo createInfo{};
  //  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  //  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  //  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  //  createInfo.pEnabledFeatures = &deviceFeatures;
  //  createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  //  createInfo.ppEnabledExtensionNames = deviceExtensions.data();
  //  createInfo.enabledLayerCount = 0;

  //  if (VkParams::DEBUG) VkDebugCheck(LogicalDeviceLayers, nullptr, &createInfo, nullptr);

  //  rslt = vkCreateDevice(physDevice, &createInfo, nullptr, &device);
  //  if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Device Creation Failed");

  //  if (VkParams::DEBUG) {
  //    vkGetDeviceQueue(device, graphicsFamily.value(), 0, &graphicsQueue);
  //    vkGetDeviceQueue(device, presentFamily.value(), 0, &presentQueue);
  //  }; // if DEBUG
  //} //CreateLogicalDevice

  graphicsPipeline = RenderIn::GFXPipeline();

  errorHandler = std::make_unique<RenderOut::ErrorHandler>(RenderOut::ErrorHandler());
  externalProgram = new ExternalProgram(1);

  graphicsPipeline.externalProgram = externalProgram;
  physDevice = externalProgram->physicalDevice;
  surface = externalProgram->getCurrentWindow()->surface;
  graphicsFamily = externalProgram->graphicsFamily;
  presentFamily = externalProgram->presentFamily;
  device = externalProgram->device;
  memProperties = externalProgram->memProperties;

  //CREATE SWAPCHAIN
  VkSwapChainManagement(CreateSwapChain);

  //UNIFORM DATA
  uniFactory = new UniformFactory(VkWindow::swapChainExtent.width / (float)VkWindow::swapChainExtent.height);
  uniFactory->UpdateRenderProperties(UniformFactory::MaterialMisc, glm::vec4(4.f,0.0f,0.0f,0.0f));
  uniFactory->UpdateRenderProperties(UniformFactory::LightingMisc, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
  uniFactory->UpdateRenderProperties(UniformFactory::AmbientLighting, glm::vec4(0.1f,1.0f,1.0f,1.0f));

  //CREATE GRAPHICS PIPELINE
  //VkManageGraphicsPipeline(GeneratePipeline, nullptr, nullptr);
  graphicsPipeline.AddShaderFile(graphicsPipeline.Vertex, "shaders/SPIR-5/vert.spv");
  graphicsPipeline.AddShaderFile(graphicsPipeline.Fragment, "shaders/SPIR-5/frag.spv");
  graphicsPipeline.AddShaderBinding(graphicsPipeline.Vertex, offsetof(Vertex, pos));
  graphicsPipeline.AddShaderBinding(graphicsPipeline.Vertex, offsetof(Vertex, color));
  graphicsPipeline.AddShaderBinding(graphicsPipeline.Vertex, offsetof(Vertex, norm));
  graphicsPipeline.AddShaderBinding(graphicsPipeline.Fragment, sizeof(Vertex));
  graphicsPipeline.AddShaderBinding(graphicsPipeline.Fragment, sizeof(Vertex));
  graphicsPipeline.AddShaderBinding(graphicsPipeline.Fragment, sizeof(Vertex));
  graphicsPipeline.AddShaderBinding(graphicsPipeline.Fragment, sizeof(Vertex));
  graphicsPipeline.AddPushConst();
  graphicsPipeline.Activate(renderPass);

  graphicsQueue = externalProgram->gfxQueue;
  presentQueue = externalProgram->presentQueue;


  //CREATE UNIFORM BUFFERS
  {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(VkParams::framesPerLoop);
    uniformBuffersMemory.resize(VkParams::framesPerLoop);
    uniformBuffersMapped.resize(VkParams::framesPerLoop);

    for (size_t i = 0; i < VkParams::framesPerLoop; ++i) {
      VkBufferCreateInfo bufferInfo{};
      bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferInfo.size = bufferSize;
      bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
      bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      rslt = vkCreateBuffer(device, &bufferInfo, nullptr, &uniformBuffers[i]); 
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Uniform Buffer Failed!");

      VkMemoryRequirements memRequirements;
      vkGetBufferMemoryRequirements(device, uniformBuffers[i], &memRequirements);

      VkMemoryAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memRequirements.size;

      VkMemoryPropertyFlags allocProp = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      
      VkManageMemoryProperties(&memRequirements, &allocProp, &allocInfo);

      rslt = vkAllocateMemory(device, &allocInfo, nullptr, &uniformBuffersMemory[i]); 
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Failed to Allocate Uniform Buffer Memory");

      vkBindBufferMemory(device, uniformBuffers[i], uniformBuffersMemory[i], 0);
      vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    } //for framesPerLoop
  } //Create Uniform Buffers

  //CREATE DESCRIPTOR OBJECTS
  {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(VkParams::framesPerLoop);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(VkParams::framesPerLoop);

    rslt = vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Descriptor Pool");

    std::vector<VkDescriptorSetLayout> layouts(VkParams::framesPerLoop, graphicsPipeline.descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(VkParams::framesPerLoop);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(VkParams::framesPerLoop);
    rslt = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());
    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Failed to Allocate Descriptor Set");

    for (size_t i = 0; i < VkParams::framesPerLoop; i++) {
      VkDescriptorBufferInfo bufferInfo{};
      bufferInfo.buffer = uniformBuffers[i];
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(UniformBufferObject);
    
      VkWriteDescriptorSet descriptorWrite{};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descriptorSets[i];
      descriptorWrite.dstBinding = 0;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = &bufferInfo;
      descriptorWrite.pImageInfo = nullptr; // Optional
      descriptorWrite.pTexelBufferView = nullptr; // Optional
    
      vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    } // for size_t i =0;



  } //CREATE DESCRIPTOR OBJECTS

  //CREATE COMMAND BUFFERS
  {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsFamily.value();

    rslt = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Command Pool Allocation");

    commandBuffers.resize(VkParams::framesPerLoop);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    rslt = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Command Buffer Creation");
  } //CreateCommandBuffers

  //CREATE SYNC OBJECTS
  {
    imageSemaphores.resize(VkParams::framesPerLoop);
    renderSemaphores.resize(VkParams::framesPerLoop);
    queueFences.resize(VkParams::framesPerLoop);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < VkParams::framesPerLoop; ++i) {
      rslt = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageSemaphores[i]);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Semaphore Creation for Images");
      rslt = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderSemaphores[i]);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Semaphore Creation for Rendering");
      rslt = vkCreateFence(device, &fenceInfo, nullptr, &queueFences[i]);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Fence Creation for Queue");
    }; //for framesPerLoop
  }

  //CREATE POLYTOPES
  {
    auto polyFactory = new PolygonFactory();
    polygons = polyFactory->GetPolyhedra();
    //polyEdges = polyFactory->GetPolyhedraEdges();

    verticeBuffer.resize(polygons.size());
    indiceBuffer.resize(polygons.size());
    indiceBufferMemory.resize(polygons.size());
    vertexBufferMemory.resize(polygons.size());

    size_t i = 0;
    for (; i < polygons.size(); ++i) {
      VkManagePolygonBuffers(CreatePolygon, &polygons[i], &verticeBuffer[i], &indiceBuffer[i], i);
      //VkManagePolygonBuffers(CreatePolygon, polyEdges[i], &verticeBuffer[i], &indiceBuffer[i], i);
    } //for (auto& polygon : polygons)

    delete polyFactory;
  } //Create Polygon Vertices

  //MAINLOOP
  {
    int frame = 0;
    while (GLFW::Ready()) {

      memcpy(uniformBuffersMapped[frame], uniFactory->GetUniformBuffer(), sizeof(UniformBufferObject));

      vkWaitForFences(device, 1, &queueFences[frame], VK_TRUE, UINT64_MAX);

      uint32_t imageIndex;
      rslt = vkAcquireNextImageKHR(
        device, swapChain, UINT64_MAX, imageSemaphores[frame], VK_NULL_HANDLE, &imageIndex);

      if (rslt == VK_ERROR_OUT_OF_DATE_KHR) {
        VkSwapChainManagement(RecreateSwapChain);
        return;
      }
      else if (rslt != VK_SUCCESS && rslt != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
      } // if rslt

      vkResetFences(device, 1, &queueFences[frame]);
      vkResetCommandBuffer(commandBuffers[frame], /*VkCommandBufferResetFlagBits*/ 0);

      //Input Commands
      {

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[frame], &beginInfo) != VK_SUCCESS) {
          throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = frameBuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainExtent;

        renderPassInfo.clearValueCount = static_cast<uint32_t>(VkParams::clearColor.size());
        renderPassInfo.pClearValues = VkParams::clearColor.data();

        vkCmdBeginRenderPass(commandBuffers[frame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[frame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.graphicsPipeline);


        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapChainExtent.width;
        viewport.height = (float)swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffers[frame], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffers[frame], 0, 1, &scissor);

        uniFactory->UpdatePolledInformation();

        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffers[frame], 0, 1, &verticeBuffer[VkParams::currentPolytope], offsets);

        vkCmdBindIndexBuffer(commandBuffers[frame], indiceBuffer[VkParams::currentPolytope], 0, VK_INDEX_TYPE_UINT16);

        vkCmdPushConstants(
          commandBuffers[frame],
          graphicsPipeline.pipelineLayout,
          VK_SHADER_STAGE_FRAGMENT_BIT,
          0, sizeof(PushConst), uniFactory->GetPushConst()
        ); //VkCmdPushCOnst

        vkCmdBindDescriptorSets(commandBuffers[frame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.pipelineLayout, 0, 1, &descriptorSets[frame], 0, nullptr);
        vkCmdDrawIndexed(commandBuffers[frame], static_cast<uint32_t>(polygons[VkParams::currentPolytope].GetIndiceSize()), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffers[frame]);        

        if (vkEndCommandBuffer(commandBuffers[frame]) != VK_SUCCESS) {
          throw std::runtime_error("failed to record command buffer!");
        } //if vkEndCommandBuffer
      }


      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

      VkSemaphore waitSemaphores[] = { imageSemaphores[frame] };
      VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = waitSemaphores;
      submitInfo.pWaitDstStageMask = waitStages;

      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &commandBuffers[frame];

      VkSemaphore signalSemaphores[] = { renderSemaphores[frame] };
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = signalSemaphores;

      if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, queueFences[frame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
      }

      VkPresentInfoKHR presentInfo{};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = signalSemaphores;

      VkSwapchainKHR swapChains[] = { swapChain };
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = swapChains;

      presentInfo.pImageIndices = &imageIndex;

      rslt = vkQueuePresentKHR(presentQueue, &presentInfo);

      if (rslt == VK_ERROR_OUT_OF_DATE_KHR || rslt == VK_SUBOPTIMAL_KHR || VkParams::resizeFrameBuffer) {
        VkParams::resizeFrameBuffer = false;
        VkSwapChainManagement(RecreateSwapChain);
      }
      else if (rslt != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
      } //else if

      frame = (frame + 1) % VkParams::framesPerLoop;
    } //while (GLFW::Ready)

    vkDeviceWaitIdle(device);
  } //Mainloop

  VkTerminateWindow();
}; //VkWindow
void VkWindow::VkDebugCheck(DebugFunction debugFunc, VkInstanceCreateInfo* instanceCreateInfo, VkDeviceCreateInfo* createInfo, VkDebugUtilsMessengerCreateInfoEXT* debugInfo) {
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
  }; //switch(debugFunc)
}; //VkDebugCheck

void VkWindow::VkManagePolygonBuffers(PolygonVerticeManagement func, Polyhedra* polygon, VkBuffer* vertexBuffer, VkBuffer* indexBuffer, int index) {
  VkDeviceSize buffSize;
  VkBuffer stageBuff{};
  VkDeviceMemory stagingBufferMemory{};

  enum PolygonCreationStage {
    CreateVerticeStageBuffer,
    CreateVerticeDataBuffer,
    CopyVerticeData,
    CreateIndiceStageBuffer,
    CreateIndiceDataBuffer,
    CopyIndiceData
  }; //PolygonCreationStage


  switch (func) {
    int PolyCreationStage;
  case CreatePolygon: {
    buffSize = polygon->GetBufferSize();

    //VERTICES
    PolyCreationStage = CreateVerticeStageBuffer;
    goto case_CreateBuffer;
  verticeStageBufferCreationFinished:
    PolyCreationStage = CreateVerticeDataBuffer;

    goto case_CreateBuffer;
  verticeDataBufferCreationFinished:
    PolyCreationStage = CopyVerticeData;

    goto case_CopyBuffer;
  verticeDataBufferCopyFinished:
    PolyCreationStage = CreateIndiceStageBuffer;

    //INDICES
    goto case_CreateBuffer;
  indiceStageBufferCreationFinished:
    PolyCreationStage = CreateIndiceDataBuffer;

    goto case_CreateBuffer;
  indiceDataBufferCreationFinished:
    PolyCreationStage = CopyIndiceData;

    goto case_CopyBuffer;
  indiceDataBufferCopyFinished:
    return;
  } //CreatePolygon
  case_CreateBuffer:
  case CreateBuffer: {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.size = buffSize;

    VkMemoryRequirements memRequirements{};
    VkMemoryPropertyFlags allocProperties;

    if ((PolyCreationStage == CreateVerticeStageBuffer) || (PolyCreationStage == CreateIndiceStageBuffer)) {
      bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      rslt = vkCreateBuffer(device, &bufferInfo, nullptr, &stageBuff);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Stage Buffer Creation");
      vkGetBufferMemoryRequirements(device, stageBuff, &memRequirements);
      allocProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }; // if creationStage

    if (PolyCreationStage == CreateVerticeDataBuffer) {
      bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
      rslt = vkCreateBuffer(device, &bufferInfo, nullptr, vertexBuffer);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Vertice Buffer Creation");
      vkGetBufferMemoryRequirements(device, *vertexBuffer, &memRequirements);
      allocProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }; //if CreateVerticeDataBuffer

    if (PolyCreationStage == CreateIndiceDataBuffer) {
      bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
      rslt = vkCreateBuffer(device, &bufferInfo, nullptr, indexBuffer);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Indice Buffer Creation");
      vkGetBufferMemoryRequirements(device, *indexBuffer, &memRequirements);
      allocProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }; //if CreationStage == IndiceData

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    VkManageMemoryProperties(&memRequirements, &allocProperties, &allocInfo);

    if (PolyCreationStage == CreateVerticeDataBuffer) {
      rslt = vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory[index]);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Vertex Buffer Memory Failed to Allocate");
      vkBindBufferMemory(device, *vertexBuffer, vertexBufferMemory[index], 0);
    }; // PolyCreationStage

    if (PolyCreationStage == CreateIndiceDataBuffer) {
      rslt = vkAllocateMemory(device, &allocInfo, nullptr, &indiceBufferMemory[index]);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Index Buffer Memory Failed to Allocate");
      vkBindBufferMemory(device, *indexBuffer, indiceBufferMemory[index], 0);
    }; // PolyCreationStage

    if (PolyCreationStage == CreateVerticeStageBuffer) {
      rslt = vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Stage Buffer Memory Failed to Allocate");

      void* interData;
      vkBindBufferMemory(device, stageBuff, stagingBufferMemory, 0);
      vkMapMemory(device, stagingBufferMemory, 0, buffSize, 0, &interData);
      memcpy(interData, polygon->vertices.data(), (size_t)buffSize);
      vkUnmapMemory(device, stagingBufferMemory);
    }; // PolyCreationStage

    if (PolyCreationStage == CreateIndiceStageBuffer) {
      rslt = vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Stage Buffer Memory Failed to Allocate");

      void* interData;
      vkBindBufferMemory(device, stageBuff, stagingBufferMemory, 0);
      vkMapMemory(device, stagingBufferMemory, 0, buffSize, 0, &interData);
      memcpy(interData, polygon->indices.data(), (size_t)buffSize);
      vkUnmapMemory(device, stagingBufferMemory);
    }; // PolyCreationStage

    if (PolyCreationStage == CreateVerticeStageBuffer) { PolyCreationStage = CreateVerticeDataBuffer; goto case_CreateBuffer; };
    if (PolyCreationStage == CreateVerticeDataBuffer)  { PolyCreationStage = CopyVerticeData;         goto case_CopyBuffer;   };
    if (PolyCreationStage == CreateIndiceStageBuffer)  { PolyCreationStage = CreateIndiceDataBuffer;  goto case_CreateBuffer; };
    if (PolyCreationStage == CreateIndiceDataBuffer)   { PolyCreationStage = CopyIndiceData;          goto case_CopyBuffer;   };
  } //CreatePolygon
  return;
  case_CopyBuffer:
  case CopyBuffer: {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = buffSize;

    if (PolyCreationStage == CopyVerticeData) vkCmdCopyBuffer(
      commandBuffer, stageBuff, *vertexBuffer, 1, &copyRegion);

    if (PolyCreationStage == CopyIndiceData) vkCmdCopyBuffer(
      commandBuffer, stageBuff, *indexBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    vkDestroyBuffer(device, stageBuff, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    if (PolyCreationStage == CopyVerticeData) { PolyCreationStage = CreateIndiceStageBuffer;  goto case_CreateBuffer; };
    if (PolyCreationStage == CopyIndiceData)  { return; };
  } //CreateBuffer
  return;
  }; //switch 
}; //VkManagePolygonBuffer
//void VkWindow::VkManageGraphicsPipeline(GraphicsPipelineManagement pipeline, std::string* shader, VkShaderModule* shaderModule) {
//  switch (pipeline) {
//  case GeneratePipeline: {
//    VkShaderModule vertShaderModule;
//    VkShaderModule fragShaderModule;
//
//    std::string vertShader = "shaders/SPIR-5/vert.spv";
//    std::string fragShader = "shaders/SPIR-5/frag.spv";
//
//    VkManageGraphicsPipeline(ManageShader, &vertShader, &vertShaderModule);
//    VkManageGraphicsPipeline(ManageShader, &fragShader, &fragShaderModule);
//
//    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
//    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//    vertShaderStageInfo.module = vertShaderModule;
//    vertShaderStageInfo.pName = "main";
//
//    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
//    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//    fragShaderStageInfo.module = fragShaderModule;
//    fragShaderStageInfo.pName = "main";
//
//    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
//
//    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//
//
//    //PUSH CONSTANTS
//    VkPushConstantRange push_constant;
//    push_constant.offset = 0;
//    push_constant.size = sizeof(PushConst);
//    push_constant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//
//
//    //FRAG ATTRIBS
//    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
//    bindingDescriptions.resize(4);
//
//    bindingDescriptions[0].binding = 0;
//    bindingDescriptions[0].stride = sizeof(Vertex);
//    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//    bindingDescriptions[1].binding = 0;
//    bindingDescriptions[1].stride = sizeof(Vertex);
//    bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//    bindingDescriptions[2].binding = 0;
//    bindingDescriptions[2].stride = sizeof(Vertex);
//    bindingDescriptions[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//    bindingDescriptions[3].binding = 0;
//    bindingDescriptions[3].stride = sizeof(Vertex);
//    bindingDescriptions[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//
//    //VERTEX ATTRIBS
//    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
//    attributeDescriptions.resize(3);
//
//    attributeDescriptions[0].binding = 0;
//    attributeDescriptions[0].location = 0;
//    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//    attributeDescriptions[0].offset = offsetof(Vertex, pos);
//
//    attributeDescriptions[1].binding = 0;
//    attributeDescriptions[1].location = 1;
//    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
//    attributeDescriptions[1].offset = offsetof(Vertex, color);
//
//    attributeDescriptions[2].binding = 0;
//    attributeDescriptions[2].location = 2;
//    attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
//    attributeDescriptions[2].offset = offsetof(Vertex, norm);
//
//
//    vertexInputInfo.vertexBindingDescriptionCount = 1;
//    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
//    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
//
//
//    //DESCRIPTOR SETS
//    VkDescriptorSetLayoutBinding uboLayoutBinding{};
//    uboLayoutBinding.binding = 0;
//    uboLayoutBinding.descriptorCount = 1;
//    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//
//    VkDescriptorSetLayoutCreateInfo layoutInfo{};
//    layoutInfo.bindingCount = 1;
//    layoutInfo.pBindings = &uboLayoutBinding;
//    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//
//    rslt = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
//    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Descriptor Set Layout Failed");
//
//
//    //GENERAL PIPELINE INFORMATION
//    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//    inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//    VkPipelineViewportStateCreateInfo viewportState{};
//    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//    viewportState.viewportCount = 1;
//    viewportState.scissorCount = 1;
//
//    VkPipelineRasterizationStateCreateInfo rasterizer{};
//    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//    rasterizer.depthClampEnable = VK_FALSE;
//    rasterizer.rasterizerDiscardEnable = VK_FALSE;
//    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//    rasterizer.lineWidth = 1.0f;
//    rasterizer.cullMode = VK_CULL_MODE_NONE;
//    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
//    rasterizer.depthBiasEnable = VK_FALSE;
//
//    VkPipelineMultisampleStateCreateInfo multisampling{};
//    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//    multisampling.sampleShadingEnable = VK_FALSE;
//    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//
//    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//    colorBlendAttachment.blendEnable = VK_FALSE;
//
//    VkPipelineColorBlendStateCreateInfo colorBlending{};
//    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//    colorBlending.logicOpEnable = VK_FALSE;
//    colorBlending.logicOp = VK_LOGIC_OP_COPY;
//    colorBlending.attachmentCount = 1;
//    colorBlending.pAttachments = &colorBlendAttachment;
//    colorBlending.blendConstants[0] = 0.0f;
//    colorBlending.blendConstants[1] = 0.0f;
//    colorBlending.blendConstants[2] = 0.0f;
//    colorBlending.blendConstants[3] = 0.0f;
//
//    VkPipelineDepthStencilStateCreateInfo depthStencil{};
//    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//    depthStencil.depthTestEnable = VK_TRUE;
//    depthStencil.depthWriteEnable = VK_TRUE;
//    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
//    depthStencil.depthBoundsTestEnable = VK_FALSE;
//    depthStencil.minDepthBounds = 0.0f; // Optional
//    depthStencil.maxDepthBounds = 1.0f; // Optional
//    depthStencil.stencilTestEnable = VK_FALSE;
//    depthStencil.front = {}; // Optional
//    depthStencil.back = {}; // Optional
//
//    std::vector<VkDynamicState> dynamicStates = {
//        VK_DYNAMIC_STATE_VIEWPORT,
//        VK_DYNAMIC_STATE_SCISSOR
//    }; //Dynamic States
//
//    VkPipelineDynamicStateCreateInfo dynamicState{};
//    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
//    dynamicState.pDynamicStates = dynamicStates.data();
//
//    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//    pipelineLayoutInfo.pPushConstantRanges = &push_constant;
//    pipelineLayoutInfo.pushConstantRangeCount = 1;
//    pipelineLayoutInfo.setLayoutCount = 1;
//    pipelineLayoutInfo.pSetLayouts = { &descriptorSetLayout };
//
//    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout!");
//
//    VkGraphicsPipelineCreateInfo pipelineInfo{};
//    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//    pipelineInfo.stageCount = 2;
//    pipelineInfo.pStages = shaderStages;
//    pipelineInfo.pVertexInputState = &vertexInputInfo;
//    pipelineInfo.pInputAssemblyState = &inputAssembly;
//    pipelineInfo.pViewportState = &viewportState;
//    pipelineInfo.pRasterizationState = &rasterizer;
//    pipelineInfo.pMultisampleState = &multisampling;
//    pipelineInfo.pColorBlendState = &colorBlending;
//    pipelineInfo.pDynamicState = &dynamicState;
//    pipelineInfo.layout = pipelineLayout;
//    pipelineInfo.renderPass = renderPass;
//    pipelineInfo.subpass = 0;
//    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//    pipelineInfo.pDepthStencilState = &depthStencil;
//
//    rslt = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline.pipelineInfo); 
//    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Graphics Pipeline");
//
//    vkDestroyShaderModule(device, fragShaderModule, nullptr);
//    vkDestroyShaderModule(device, vertShaderModule, nullptr);
//
//    return;
//  }
//  case ManageShader: {
//    std::ifstream file(*shader, std::ios::ate | std::ios::binary);
//
//    if (!file.is_open()) throw std::runtime_error("failed to open file!");
//
//    size_t fileSize = (size_t)file.tellg();
//    std::vector<char> buffer(fileSize);
//
//    file.seekg(0);
//    file.read(buffer.data(), fileSize);
//
//    file.close();
//
//    VkShaderModuleCreateInfo createInfo{};
//    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//    createInfo.codeSize = buffer.size();
//    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
//
//    rslt = vkCreateShaderModule(device, &createInfo, nullptr, shaderModule);
//    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad Shader Loading");
//    return;
//  }
//  }; //Switch Graphics Pipeline Management
//}; //GraphicsPipelineMangement;
void VkWindow::VkSwapChainManagement(SwapChainFunction swapFunc) {
  static std::vector<VkImage> swapChainImages;
  static std::vector<VkImageView> swapChainImageViews;
  static VkFormat swapChainImageFormat;

  static VkSurfaceCapabilitiesKHR swapChainAbilities;
  static std::vector<VkSurfaceFormatKHR> swapChainFormats;
  static std::vector<VkPresentModeKHR> swapChainModes;

  static VkSurfaceFormatKHR surfaceFormat;
  static VkPresentModeKHR presentMode;

  static VkImage depthImage;
  static VkDeviceMemory depthImageMemory;
  static VkImageView depthImageView;
  static VkFormat depthFormat;

  switch (swapFunc) {
  case_CleanupSwapChain:
  case CleanUpSwapChain: {
    for (auto framebuffer : frameBuffers) {
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    } //for buffers in framebuffers

    for (auto imageView : swapChainImageViews) {
      vkDestroyImageView(device, imageView, nullptr);
    } //for images in imageviews

    vkDestroySwapchainKHR(device, swapChain, nullptr);
    if (swapFunc != RecreateSwapChain) return;

    vkDestroyImage(device, depthImage, nullptr);
    vkDestroyImageView(device, depthImageView, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);
    if (swapFunc != CleanUpSwapChain) goto case_CreateSwapChain;
    return;
  } //case CleanupswapChain

  case RecreateSwapChain: {
    int width = 0, height = 0;
    glfwGetFramebufferSize(GLFW::window, &width, &height);
    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(GLFW::window, &width, &height);
      glfwWaitEvents();
    } //While window minimized

    vkDeviceWaitIdle(device);
    goto case_CleanupSwapChain;
  }; //RecreateSwapChain

  case_CreateSwapChain:
  case CreateSwapChain: {
    //CREATE SWAP CHAIN

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &swapChainAbilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
      swapChainFormats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, swapChainFormats.data());
    } //if (formatCount != 0)
    else {
      throw std::runtime_error("formatCount Failed");
    }; //formatCount != 0

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
      swapChainModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, swapChainModes.data());
    } //if presentModeCount != 0
    else {
      throw std::runtime_error("presentModeCountFailed Failed");
    }; //presentModeCount

    surfaceFormat = swapChainFormats[0];
    for (auto& swapChainFormat : swapChainFormats) {
      if (swapChainFormat.format == VK_FORMAT_B8G8R8A8_SRGB && swapChainFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        surfaceFormat = swapChainFormat;
        break;
      } //for 
    } //for formats


    presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode : swapChainModes) {
      if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
        presentMode = mode;
        break;
      } //if mode == VK_PRESENT_MODE
    } //for (const auto mode)


    if (swapChainAbilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) swapChainExtent = swapChainAbilities.currentExtent;
    else {
      int width, height;
      glfwGetFramebufferSize(GLFW::window, &width, &height);

      VkExtent2D extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
      }; //VkExtent2D

      swapChainExtent.width = std::clamp(extent.width, swapChainAbilities.minImageExtent.width, swapChainAbilities.maxImageExtent.width);
      swapChainExtent.height = std::clamp(extent.height, swapChainAbilities.minImageExtent.height, swapChainAbilities.maxImageExtent.height);
    }; //else

    uint32_t imageCount = swapChainAbilities.minImageCount + 1;
    if (swapChainAbilities.maxImageCount > 0 && imageCount > swapChainAbilities.maxImageCount) {
      imageCount = swapChainAbilities.maxImageCount;
    } //Swap Chain Abilities

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = swapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { graphicsFamily.value(), presentFamily.value() };

    if (graphicsFamily != presentFamily) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0;
      createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainAbilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    rslt = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad SwapChain");

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
  }
  //CREATE DEPTH BUFFER 
  {
    for (VkFormat format : {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}) {
      VkFormatProperties props;
      vkGetPhysicalDeviceFormatProperties(physDevice, format, &props);

      if (VK_IMAGE_TILING_OPTIMAL == VK_IMAGE_TILING_LINEAR
        && (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        depthFormat = format;
        break;
      }
      else if (VK_IMAGE_TILING_OPTIMAL == VK_IMAGE_TILING_OPTIMAL
        && (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        depthFormat = format;
        break;
      } //if correct format;
    } //for VkFormat

    if (!depthFormat) throw std::runtime_error("failed to find supported format!");

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = swapChainExtent.width;
    imageInfo.extent.height = swapChainExtent.width;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    auto rslt = vkCreateImage(device, &imageInfo, nullptr, &depthImage);
    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Depth Image Creation");

    VkMemoryRequirements memRequirements{};
    vkGetImageMemoryRequirements(device, depthImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    VkMemoryPropertyFlags allocProp = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkManageMemoryProperties(&memRequirements, &allocProp, &allocInfo);

    rslt = vkAllocateMemory(device, &allocInfo, nullptr, &depthImageMemory);
    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Depth Image Memory Allocation");

    vkBindImageMemory(device, depthImage, depthImageMemory, 0);

  }
  // CREATE IMAGE VIEWS
  {
    swapChainImageViews.resize(swapChainImages.size());

    //Plus One added for Depth Buffer
    for (size_t i = 0; i < (swapChainImages.size() + 1); ++i) {
      
      VkImageViewCreateInfo createInfo{};
      if (i < swapChainImages.size()) {
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        rslt = vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]);
        if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad SwapChain ImageLine");
      } //if swapChain Image
      else {
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = depthImage;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = depthFormat;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        rslt = vkCreateImageView(device, &createInfo, nullptr, &depthImageView);
        if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad SwapChain ImageLine");
      } //if depth Buffer Image
    }; //for images in swapchain
  }
  case CreateRenderPass: {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.srcAccessMask = 0;

    std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;


    rslt = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
    if (rslt != VK_SUCCESS) ErrorHandler(rslt, "Bad RenderPass");
  }
  // CREATE FRAME BUFFERS
  {
    frameBuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
      std::vector<VkImageView> attachments = {
        swapChainImageViews[i],
        depthImageView
      }; //Attachments

      VkFramebufferCreateInfo framebufferInfo{};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = renderPass;
      framebufferInfo.attachmentCount = attachments.size();
      framebufferInfo.pAttachments = attachments.data();
      framebufferInfo.width = swapChainExtent.width;
      framebufferInfo.height = swapChainExtent.height;
      framebufferInfo.layers = 1;

      auto rslt = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[i]);
      if (rslt != VK_SUCCESS) ErrorHandler(rslt, "framebuffer");
    } //for (size_t i)
  } //Create Frame Buffers
  };// switch Swapchain
  return;
}; //VkSwapChainCreation
void VkWindow::VkManageMemoryProperties(
  VkMemoryRequirements* memRequirements,
  VkMemoryPropertyFlags* allocProperties,
  VkMemoryAllocateInfo* allocInfo
) {
  uint32_t i = 0;
  for (; i <= memProperties.memoryTypeCount; ++i) {
    if ((memRequirements->memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & *allocProperties) == *allocProperties) {
      allocInfo->memoryTypeIndex = i;
      break;
    } //if (typeFilter)

    if (i == memProperties.memoryTypeCount) {
      throw std::runtime_error("failed to find suitable memory type!");
    }; //if i == memProperties
  } //for uint32_t
}
void VkWindow::VkTerminateWindow() {
  GLFW::TerminateWindow();
  VkSwapChainManagement(CleanUpSwapChain);


  vkDestroyPipeline(device, graphicsPipeline.graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(device, graphicsPipeline.pipelineLayout, nullptr);
  vkDestroyRenderPass(device, renderPass, nullptr);

  size_t i = 0;
  for (auto polygon : polygons) {
    vkDestroyBuffer(device, indiceBuffer[i], nullptr);
    vkFreeMemory(device, indiceBufferMemory[i], nullptr);

    vkDestroyBuffer(device, verticeBuffer[i], nullptr);
    vkFreeMemory(device, vertexBufferMemory[i], nullptr);
    ++i;
  }; //polygons

  for (i = 0; i < VkParams::framesPerLoop; ++i) {
    vkDestroyBuffer(device, uniformBuffers[i], nullptr);
    vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    vkDestroySemaphore(device, imageSemaphores[i], nullptr);
    vkDestroySemaphore(device, renderSemaphores[i], nullptr);
    vkDestroyFence(device, queueFences[i], nullptr);
  }; // for i = 0; i < VkParams::framesPerLoop

  vkDestroyDescriptorPool(device, descriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(device, graphicsPipeline.descriptorSetLayout, nullptr);
  //vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

  vkDestroyCommandPool(device, commandPool, nullptr);

  vkDestroyDevice(device, nullptr);

  //if (enableValidationLayers) {
  //  DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  //}

  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
}; //VkTerminateWindow
