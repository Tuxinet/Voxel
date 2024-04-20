#include "first_app.hpp"

#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>
#include <iostream>

namespace lve {

    FirstApp::FirstApp() {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}
    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem(lveDevice, lveRenderer.getSwapChainRenderPass());
        while (!lveWindow.shouldClose()) {
            glfwPollEvents();
            

            if (auto commandBuffer = lveRenderer.beginFrame()) {
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects() {
        std::vector<LveModel::Vertex> vertices {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

        auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);

        for (int i = 0; i < 1000; i++) {
            auto triangle = LveGameObject::createGameObject();
            triangle.model = lveModel;
            triangle.color = {.001f * i, 0.0f, 0.0f};
            //triangle.transform2d.translation.x = 0.2f;
            triangle.transform2d.scale = {0.001f * i, 0.001f * i};
            triangle.transform2d.rotation = .25f * glm::two_pi<float>();
            triangle.transform2d.rotationSpeed = 0.00001f * i;

            gameObjects.push_back(std::move(triangle));
        }
    }
}