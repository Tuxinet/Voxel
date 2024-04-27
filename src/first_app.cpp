#include "first_app.hpp"
#include "systems/chunk_render_system.hpp"
#include "systems/keyboard_movement_controller.hpp"
#include "systems/lve_buffer.hpp"
#include "systems/lve_camera.hpp"
#include "systems/lve_descriptors.hpp"
#include "systems/lve_game_object.hpp"
#include "systems/lve_model.hpp"
#include "systems/lve_swap_chain.hpp"
#include "systems/point_light_system.hpp"
#include "systems/simple_render_system.hpp"
#include <glm/geometric.hpp>
#include <memory>
#include <mutex>
#include <vulkan/vulkan_core.h>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <iostream>
#include <numeric>
#include <stdexcept>

namespace lve {

FirstApp::FirstApp() {
  globalPool = LveDescriptorPool::Builder(lveDevice)
                   .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
                   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
                   .build();
  loadChunks();
}

FirstApp::~FirstApp() {}
void FirstApp::run() {

  std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<LveBuffer>(lveDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    uboBuffers[i]->map();
  }

  auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
                             .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                             .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    LveDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
  }

  SimpleRenderSystem simpleRenderSystem(lveDevice, lveRenderer.getSwapChainRenderPass(),
                                        globalSetLayout->getDescriptorSetLayout());

  ChunkRenderSystem chunkRenderSystem(lveDevice, lveRenderer.getSwapChainRenderPass(),
                                      globalSetLayout->getDescriptorSetLayout());

  PointLightSystem pointLightSystem(lveDevice, lveRenderer.getSwapChainRenderPass(),
                                    globalSetLayout->getDescriptorSetLayout());

  LveCamera camera{};
  camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

  auto viewerObject = LveGameObject::createGameObject();
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!lveWindow.shouldClose()) {
    glfwPollEvents();
    std::lock_guard<std::mutex> guard(LveModel::m_createBuffersMutex);

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    float aspect = lveRenderer.getAspectRatio();
    // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10000.f);

    if (auto commandBuffer = lveRenderer.beginFrame()) {
      int frameIndex = lveRenderer.getFrameIndex();
      FrameInfo frameInfo{frameIndex,
                          frameTime,
                          commandBuffer,
                          camera,
                          globalDescriptorSets[frameIndex],
                          gameObjects,
                          world.getChunksAroundPosition(LveChunk::getChunkPositionX(camera.getPosition().x),
                                                        LveChunk::getChunkPositionZ(camera.getPosition().z), 20)};

      // update
      GlobalUbo ubo{};
      ubo.projection = camera.getProjection();
      ubo.view = camera.getView();
      ubo.inverseView = camera.getInverseView();
      pointLightSystem.update(frameInfo, ubo);
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      // render
      lveRenderer.beginSwapChainRenderPass(commandBuffer);

      // order here matters

      simpleRenderSystem.renderGameObjects(frameInfo);

      chunkRenderSystem.renderChunks(frameInfo);

      pointLightSystem.render(frameInfo);

      lveRenderer.endSwapChainRenderPass(commandBuffer);
      lveRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadChunks() {
  std::shared_ptr<LveModel> cubeModel = LveModel::createModelFromFile(lveDevice, "models/cube.obj");
  std::shared_ptr<LveModel> cubeColorModel = LveModel::createModelFromFile(lveDevice, "models/colored_cube.obj");
  std::shared_ptr<LveModel> floorModel = LveModel::createModelFromFile(lveDevice, "models/quad.obj");
  std::shared_ptr<LveModel> vaseModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");

  auto floor = LveGameObject::createGameObject();
  floor.model = nullptr;
  floor.transform.translation = {0.5f, 2.2f, 0.5f};
  floor.transform.scale = {3.f, 1.5f, 3.5f};

  std::cout << "Generating world..." << std::endl;
  auto chunkStartTime = std::chrono::high_resolution_clock::now();

  auto chunks = world.getChunksAroundPosition(0, 0, 1);

  std::cout << "Generating chunk meshes..." << std::endl;
  for (auto chunk : chunks) {
    chunk->generateMesh();
  }

  auto chunkEndTime = std::chrono::high_resolution_clock::now();
  float chunkGenTime = std::chrono::duration<float, std::chrono::seconds::period>(chunkEndTime - chunkStartTime).count();

  std::cout << "World generation took " << chunkGenTime << " seconds" << std::endl;

  std::cout << "Number of gameobjects: " << gameObjects.size() << std::endl;
}
} // namespace lve