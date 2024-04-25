#pragma once

#include "lve_model.hpp"

namespace lve {
enum BlockState { AIR, STONE, DIRT };

class Block {
public:

static std::vector<LveModel::Vertex> leftFace() {
  std::vector<LveModel::Vertex> vertices {
    
      {{-.5f, -.5f, -.5f}, {0.f, 1.f, 0.f}, {-1.f,0,0}},
      {{-.5f, .5f, .5f}, {0.f, 1.f, 0.f}, {-1.f,0,0}},
      {{-.5f, -.5f, .5f}, {0.f, 1.f, 0.f}, {-1.f,0,0}},
      {{-.5f, -.5f, -.5f}, {0.f, 1.f, 0.f}, {-1.f,0,0}},
      {{-.5f, .5f, -.5f}, {0.f, 1.f, 0.f}, {-1.f,0,0}},
      {{-.5f, .5f, .5f}, {0.f, 1.f, 0.f}, {-1.f,0,0}},
  };

  return vertices;
};

static std::vector<LveModel::Vertex> rightFace() {
  std::vector<LveModel::Vertex> vertices {
    
      {{.5f, -.5f, -.5f}, {0.f, 1.f, 0.f}, {1.f,0,0}},
      {{.5f, .5f, .5f}, {0.f, 1.f, 0.f}, {1.f,0,0}},
      {{.5f, -.5f, .5f}, {0.f, 1.f, 0.f}, {1.f,0,0}},
      {{.5f, -.5f, -.5f}, {0.f, 1.f, 0.f}, {1.f,0,0}},
      {{.5f, .5f, -.5f}, {0.f, 1.f, 0.f}, {1.f,0,0}},
      {{.5f, .5f, .5f}, {0.f, 1.f, 0.f}, {1.f,0,0}},
  };

  return vertices;
};

static std::vector<LveModel::Vertex> topFace() {
  std::vector<LveModel::Vertex> vertices {
    
      {{-.5f, -.5f, -.5f}, {0.f, 1.f, 0.f}, {0.f,-1.f,0.f}},
      {{.5f, -.5f, .5f}, {0.f, 1.f, 0.f}, {0.f,-1.f,0.f}},
      {{-.5f, -.5f, .5f}, {0.f, 1.f, 0.f}, {0.f,-1.f,0.f}},
      {{-.5f, -.5f, -.5f}, {0.f, 1.f, 0.f}, {0.f,-1.f,0.f}},
      {{.5f, -.5f, -.5f}, {0.f, 1.f, 0.f}, {0.f,-1.f,0.f}},
      {{.5f, -.5f, .5f}, {0.f, 1.f, 0.f}, {0.f,-1.f,0.f}},
  };

  return vertices;
};

static std::vector<LveModel::Vertex> bottomFace() {
  std::vector<LveModel::Vertex> vertices {
    
      {{-.5f, .5f, -.5f}, {0.f, 1.f, 0.f}, {0.f,1.f,0.f}},
      {{.5f, .5f, .5f}, {0.f, 1.f, 0.f}, {0.f,1.f,0.f}},
      {{-.5f, .5f, .5f}, {0.f, 1.f, 0.f}, {0.f,1.f,0.f}},
      {{-.5f, .5f, -.5f}, {0.f, 1.f, 0.f}, {0.f,1.f,0.f}},
      {{.5f, .5f, -.5f}, {0.f, 1.f, 0.f}, {0.f,1.f,0.f}},
      {{.5f, .5f, .5f}, {0.f, 1.f, 0.f}, {0.f,1.f,0.f}},
  };

  return vertices;
};

static std::vector<LveModel::Vertex> noseFace() {
  std::vector<LveModel::Vertex> vertices {
    
      {{-.5f, -.5f, 0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,1.f}},
      {{.5f, .5f, 0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,1.f}},
      {{-.5f, .5f, 0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,1.f}},
      {{-.5f, -.5f, 0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,1.f}},
      {{.5f, -.5f, 0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,1.f}},
      {{.5f, .5f, 0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,1.f}},
  };

  return vertices;
};

static std::vector<LveModel::Vertex> tailFace() {
  std::vector<LveModel::Vertex> vertices {
    
      {{-.5f, -.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,-1.f}},
      {{.5f, .5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,-1.f}},
      {{-.5f, .5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,-1.f}},
      {{-.5f, -.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,-1.f}},
      {{.5f, -.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,-1.f}},
      {{.5f, .5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f,0.f,-1.f}},
  };

  return vertices;
};

private:


};
} // namespace lve