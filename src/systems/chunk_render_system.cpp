#include "chunk_render_system.hpp"
#include <cstddef>
#include <vulkan/vulkan_core.h>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace lve {

struct SimplePushConstantData {
  glm::mat4 modelMatrix{1.f};
  glm::mat4 normalMatrix{1.f};
};

ChunkRenderSystem::ChunkRenderSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
    : lveDevice{device}, pool(3) {
  createPipelineLayout(globalSetLayout);
  createPipeline(renderPass);
}

ChunkRenderSystem::~ChunkRenderSystem() { vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr); }

void ChunkRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void ChunkRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;

  lvePipeline = std::make_unique<LvePipeline>(lveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv",
                                              pipelineConfig);
}

void ChunkRenderSystem::renderChunks(FrameInfo &frameInfo) {
  lvePipeline->bind(frameInfo.commandBuffer);

  vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &frameInfo.globaleDescriptorSet, 0, nullptr);

  for (auto chunk : frameInfo.chunks) {

    SimplePushConstantData push{};
    push.modelMatrix = chunk->transform.mat4();
    push.normalMatrix = chunk->transform.normalMatrix();

    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(SimplePushConstantData), &push);

    // This should probably NOT be done here, but oh well...
    // TODO: Move mesh-generation to somewhere else (tm)
    if (chunk->hasStartedThread) continue;

    if (chunk->dirtyMesh) {
      chunk->hasStartedThread = true;
      pool.enqueue([chunk]{
        chunk->generateMesh();
      });

      continue;
    }

    chunk->model->bind(frameInfo.commandBuffer);
    chunk->model->draw(frameInfo.commandBuffer);
  }
}
} // namespace lve