#include "lve_world.hpp"

namespace lve {
BlockState LveWorld::getBlock(int32_t x, int32_t y, int32_t z) {
  // First find the chunk-index
  auto chunk_x = x / CHUNK_SIZE_X;
  auto chunk_y = y / CHUNK_SIZE_Y;

  auto idx = std::make_pair(chunk_x, chunk_y);

  if (m_world.count(idx) == 0) {
    initializeChunk(chunk_x, chunk_y);
  }

  return m_world[idx]->getBlock(x, y, z);
}

void LveWorld::initializeChunk(int32_t x, int32_t y) {
  auto idx = std::make_pair(x, y);

  m_world[idx] = std::make_shared<LveChunk>(LveChunk::createChunk(x, y, lveDevice));
}

// gets all chunks in the span {position-radius, position+radius}
std::vector<std::shared_ptr<LveChunk>> LveWorld::getChunksAroundPosition(int32_t x, int32_t y, int32_t radius) {
  std::vector<std::shared_ptr<LveChunk>> chunks{};

  for (auto chunk_x = x - radius; chunk_x < x + radius; chunk_x++) {
    for (auto chunk_y = y - radius; chunk_y < y + radius; chunk_y++) {
      auto idx = std::make_pair(chunk_x, chunk_y);
      if (m_world.count(idx) == 0) {
        initializeChunk(chunk_x, chunk_y);
      }

      chunks.push_back(m_world[idx]);
    }
  }

  return chunks;
}
} // namespace lve