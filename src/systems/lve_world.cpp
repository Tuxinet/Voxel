#include "lve_world.hpp"

// std
#include <iostream>

namespace lve {
BlockState LveWorld::getBlock(int32_t x, int32_t y, int32_t z) {
  // First find the chunk-index
  auto chunk_x = x / CHUNK_SIZE_X;
  auto chunk_z = z / CHUNK_SIZE_Y;

  auto idx = std::make_pair(chunk_x, chunk_z);

  if (m_world.count(idx) == 0) {
    initializeChunk(chunk_x, chunk_z);
  }

  return m_world[idx]->getBlock(x, y, z);
}

void LveWorld::initializeChunk(int32_t x, int32_t z) {
  auto idx = std::make_pair(x, z);

  m_world[idx] = std::make_shared<LveChunk>(LveChunk::createChunk(x, z, lveDevice));
}

// gets all chunks in the span {position-radius, position+radius}
std::vector<std::shared_ptr<LveChunk>> LveWorld::getChunksAroundPosition(int32_t x, int32_t z, int32_t radius) {
  std::vector<std::shared_ptr<LveChunk>> chunks{};

  auto genCounter = 0;
  for (auto chunk_x = x - radius; chunk_x <= x + radius; chunk_x++) {
    for (auto chunk_z = z - radius; chunk_z <= z + radius; chunk_z++) {
      auto idx = std::make_pair(chunk_x, chunk_z);
      if (m_world.count(idx) == 0) {
        initializeChunk(chunk_x, chunk_z);
        genCounter++;
      }

      chunks.push_back(m_world[idx]);
    }
  }

  if (genCounter)
    std::cout << "Generated " << genCounter << " chunks" << std::endl;

  return chunks;
}
} // namespace lve