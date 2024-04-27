#pragma once

#include "lve_block.hpp"
#include "lve_chunk.hpp"
#include "lve_device.hpp"

// std
#include <map>

namespace lve {
class LveWorld {

public:
  LveWorld(LveDevice &device) : lveDevice{device} {};

  BlockState getBlock(int32_t x, int32_t y, int32_t z);
  std::vector<std::shared_ptr<LveChunk>> getChunksAroundPosition(int32_t x, int32_t z, int32_t radius);

private:
  void initializeChunk(int32_t x, int32_t z);

  std::map<std::pair<int, int>, std::shared_ptr<LveChunk>> m_world{};

  LveDevice &lveDevice;
};
} // namespace lve