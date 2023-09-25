#pragma once

#include "Vulkan/Objects/Buffers/GPUBuffer.h"

#include <utility>
#include <string>
#include <vector>

struct GeneralBuffer {

}; //Buffer

struct TextureBuffer {

}; //TextureBuffer

struct IndiceBuffer {

}; //IndiceBuffer

struct VerticeBuffer {

}; //VerticeBuffer

struct UniformBuffer {

}; //Uniform

struct FrameBuffer {
  VkFramebuffer framebuffer;
}; //FrameBuffer

struct ImageBuffer : VkImageViewCreateInfo {
  VkImage imageBuffer;
  VkImageView imageView;
  VkDevice device;

  VkResult CreateSwapImage() {
    sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image = imageBuffer;
    viewType = VK_IMAGE_VIEW_TYPE_2D;
    format = VK_FORMAT_B8G8R8A8_SRGB;
    components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    return vkCreateImageView(device, this, nullptr, &imageView);
  }; //CreateSwapImage

  VkResult CreateDepthImage() {
    sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image = imageBuffer;
    viewType = VK_IMAGE_VIEW_TYPE_2D;
    format = VK_FORMAT_B8G8R8A8_SRGB;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    return vkCreateImageView(device, this, nullptr, &imageView);
  }; //CreateDepthImage

  ImageBuffer(VkDevice d) : device(d) {};

  ~ImageBuffer() {
    vkDestroyImage(device, imageBuffer, nullptr);
    vkDestroyImageView(device, imageView, nullptr);
  }; //ImageBuffer
}; //ImageBuffer

struct BufferFactory {
  std::vector<FrameBuffer> framebuffers;
  std::vector<ImageBuffer> imageBuffers;

  VkImage createImageBuffer(VkDevice d) 
  { imageBuffers.emplace_back(new ImageBuffer(d)); }
  
  VkFramebuffer getFrameBuffer(int i) {
    return framebuffers[i].framebuffer;
  }; //getFrameBuffer

  VkImage getImage(int i) {
    return imageBuffers[i].image;
  }; //getImage

  VkImage getImage(int i) {
    return imageBuffers[i].image;
  }; //getImage
}; //BufferCollection