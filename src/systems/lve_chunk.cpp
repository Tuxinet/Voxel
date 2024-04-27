#include "lve_chunk.hpp"
#include "lve_model.hpp"
#include "systems/lve_block.hpp"

// std
#include <bits/stdc++.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

namespace lve {
void LveChunk::addBlock(LveGameObject &block) {
  blocks.emplace(block.getId(), std::move(block));
  dirtyMesh = true;
}

BlockState LveChunk::getBlock(int32_t x, int32_t y, int32_t z) {

  int chunk_x = static_cast<int>(std::floor(static_cast<double>(x) / CHUNK_SIZE_X));
  int chunk_z = static_cast<int>(std::floor(static_cast<double>(z) / CHUNK_SIZE_Z));
  y = CHUNK_SIZE_Y - y;

  if (chunk_x != m_pos_x || chunk_z != m_pos_z || y < 0 || y >= CHUNK_SIZE_Y)
    return BlockState::AIR;

  auto val1 = noise.GetNoise(static_cast<float>(x) * 2.f, static_cast<float>(z) * 2.f);
  auto val2 = noise.GetNoise(static_cast<float>(x) * 0.3f, static_cast<float>(z) * 0.3f);
  auto val3 = noise.GetNoise(static_cast<float>(x) * 10.0f, static_cast<float>(z) * 10.0f);

  auto value = 100 + (val2 * 100 + val1 * 10 + val3 * 2);

  auto local_x = x % CHUNK_SIZE_X;
  auto local_z = z % CHUNK_SIZE_Z;
  auto local_y = y;

  if (y > value)
    return BlockState::AIR;
  else
    return BlockState::STONE;
}

void LveChunk::startGenerateMesh() {
  std::cout << "This function is deprecated...\n";
}

void LveChunk::generateMesh() {
  hasStartedThread = true;

  std::vector<LveModel::Vertex> vertices;
  std::vector<uint32_t> indices;

  int global_x = m_pos_x * CHUNK_SIZE_X;
  int global_z = m_pos_z * CHUNK_SIZE_Z;

  std::vector<LveModel::Vertex> blockVertices{};

  auto topFace{Block::topFace()};
  auto bottomFace{Block::bottomFace()};
  auto leftFace{Block::leftFace()};
  auto rightFace{Block::rightFace()};
  auto noseFace{Block::noseFace()};
  auto tailFace{Block::tailFace()};
  auto blockCounter = 0;
  for (auto x = global_x; x < global_x + CHUNK_SIZE_X; x++) {
    for (auto z = global_z; z < global_z + CHUNK_SIZE_Z; z++) {
      for (auto y = 0; y < CHUNK_SIZE_Y; y++) {
        blockCounter++;
        blockVertices.clear();
        // Now we have to test all around the cube. Every face that has something touching it does not warrant face

        if (getBlock(x, y, z) == BlockState::AIR)
          continue;

        if (getBlock(x, y - 1, z) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), topFace.begin(), topFace.end());
        if (getBlock(x, y + 1, z) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), bottomFace.begin(), bottomFace.end());
        if (getBlock(x, y, z - 1) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), tailFace.begin(), tailFace.end());
        if (getBlock(x, y, z + 1) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), noseFace.begin(), noseFace.end());
        if (getBlock(x + 1, y, z) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), rightFace.begin(), rightFace.end());
        if (getBlock(x - 1, y, z) == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), leftFace.begin(), leftFace.end());

        // We actually got a block that was not fully surrounded
        if (blockVertices.size() > 0) {
          auto local_x = x - global_x;
          auto local_z = z - global_z;

          glm::vec3 offset = {local_x, y, local_z};

          for (auto vert : blockVertices) {
            vert.position += offset;

            vert.color.x = static_cast<float>(local_x) / CHUNK_SIZE_X;
            vert.color.z = static_cast<float>(local_z) / CHUNK_SIZE_Z;
            vert.color.y = static_cast<float>(CHUNK_SIZE_Y - y) / CHUNK_SIZE_Y;

            vert.color.x = 0.f;
            vert.color.y = (float)(std::rand()) / (float)(RAND_MAX);
            vert.color.z = 0.f;

            vertices.push_back(vert);
          }
        }
      }
    }
  }

  // Generate the model
  model = LveModel::createModelFromBuffers(this->lveDevice, vertices, indices);
  this->dirtyMesh = false;
  hasStartedThread = false;
}
} // namespace lve
