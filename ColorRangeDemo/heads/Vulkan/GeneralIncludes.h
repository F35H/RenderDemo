#pragma once
#pragma inline_recursion(on)

#include <sole.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <stb_image.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>

#include <GLFW/glfw3native.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <random>
#include <map>
#include <optional>
#include <queue>
#include <deque>
#include <fstream>
#include <chrono>
#include <vector>
#include <set>