#include "lve_chunk.hpp"
#include "lve_model.hpp"

// std
#include <iostream>
#include <vector>

namespace lve {
void LveChunk::addBlock(LveGameObject &block) {
  blocks.emplace(block.getId(), std::move(block));
  dirtyMesh = true;
}

void LveChunk::generateMesh() {
  this->dirtyMesh = false;

  std::vector<LveModel::Vertex> vertices;
  std::vector<uint32_t> indices;

  for (auto &kv : blocks) {
    auto &block = kv.second;
    // getVertices returns a const reference, therefore we copy the vertex
    for (auto vert : block.model->getVertices()) {
      // Apply the object-position as a vertex-offset. Not great, not terrible. Should really use the chunk as the
      // reference-point
      // TODO: Apply offset relative to chunk-position when we get child->parent relations in gameobjects
      vert.position += block.transform.translation;

      vertices.push_back(vert);
    }

    // Now to copy the indices. They already contain the correct data, we just need to apply the appropiate offset so that
    // they reference the right values in the vertex-array

    auto offset = vertices.size();
    for (auto index : block.model->getIndices()) {
      indices.push_back(index + offset);
    }
  }

  // Generate the model
  model = LveModel::createModelFromBuffers(this->lveDevice, vertices, indices);
}
} // namespace lve
