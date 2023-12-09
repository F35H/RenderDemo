#pragma once

#include <Vulkan/GeneralIncludes.h>
#include <ConsoleUI/UISingletons.h>

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec3 norm;
}; //Vertex

class Polyhedra {
public:
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;

  size_t GetBufferSize() {
    return sizeof(Vertex) * vertices.size();
  }; //GetBufferSize

  size_t GetIndiceSize() {
    return indices.size();
  }; //GetBufferSize

  void GenerateNorm() {
    for (size_t i = 0; i < indices.size(); i += 3)
    { 
      // Get the face normal
      auto vector1 = vertices[indices[(size_t)i + 1]].pos - vertices[indices[i]].pos;
      auto vector2 = vertices[indices[(size_t)i + 2]].pos - vertices[indices[i]].pos;
      auto faceNormal = glm::cross(vector1, vector2);
      glm::normalize(faceNormal);

      // Add the face normal to the 3 vertices normal touching this face
      vertices[indices[i]].norm             += faceNormal;
      vertices[indices[(size_t)i + 1]].norm += faceNormal;
      vertices[indices[(size_t)i + 2]].norm += faceNormal;
    }

    for (size_t i = 0; i < vertices.size(); ++i) glm::normalize(vertices[i].norm);
  }; //GenerateNorm
}; //Polyhedra

//class Edge : protected Polyhedra {
//  Edge(Vertex* edge) {
//    size_t i = 0;
//    for (; i < polyhedra->vertices.size(); ++i) {
//      polyhedra->vertices[i].norm;
//    }; //for i < polyhedra->vertices.size()
//
//    indices = {0,1,2,2,3}
//  }; //Edge
//}; //Edge
//
//class EdgeMesh : Edge {
//public:
//  EdgeMesh(Polyhedra* polyhedra) {
//  }; //Edge
//};

namespace Polytopes {
  struct Triangle : Polyhedra {
    Triangle() {
      vertices.resize(3);

      vertices[0].pos = { 0.0f, -0.5f, 0.0f };
      vertices[1].pos = { 0.5f, 0.5f, 0.0f };
      vertices[2].pos = { -0.5f, 0.5f, 0.0f };

      vertices[0].color = { 1.0f, 0.0f, 0.0f };
      vertices[1].color = { 0.0f, 1.0f, 0.0f };
      vertices[2].color = { 0.0f, 0.0f, 1.0f };

      indices = { 0,1,2 };
      
      GenerateNorm();
    }; //Triangle Ctor
  }; //Triangle

  struct Quad : Polyhedra {
    Quad() {
      vertices.resize(4);

      vertices[0].pos = { -0.5f, -0.5f, 0.0f };
      vertices[1].pos = { 0.5f, -0.5f, 0.0f };
      vertices[2].pos = { 0.5f, 0.5f, 0.0f };
      vertices[3].pos = { -0.5f, 0.5f, 0.0f };

      vertices[0].color = { 1.0f, 0.0f, 0.0f };
      vertices[1].color = { 0.0f, 1.0f, 0.0f };
      vertices[2].color = { 0.0f, 0.0f, 1.0f };
      vertices[3].color = { 1.0f, 1.0f, 1.0f };

      indices = {
        0, 1, 2, 
        2, 3, 0
      }; //indices

      GenerateNorm();
    }; //Quad Ctor
  }; //Quad

  struct Hexagon : Polyhedra {
    Hexagon() {
      vertices.resize(7);

      vertices[0].pos = { 0.5f, 0.0f,  0.0f };
      vertices[1].pos = { 0.25f, 0.43f, 0.0f };
      vertices[2].pos = { -0.25f, 0.43f, 0.0f };
      vertices[3].pos = { -0.5f, 0.0f, 0.0f };
      vertices[4].pos = { -0.25f, -0.43f, 0.0f };
      vertices[5].pos = { 0.25f, -0.43f, 0.0f };
      vertices[6].pos = { 0.0f, 0.0f, 0.0f };

      vertices[0].color = { 0.0f, 1.0f, 0.0f };
      vertices[1].color = { 0.0f, 1.0f, 1.0f };
      vertices[2].color = { 0.0f, 0.0f, 1.0f };
      vertices[3].color = { 1.0f, 0.0f, 1.0f };
      vertices[4].color = { 1.0f, 0.0f, 0.0f };
      vertices[5].color = { 1.0f, 1.0f, 0.0f };
      vertices[6].color = { 1.0f, 1.0f, 1.0f };

      GenerateNorm();
      
      indices = {
        0, 1, 6,
        1, 2, 6,
        2, 3, 6,
        3, 4, 6,
        4, 5, 6,
        5, 0, 6
      }; //indices
    }; //Hexagon Ctor
  }; //Cube

  struct Tetrahedron : Polyhedra {
    Tetrahedron() {
      vertices.resize(4);

      //-0.3535 is half of 2/sqrt(2)
      vertices[0].pos = { 0.5f, 0.0f, -0.3535 };
      vertices[1].pos = { -0.5f, 0.0f, -0.3535 };
      vertices[3].pos = { 0.0f, -0.5f, 0.3535 };
      vertices[2].pos = { 0.0f, 0.5f, 0.3535 };

      vertices[0].color = { 1.0f, 0.0f, 0.0f };
      vertices[1].color = { 0.0f, 1.0f, 0.0f };
      vertices[2].color = { 0.0f, 0.0f, 1.0f };
      vertices[3].color = { 1.0f, 1.0f, 1.0f };


      indices = {
        2, 1, 0,
        0, 1, 3,
        3, 2, 0,
        1, 2, 3
      }; //indices

      GenerateNorm();
    }; //Tetrahedon Ctor
  }; //Tetrahedron

  struct Cube : Polyhedra {
    Cube() {
      vertices.resize(8);

      vertices[0].pos = { 0.5f, 0.5f, 0.5f };
      vertices[1].pos = { -0.5f, 0.5f, 0.5f };
      vertices[2].pos = { 0.5f, -0.5f, 0.5f };
      vertices[3].pos = { -0.5f, -0.5f, 0.5f };
      vertices[4].pos = { 0.5f, 0.5f, -0.5f };
      vertices[5].pos = { 0.5f, -0.5f, -0.5f };
      vertices[6].pos = { -0.5f, -0.5f, -0.5f };
      vertices[7].pos = { -0.5f, 0.5f, -0.5f };

      vertices[0].color = { 0.0f, 0.0f, 0.0f };
      vertices[1].color = { 1.0f, 0.0f, 0.0f };
      vertices[2].color = { 0.0f, 1.0f, 0.0f };
      vertices[3].color = { 0.0f, 0.0f, 1.0f };
      vertices[4].color = { 1.0f, 1.0f, 1.0f };
      vertices[5].color = { 0.0f, 0.5f, 0.5f };
      vertices[6].color = { 0.5f, 0.0f, 0.5f };
      vertices[7].color = { 0.5f, 0.5f, 0.0f };


      indices = {
        0,2,3,
        3,1,0,

        0,4,5,
        5,2,0,

        //2,3,4,
        //4,5,2,

        //3,4,5,
        //5,6,3,

        //4,5,6,
        //6,7,4,

        //5,6,7,
        //7,0,5,

        //6,7,0,
        //0,1,6,

        //7,0,1,
        //1,2,7
      }; //indices

      GenerateNorm();
    }; //Cube Ctor
  }; //Cube

  struct Octahedron : Polyhedra {
    Octahedron() {
      vertices.resize(6);

      //.2928 is added to correct edgelength of sqrt(2)
      vertices[1].pos = { 1.2928f, 0.0f, 0.0f };
      vertices[2].pos = { -1.2928f, 0.0f, 0.0f };
      vertices[3].pos = { 0.0f, 0.0f, -1.2928f };
      vertices[5].pos = { 0.0f, 0.0f, 1.2928f };

      vertices[0].pos = { 0.0f, -1.2928f, 0.0f };
      vertices[4].pos = { 0.0f, 1.2928f, 0.0f };
      
      vertices[0].color = { 0.0f, 0.0f, 0.0f };
      vertices[1].color = { 1.0f, 0.0f, 0.0f };
      vertices[2].color = { 0.0f, 1.0f, 0.0f };
      vertices[3].color = { 0.0f, 0.0f, 1.0f };
      vertices[4].color = { 1.0f, 1.0f, 1.0f };
      vertices[5].color = { 0.5f, 0.5f, 0.5f };

      indices = {

      }; //indices
    }; //Octahedron
  
  }; //Octahedron

  struct Icosahedron : Polyhedra {
    Icosahedron() {

    }; //Icosahedron
  }; //Icosahedron

  struct Dodecahedron : Polyhedra {
  }; //Dodecahedron
}; //Polytopes

