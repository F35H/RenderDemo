#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <Vulkan/GeneralIncludes.h>
#include <ConsoleUI/UISingletons.h>

class FaceFactory {
  //FillT this with classes that build primitives like dodecahedrons, quadrehons, and 
}; //


struct Texture {
  int height;
  int width;
  stbi_uc* data;
  int texChannel;
  VkDeviceSize imageSize;

  Texture(std::string filename) {
    data = stbi_load(filename.c_str(), &width, &height, &texChannel, STBI_rgb_alpha);
    if (data == NULL) throw std::runtime_error("Loading Texture File Failed");
    imageSize = width * height * 4;
  }; //Texture
}; //Texture

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec3 norm;
  glm::vec2 texPos;
}; //Vertex

class Polytope {
  std::optional<uint16_t> recurseIndex;

public:
  uint16_t edgeCount; //temporary until face class comes into play
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::shared_ptr<Texture> texture;

  size_t GetBufferSize() {
    return sizeof(Vertex) * vertices.size();
  }; //GetBufferSize

  uint32_t GetIndiceSize() {
    return indices.size() /** sizeof(uint32_t)*/;
  }; //GetBufferSize

  void Shrink(uint16_t divisor) {
    if (!recurseIndex) recurseIndex = vertices.size() - 1;
    if (recurseIndex.value() > vertices.size()) { recurseIndex.reset(); return; };
    vertices[recurseIndex.value()].pos /= divisor;
    recurseIndex.value() -= 1;
    Shrink(divisor);
  }; //Shrink

  void GenerateNorm() {
    for (size_t i = 0; i < indices.size(); i += 3)
    {
      // Get the face normal
      auto vector1 = vertices[indices[(size_t)i + 1]].pos - vertices[indices[i]].pos;
      auto vector2 = vertices[indices[(size_t)i + 2]].pos - vertices[indices[i]].pos;
      auto faceNormal = glm::cross(vector1, vector2);
      glm::normalize(faceNormal);

      // Add the face normal to the 3 vertices normal touching this face
      vertices[indices[i]].norm += faceNormal;
      vertices[indices[(size_t)i + 1]].norm += faceNormal;
      vertices[indices[(size_t)i + 2]].norm += faceNormal;
    }

    for (size_t i = 0; i < vertices.size(); ++i) glm::normalize(-vertices[i].norm);
  }; //GenerateNorm

  enum ProjType {
    norms = 0,
    verts
  }; //ProjType

  void ProjectSphereUV(ProjType type, glm::mat3 transform = glm::mat3(1.0f)) {
    for (auto& vertice : vertices) {
      glm::vec3 normVec; 
      if (type == verts) {
        auto transVec = vertice.pos * transform;
        
        normVec.x = std::pow(transVec.x,2);
        normVec.y = std::pow(transVec.y,2);
        normVec.z = std::pow(transVec.x,2);

        auto norm = sqrt(normVec.x + normVec.y + normVec.z);

        normVec = normVec/norm;

        auto u = 0.5 + (2 * std::atan2(normVec.z, normVec.x)) / (2 * 3.14);
        auto v = 0.5 + std::sin(normVec.y);
        vertice.texPos = { u,v };
      }
      else if (type == norms) {
        auto transVec = vertice.norm * transform;

        normVec.x = std::pow(transVec.x, 2);
        normVec.y = std::pow(transVec.y, 2);
        normVec.z = std::pow(transVec.x, 2);

        auto norm = sqrt(normVec.x + normVec.y + normVec.z);

        normVec = normVec / norm;

        auto u = 0.5 + (2 * std::atan2(normVec.z, normVec.x)) / (2 * 3.14);
        auto v = 0.5 + std::sin(normVec.y);
        vertice.texPos = { u,v };
      }
    }; //Vertices
  }; //ProjectSphere

  void ProjectCylanderUV(ProjType type, glm::mat3 transform = glm::mat3(1.0f)) {
    for (auto& vertice : vertices) {
      if (type == verts) {
        auto transVec = vertice.pos * transform;
        auto u = std::atan2(1, vertice.pos.x)/(2*3.14);
        auto v = std::clamp((vertice.pos.y),-1.f,1.f);
        vertice.texPos = { u,v };      
      } 
      else if (type == norms) {
        auto transVec = vertice.norm * transform;
        auto u = std::atan2(1, transVec.x) / (2 * 3.14);
        auto v = std::clamp((transVec.y), -1.f, 1.f);
        vertice.texPos = { u,v };
      }
    }; //Vertices
  }; //ProjectSphere

  void ProjectLinear(ProjType type, glm::mat3 transform = glm::mat3(1.0f)) {
    for (auto& vertice : vertices) {
      glm::vec3 normVec;
      if (type == verts) {
        auto transVec = vertice.pos * transform;
        normVec.x = std::pow(transVec.x, 2);
        normVec.y = std::pow(transVec.y, 2);
        normVec.z = std::pow(transVec.x, 2);

        auto norm = sqrt(normVec.x + normVec.y + normVec.z);

        normVec = normVec / norm;

        vertice.texPos = { normVec.x,normVec.z };
      }
      else if (type == norms) {
        auto transVec = vertice.norm * transform;
        
        normVec.x = std::pow(transVec.x, 2);
        normVec.y = std::pow(transVec.y, 2);
        normVec.z = std::pow(transVec.x, 2);

        auto norm = sqrt(normVec.x + normVec.y + normVec.z);

        normVec = normVec / norm;

        vertice.texPos = { normVec.x,normVec.z };
      }
    }; //Vertices
  }; //ProjectLinear
}; //Polytope

class EdgeMesh {
  std::optional<uint16_t> recurseIndex;
  Polytope* polyhedra;
public:
  std::vector<Polytope> edges;

  EdgeMesh(Polytope* polyhedra) {
    CreateEdges();
  }; //Edge

  void CreateEdges() {
    if (!recurseIndex) recurseIndex = polyhedra->edgeCount;
    if (recurseIndex.value() > polyhedra->edgeCount) return;
    CreateEdge(&polyhedra->vertices[recurseIndex.value()], &polyhedra->vertices[recurseIndex.value() - 1]);
    recurseIndex.value() -= 1;
    CreateEdges();
  }; //EdgeMesh

  void CreateEdge(Vertex* initVertice, Vertex* scndVertice) {
    auto polyhedra = new Polytope();

    polyhedra->vertices.resize(4);

    polyhedra->vertices[0].pos = initVertice->pos;
    polyhedra->vertices[1].pos = scndVertice->pos;
    polyhedra->vertices[2].pos = initVertice->norm;
    polyhedra->vertices[3].pos = scndVertice->norm;

    polyhedra->vertices[0].color = { 0,0,0 };
    polyhedra->vertices[1].color = { 0,0,0 };
    polyhedra->vertices[2].color = { 0,0,0 };
    polyhedra->vertices[3].color = { 0,0,0 };

    polyhedra->indices = { 0,1,2,2,3,0 };
    edges.push_back(*polyhedra);
  }; //CreateEdge
};
