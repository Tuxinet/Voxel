#pragma once

#include "lve_game_object.hpp"
#include "lve_model.hpp"
#include "systems/lve_descriptors.hpp"
#include "systems/lve_device.hpp"
#include "lve_block.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 256

namespace lve {

class LveChunk {
public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, LveChunk>;

  static LveChunk createChunk(int32_t chunk_x, int32_t chunk_y, LveDevice &device) {
    static id_t currentId = 0;
    return LveChunk{chunk_x, chunk_y, currentId++, device};
  }

  void addBlock(LveGameObject &block);
  void generateMesh();
  BlockState getBlock(int32_t x, int32_t y, int32_t z);

  id_t getId() { return id; }

  LveGameObject::Map blocks;
  TransformComponent transform{};
  std::shared_ptr<LveModel> model{};
  const int32_t m_pos_x;
  const int32_t m_pos_y;

private:
  bool dirtyMesh{false};
  LveChunk(int32_t chunk_x, int32_t chunk_y, id_t objId, LveDevice &device)
      : id{objId}, lveDevice(device), m_pos_x{chunk_x}, m_pos_y{chunk_y} {
        transform.translation.x = chunk_x * CHUNK_SIZE_X;
        transform.translation.z = chunk_y * CHUNK_SIZE_Y;
      };

  id_t id;
  LveDevice &lveDevice;
};
} // namespace lve