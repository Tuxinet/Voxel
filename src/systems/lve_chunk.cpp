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

BlockInfo LveChunk::getBlock(int32_t x, int32_t y, int32_t z) {

  int chunk_x = static_cast<int>(std::floor(static_cast<double>(x) / CHUNK_SIZE_X));
  int chunk_z = static_cast<int>(std::floor(static_cast<double>(z) / CHUNK_SIZE_Z));
  y = CHUNK_SIZE_Y - y;

  //if (chunk_x != m_pos_x || chunk_z != m_pos_z || y < 0 || y >= CHUNK_SIZE_Y)
  //  return BlockState::AIR;

  auto idx = std::make_pair(x, z);
  auto value = 0;
  if (noiseValues.count(idx) > 0) {
    value = noiseValues[idx];
  }

  else {
    auto val1 = noise2D.GetNoise(static_cast<float>(x) * 2.f, static_cast<float>(z) * 2.f);
    auto val2 = noise2D.GetNoise(static_cast<float>(x) * 0.2f, static_cast<float>(z) * 0.2f);
    auto val3 = noise2D.GetNoise(static_cast<float>(x) * 10.0f, static_cast<float>(z) * 10.0f);

    value = 100 + (val2 * 50);

    auto local_x = x % CHUNK_SIZE_X;
    auto local_z = z % CHUNK_SIZE_Z;
    auto local_y = y;

    noiseValues[idx] = value;
  }

  if (y > value) {
    if (y < 100) {
      return BlockInfo{BlockState::WATER, true};
    }
    else {
      return BlockInfo{BlockState::AIR, true};
    }
  } else {
    return BlockInfo{BlockState::STONE, false};
  }
}

void LveChunk::generateSolidMesh() {
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
        // Now we have to test all around the cube. Every face that has something touching it does not warrant face

        auto blockInfo = getBlock(x, y, z);

        if (blockInfo.state == BlockState::AIR)
          continue;

        blockVertices.clear();
        if (getBlock(x, y - 1, z).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), topFace.begin(), topFace.end());
        if (getBlock(x, y + 1, z).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), bottomFace.begin(), bottomFace.end());
        if (getBlock(x, y, z - 1).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), tailFace.begin(), tailFace.end());
        if (getBlock(x, y, z + 1).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), noseFace.begin(), noseFace.end());
        if (getBlock(x + 1, y, z).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), rightFace.begin(), rightFace.end());
        if (getBlock(x - 1, y, z).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), leftFace.begin(), leftFace.end());

        // We actually got a block that was not fully surrounded
        if (blockVertices.size() > 0) {
          auto local_x = x - global_x;
          auto local_z = z - global_z;

          glm::vec3 offset = {local_x, y, local_z};

          for (auto vert : blockVertices) {
            vert.position += offset;

            // For debug purposes only
            vert.color.x = static_cast<float>(local_x) / CHUNK_SIZE_X;
            vert.color.z = static_cast<float>(local_z) / CHUNK_SIZE_Z;
            vert.color.y = static_cast<float>(CHUNK_SIZE_Y - y) / CHUNK_SIZE_Y;

            auto r = (float)(std::rand()) / (float)(RAND_MAX);

            if (blockInfo.state == BlockState::STONE) {
              vert.color.x = 0.2f + r * 0.1f;
              vert.color.y = 0.2f + r * 0.1f;
              vert.color.z = 0.2f + r * 0.1f;
            } else if (blockInfo.state == BlockState::WATER) {
              vert.color.x = 0.f;
              vert.color.y = 0.f;
              vert.color.z = 0.3f + r * 0.1f;
            }

            vertices.push_back(vert);
          }
        }
      }
    }
  }

  // Generate the model
  solidModel = LveModel::createModelFromBuffers(this->lveDevice, vertices, indices);
  this->dirtyMesh = false;
  hasStartedThread = false;
}

void LveChunk::generateTransparentMesh() {
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
        // Now we have to test all around the cube. Every face that has something touching it does not warrant face

        auto blockInfo = getBlock(x, y, z);

        if (blockInfo.state == BlockState::AIR)
          continue;

        blockVertices.clear();
        if (getBlock(x, y - 1, z).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), topFace.begin(), topFace.end());
        if (getBlock(x, y + 1, z).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), bottomFace.begin(), bottomFace.end());
        if (getBlock(x, y, z - 1).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), tailFace.begin(), tailFace.end());
        if (getBlock(x, y, z + 1).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), noseFace.begin(), noseFace.end());
        if (getBlock(x + 1, y, z).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), rightFace.begin(), rightFace.end());
        if (getBlock(x - 1, y, z).state == BlockState::AIR)
          blockVertices.insert(blockVertices.end(), leftFace.begin(), leftFace.end());

        // We actually got a block that was not fully surrounded
        if (blockVertices.size() > 0) {
          auto local_x = x - global_x;
          auto local_z = z - global_z;

          glm::vec3 offset = {local_x, y, local_z};

          for (auto vert : blockVertices) {
            vert.position += offset;

            // For debug purposes only
            vert.color.x = static_cast<float>(local_x) / CHUNK_SIZE_X;
            vert.color.z = static_cast<float>(local_z) / CHUNK_SIZE_Z;
            vert.color.y = static_cast<float>(CHUNK_SIZE_Y - y) / CHUNK_SIZE_Y;

            auto r = (float)(std::rand()) / (float)(RAND_MAX);

            if (blockInfo.state == BlockState::STONE) {
              vert.color.x = 0.2f + r * 0.1f;
              vert.color.y = 0.2f + r * 0.1f;
              vert.color.z = 0.2f + r * 0.1f;
            } else if (blockInfo.state == BlockState::WATER) {
              vert.color.x = 0.f;
              vert.color.y = 0.f;
              vert.color.z = 0.3f + r * 0.1f;
            }

            vertices.push_back(vert);
          }
        }
      }
    }
  }

  // Generate the model
  solidModel = LveModel::createModelFromBuffers(this->lveDevice, vertices, indices);
  this->dirtyMesh = false;
  hasStartedThread = false;
}
} // namespace lve
