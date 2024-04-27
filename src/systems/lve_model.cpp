#include "lve_model.hpp"

#include "lve_buffer.hpp"
#include "lve_utils.hpp"
#include <memory>
#include <mutex>
#include <vulkan/vulkan_core.h>

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace std {
template <> struct hash<lve::LveModel::Vertex> {
  size_t operator()(lve::LveModel::Vertex const &vertex) const {
    size_t seed = 0;
    lve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
    return seed;
  }
};
} // namespace std

namespace lve {
LveModel::LveModel(LveDevice &device, const LveModel::Builder &builder) : lveDevice{device}, builder{builder} {
  std::lock_guard<std::mutex> guard(m_createBuffersMutex);
  createVertexBuffers(builder.m_vertices);
  createIndexBuffers(builder.m_indices);
}

LveModel::~LveModel() {}

std::unique_ptr<LveModel> LveModel::createModelFromFile(LveDevice &device, const std::string filepath) {
  Builder builder{};
  builder.loadModel(ENGINE_DIR + filepath);

  return std::make_unique<LveModel>(device, builder);
}

std::unique_ptr<LveModel> LveModel::createModelFromVertices(LveDevice &device, const std::vector<Vertex> &vertices) {
  Builder builder{};
  builder.loadVertices(vertices);

  return std::make_unique<LveModel>(device, builder);
}

std::unique_ptr<LveModel> LveModel::createModelFromBuffers(LveDevice &device, const std::vector<Vertex> &vertices,
                                                           const std::vector<uint32_t> &indices) {
  Builder builder{};
  builder.loadBuffers(vertices, indices);

  return std::make_unique<LveModel>(device, builder);
}

void LveModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
  uint32_t vertexSize = sizeof(vertices[0]);

  LveBuffer stagingBuffer{
      lveDevice,
      vertexSize,
      vertexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *)vertices.data());

  vertexBuffer = std::make_unique<LveBuffer>(lveDevice, vertexSize, vertexCount,
                                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  lveDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void LveModel::createIndexBuffers(const std::vector<uint32_t> &indices) {
  indexCount = static_cast<uint32_t>(indices.size());
  hasIndexBuffer = indexCount > 0;

  if (!hasIndexBuffer) {
    return;
  }

  VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

  uint32_t indexSize = sizeof(indices[0]);

  LveBuffer stagingBuffer{lveDevice, indexSize, indexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT};

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *)indices.data());

  indexBuffer = std::make_unique<LveBuffer>(lveDevice, indexSize, indexCount,
                                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  lveDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

void LveModel::draw(VkCommandBuffer commandBuffer) {
  if (hasIndexBuffer) {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  } else {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  }
}

void LveModel::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer->getBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
  }
}

std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions() {
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions() {
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

  attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
  attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
  attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
  attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

  return attributeDescriptions;
}

void LveModel::Builder::loadVertices(const std::vector<Vertex> &vertices) {
  m_vertices.clear();
  m_indices.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (auto &vertex : vertices) {
    if (uniqueVertices.count(vertex) == 0) {
      uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
      m_vertices.push_back(vertex);
    }
    m_indices.push_back(uniqueVertices[vertex]);
  }
}

void LveModel::Builder::loadBuffers(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) {
  // m_vertices.clear();
  // m_indices.clear();

  // TODO: Check if this is the right way to copy the vectors
  // (https://stackoverflow.com/questions/644673/is-it-more-efficient-to-copy-a-vector-by-reserving-and-copying-or-by-creating-a)

  m_vertices = vertices;
  m_indices = indices;
}

// For now we only combine the vertices, and don't care about everything else
void LveModel::Builder::combineModels(std::vector<std::shared_ptr<LveModel>> &models) {}

void LveModel::Builder::loadModel(const std::string &filepath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
    throw std::runtime_error(warn + err);
  }

  m_vertices.clear();
  m_indices.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};

      if (index.vertex_index >= 0) {
        vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2],
        };

        vertex.color = {
            attrib.colors[3 * index.vertex_index + 0],
            attrib.colors[3 * index.vertex_index + 1],
            attrib.colors[3 * index.vertex_index + 2],
        };
      }

      if (index.normal_index >= 0) {
        vertex.normal = {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2],
        };
      }

      if (index.texcoord_index >= 0) {
        vertex.uv = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            attrib.texcoords[2 * index.texcoord_index + 1],
        };
      }

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
        m_vertices.push_back(vertex);
      }
      m_indices.push_back(uniqueVertices[vertex]);
    }
  }
}

} // namespace lve