#pragma once

#include "systems/lve_chunk.hpp"
#include "systems/lve_descriptors.hpp"
#include "systems/lve_device.hpp"
#include "systems/lve_game_object.hpp"
#include "systems/lve_model.hpp"
#include "systems/lve_renderer.hpp"
#include "systems/lve_window.hpp"
#include "systems/lve_world.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();

private:
  void loadChunks();

  LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
  LveDevice lveDevice{lveWindow};
  LveRenderer lveRenderer{lveWindow, lveDevice};

  // note: order of declarations matters
  std::unique_ptr<LveDescriptorPool> globalPool{};
  LveGameObject::Map gameObjects;
  LveWorld world{lveDevice};
};
} // namespace lve