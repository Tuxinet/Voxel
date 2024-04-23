#pragma once

#include "lve_chunk.hpp"
#include "lve_camera.hpp"
#include "lve_device.hpp"
#include "lve_frame_info.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"

// std
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {
class ChunkRenderSystem {
public:
  ChunkRenderSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
  ~ChunkRenderSystem();

  ChunkRenderSystem(const ChunkRenderSystem &) = delete;
  ChunkRenderSystem &operator=(const ChunkRenderSystem &) = delete;

  void renderChunks(FrameInfo &frameInfo);

private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createPipeline(VkRenderPass renderPass);

  LveDevice &lveDevice;

  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace lve