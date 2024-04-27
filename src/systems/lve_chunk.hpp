#pragma once

#include "FastNoiseLite/FastNoiseLite.h"
#include "lve_block.hpp"
#include "lve_game_object.hpp"
#include "lve_model.hpp"
#include "systems/lve_descriptors.hpp"
#include "systems/lve_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>
#include <thread>

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Z 16
#define CHUNK_SIZE_Y 256

namespace lve {

class LveChunk {
public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, LveChunk>;

  static LveChunk createChunk(int32_t chunk_x, int32_t chunk_z, LveDevice &device) {
    static id_t currentId = 0;
    return LveChunk{chunk_x, chunk_z, currentId++, device};
  }

  void addBlock(LveGameObject &block);
  void startGenerateMesh();
  void generateMesh();
  BlockState getBlock(int32_t x, int32_t y, int32_t z);

  static std::pair<uint32_t, uint32_t> getChunkPosition(float x, float z) {
    int chunk_x = static_cast<int>(std::floor(static_cast<double>(x) / CHUNK_SIZE_X));
    int chunk_z = static_cast<int>(std::floor(static_cast<double>(z) / CHUNK_SIZE_Z));

    return std::make_pair(chunk_x, chunk_z);
  }

  static uint32_t getChunkPositionX(float x) { return static_cast<int>(std::floor(static_cast<double>(x) / CHUNK_SIZE_X)); }

  static uint32_t getChunkPositionZ(float z) { return static_cast<int>(std::floor(static_cast<double>(z) / CHUNK_SIZE_Z)); }

  id_t getId() { return id; }

  LveGameObject::Map blocks;
  TransformComponent transform{};
  std::shared_ptr<LveModel> model{};
  const int32_t m_pos_x;
  const int32_t m_pos_z;

  bool hasStartedThread = false;

  bool dirtyMesh{true};

private:
  LveChunk(int32_t chunk_x, int32_t chunk_z, id_t objId, LveDevice &device)
      : id{objId}, lveDevice(device), m_pos_x{chunk_x}, m_pos_z{chunk_z} {
    transform.translation.x = chunk_x * CHUNK_SIZE_X;
    transform.translation.z = chunk_z * CHUNK_SIZE_Z;

    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetSeed(1);
  };

  id_t id;
  LveDevice &lveDevice;

  FastNoiseLite noise{};
};
} // namespace lve