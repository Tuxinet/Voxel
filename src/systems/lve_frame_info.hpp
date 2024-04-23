#pragma once

#include "lve_camera.hpp"
#include "lve_game_object.hpp"
#include "lve_chunk.hpp"
// lib
#include <vulkan/vulkan.h>

namespace lve {

#define MAX_LIGHTS 10

struct PointLight {
  glm::vec4 position{}; // ignore w, vec4 because of memory alignment
  glm::vec4 color{};    // w is intensity
};

struct GlobalUbo {
  glm::mat4 projection{1.f};
  glm::mat4 view{1.f};
  glm::mat4 inverseView{1.f};
  glm::vec4 ambientColor{1.f, 1.f, 1.f, .05f}; // w is intensity
  PointLight pointLights[MAX_LIGHTS];
  int numLights;
};

struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  LveCamera &camera;
  VkDescriptorSet globaleDescriptorSet;
  LveGameObject::Map &gameObjects;
  LveChunk::Map &chunks;
};
} // namespace lve