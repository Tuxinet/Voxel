#pragma once

#include "lve_game_object.hpp"
#include "lve_model.hpp"
#include "systems/lve_descriptors.hpp"
#include "systems/lve_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace lve {

class LveChunk {
public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, LveChunk>;

  static LveChunk createChunk(LveDevice &device) {
    static id_t currentId = 0;
    return LveChunk{currentId++, device};
  }

  void addBlock(LveGameObject& block);
  void generateMesh();

  id_t getId() { return id; }

  LveGameObject::Map blocks;
  TransformComponent transform{};
  std::shared_ptr<LveModel> model{};

private:
  bool dirtyMesh{false};
  LveChunk(id_t objId, LveDevice &device) : id{objId}, lveDevice(device) {}
  id_t id;
  LveDevice &lveDevice;
};
} // namespace lve