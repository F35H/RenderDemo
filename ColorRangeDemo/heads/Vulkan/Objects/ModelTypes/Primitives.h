#pragma once

#include <Vulkan/GeneralIncludes.h>
#include <ConsoleUI/UISingletons.h>

class FaceFactory {
  //FillT this with classes that build primitives like dodecahedrons, quadrehons, and 
}; //

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec3 norm;
}; //Vertex

class Polytope {
  std::optional<uint16_t> recurseIndex;

public:
  uint16_t edgeCount; //temporary until face class comes into play
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  size_t GetBufferSize() {
    return sizeof(Vertex) * vertices.size();
  }; //GetBufferSize

  size_t GetIndiceSize() {
    return indices.size();
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
