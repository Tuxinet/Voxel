#include "lve_chunk.hpp"
#include "lve_model.hpp"
#include "systems/lve_block.hpp"

// std
#include <iostream>
#include <vector>

namespace lve {
void LveChunk::addBlock(LveGameObject &block) {
  blocks.emplace(block.getId(), std::move(block));
  dirtyMesh = true;
}

BlockState LveChunk::getBlock(int32_t x, int32_t y, int32_t z) {

  int chunk_x = static_cast<int>(std::floor(static_cast<double>(x) / CHUNK_SIZE_X));
  int chunk_y = static_cast<int>(std::floor(static_cast<double>(y) / CHUNK_SIZE_Y));

  if (chunk_x != m_pos_x || chunk_y != m_pos_y || z < 0 || z >= CHUNK_SIZE_Z)
    return BlockState::AIR;

  auto local_x = x % CHUNK_SIZE_X;
  auto local_y = y % CHUNK_SIZE_Y;
  auto local_z = z;

  return BlockState::STONE;
}

void LveChunk::generateMesh() {
  this->dirtyMesh = false;

  std::vector<LveModel::Vertex> vertices;
  std::vector<uint32_t> indices;

  int global_x = m_pos_x * CHUNK_SIZE_X;
  int global_y = m_pos_y * CHUNK_SIZE_Y;

  auto blockCounter = 0;
  for (auto x = global_x; x < global_x + CHUNK_SIZE_X; x++) {
    for (auto y = global_y; y < global_y + CHUNK_SIZE_Y; y++) {
      for (auto z = 0; z < CHUNK_SIZE_Z; z++) {
        blockCounter++;
        // Now we have to test all around the cube. Every face that has something touching it does not warrant face
        std::vector<LveModel::Vertex> blockVertices{};

        auto topFace{Block::topFace()};
        auto bottomFace{Block::bottomFace()};
        auto leftFace{Block::leftFace()};
        auto rightFace{Block::rightFace()};
        auto noseFace{Block::noseFace()};
        auto tailFace{Block::tailFace()};

        if (getBlock(x, y, z) == BlockState::AIR)
          continue;

        if (getBlock(x, y, z - 1) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), topFace.begin(), topFace.end());
        if (getBlock(x, y, z + 1) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), bottomFace.begin(), bottomFace.end());
        if (getBlock(x, y - 1, z) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), tailFace.begin(), tailFace.end());
        if (getBlock(x, y + 1, z) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), noseFace.begin(), noseFace.end());
        if (getBlock(x + 1, y, z) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), rightFace.begin(), rightFace.end());
        if (getBlock(x - 1, y, z) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), leftFace.begin(), leftFace.end());

        // We actually got a block that was not fully surrounded
        if (blockVertices.size() > 0) {
          auto local_x = x - global_x;
          auto local_y = y - global_y;

          glm::vec3 offset = {local_x, z, local_y};

          for (auto vert : blockVertices) {
            vert.position += offset;

            vert.color.x = static_cast<float>(local_x) / CHUNK_SIZE_X;
            vert.color.y = static_cast<float>(local_y) / CHUNK_SIZE_Y;
            vert.color.z = static_cast<float>(z) / CHUNK_SIZE_Z;

            vertices.push_back(vert);
          }
        }
      }
    }
  }

  std::cout << "Processed " << blockCounter << " blocks to generate mesh..." << std::endl;
  // Generate the model
  model = LveModel::createModelFromBuffers(this->lveDevice, vertices, indices);
}
} // namespace lve
