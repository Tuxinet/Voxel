#pragma once

#include "lve_buffer.hpp"
#include "lve_device.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>
#include <mutex>

namespace lve {
class LveModel {

public:
  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 uv;

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

    bool operator==(const Vertex &other) const {
      return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
    }
  };

  struct Builder {
    std::vector<Vertex> m_vertices{};
    std::vector<uint32_t> m_indices{};

    void loadModel(const std::string &filepath);
    void loadVertices(const std::vector<Vertex> &vertices);
    void loadBuffers(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
    void combineModels(std::vector<std::shared_ptr<LveModel>> &models);
  };

  LveModel(LveDevice &device, const LveModel::Builder &builder);
  ~LveModel();

  LveModel(const LveModel &) = delete;
  LveModel &operator=(const LveModel &) = delete;

  static std::unique_ptr<LveModel> createModelFromFile(LveDevice &device, const std::string filepath);
  static std::unique_ptr<LveModel> createModelFromVertices(LveDevice &device, const std::vector<Vertex> &vertices);
  static std::unique_ptr<LveModel> createModelFromBuffers(LveDevice &device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);
  bool usesIndexBuffer() { return hasIndexBuffer; };

  const std::vector<Vertex> &getVertices() const { return builder.m_vertices; }
  const std::vector<uint32_t> &getIndices() const { return builder.m_indices; }
  inline static std::mutex m_createBuffersMutex;

private:
  Builder builder;

  void createVertexBuffers(const std::vector<Vertex> &vertices);
  void createIndexBuffers(const std::vector<uint32_t> &indices);

  LveDevice &lveDevice;

  std::unique_ptr<LveBuffer> vertexBuffer;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  std::unique_ptr<LveBuffer> indexBuffer;
  uint32_t indexCount;

};
} // namespace lve