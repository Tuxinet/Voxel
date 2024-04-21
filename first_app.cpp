#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "lve_game_object.hpp"
#include "lve_model.hpp"
#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <iostream>
#include <stdexcept>

namespace lve {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}
void FirstApp::run() {
  SimpleRenderSystem simpleRenderSystem(lveDevice,
                                        lveRenderer.getSwapChainRenderPass());

  LveCamera camera{};
  camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

  auto viewerObject = LveGameObject::createGameObject();
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!lveWindow.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime,
                                   viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    float aspect = lveRenderer.getAspectRatio();
    // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

    if (auto commandBuffer = lveRenderer.beginFrame()) {
      lveRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
      lveRenderer.endSwapChainRenderPass(commandBuffer);
      lveRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadGameObjects() {
  std::shared_ptr<LveModel> lveModel =
      LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");

  for (int x = 0; x < 20; x++) {
    for (int y = 0; y < 20; y++) {
      for (int z = 0; z < 20; z++) {
        auto cube = LveGameObject::createGameObject();
        cube.model = lveModel;
        cube.transform.translation = {-2.5f + x * 1.2f, 1.5f + y * 1.2f, 2.5f + z * 1.2f};
        cube.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(cube));
      }
    }
  }

  std::cout << "Number of gameobjects: " << gameObjects.size() << std::endl;
}
} // namespace lve