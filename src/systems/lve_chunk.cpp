#include "lve_chunk.hpp"

namespace lve {
  void LveChunk::addBlock(LveGameObject& block) {
    blocks.emplace(block.getId(), std::move(block));
  }

  void LveChunk::generateMesh() {
    return;
  }
}
