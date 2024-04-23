#pragma once

#include "lve_game_object.hpp"
#include "lve_model.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace lve {

class LveChunk {
public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, LveChunk>;

  static LveChunk createChunk() {
    static id_t currentId = 0;
    return LveChunk{currentId++};
  }

  void addBlock(LveGameObject& block);
  void generateMesh();


  id_t getId() { return id; }
  LveGameObject::Map blocks;

private:
  bool dirtyMesh{false};
  std::shared_ptr<LveModel> model{};
  LveChunk(id_t objId) : id{objId} {}
  id_t id;
};
} // namespace lve