#pragma once

#include <vulkan/Objects/ModelTypes/ModelPrimitives.h>

//Indices 
//Triangle
//1,2,3

//Square
//1,2,3
//1,4,3

//Dodecahedron
//1,2,3,
//5,4,3,
//1,5,3

//Hexagon
//1,2,3,
//1,6,3,
//4,6,3,
//4,5,6

namespace Polyhedra {
  struct Triangle : Polytope {
    Triangle(std::string texturePath) {
      edgeCount = 3;
      vertices.resize(3);

      vertices[0].pos = { 0.0f, -0.5f, 0.0f };
      vertices[1].pos = { 0.5f, 0.5f, 0.0f };
      vertices[2].pos = { -0.5f, 0.5f, 0.0f };

      vertices[0].color = { 1.0f, 0.0f, 0.0f };
      vertices[1].color = { 0.0f, 1.0f, 0.0f };
      vertices[2].color = { 0.0f, 0.0f, 1.0f };

      vertices[0].texPos = { 1.0f, 0.0f};
      vertices[1].texPos = { 0.0f, 1.0f};
      vertices[2].texPos = { 0.0f, 0.0f};


      indices = { 0,1,2 };
      
      texture = std::make_shared<Texture>(Texture(texturePath));
      GenerateNorm();
    }; //Triangle Ctor
  }; //Triangle

  struct Quad : Polytope {
    Quad(std::string texturePath) {
      edgeCount = 4;
      vertices.resize(4);

      vertices[0].pos = { -0.5f, -0.5f, 0.0f };
      vertices[1].pos = { 0.5f, -0.5f, 0.0f };
      vertices[2].pos = { 0.5f, 0.5f, 0.0f };
      vertices[3].pos = { -0.5f, 0.5f, 0.0f };

      vertices[0].color = { 1.0f, 0.0f, 0.0f };
      vertices[1].color = { 0.0f, 1.0f, 0.0f };
      vertices[2].color = { 0.0f, 0.0f, 1.0f };
      vertices[3].color = { 1.0f, 1.0f, 1.0f };

      vertices[0].texPos = { 0.0,1.0 };
      vertices[1].texPos = { 1.0,1.0 };
      vertices[2].texPos = { 1.0,0.0 };
      vertices[3].texPos = { 0.0,1.0 };

      indices = {
        0, 1, 2, 
        2, 3, 0
      }; //indices

      texture = std::make_shared<Texture>(Texture(texturePath));
      GenerateNorm();
    }; //Quad Ctor
  }; //Quad

  struct Hexagon : Polytope {
    Hexagon(std::string texturePath) {
      edgeCount = 6;
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

      vertices[0].texPos = { 0.0,1.0 };
      vertices[1].texPos = { 1.0,1.0 };
      vertices[2].texPos = { 1.0,0.0 };
      vertices[3].texPos = { 0.0,1.0 };
      vertices[4].texPos = { 0.0,1.0 };
      vertices[5].texPos = { 1.0,1.0 };
      vertices[6].texPos = { 1.0,0.0 };

      texture = std::make_shared<Texture>(Texture(texturePath));
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
  }; //Hexagon

  namespace PlatonicSolids {
  struct Tetrahedron : Polytope {
    Tetrahedron(std::string texturePath) {
      edgeCount = 6;
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

      texture = std::make_shared<Texture>(Texture(texturePath));
      GenerateNorm();
      ProjectSphereUV(norms);
    }; //Tetrahedon Ctor
  }; //Tetrahedron

  struct Cube : Polytope {
    Cube(std::string texturePath) {
      edgeCount = 12;
      vertices.resize(8);

      vertices[0].pos = { 0.f, 0.f, 1.2247f };
      vertices[1].pos = { 1.154701f, 0.f, 0.4082483f };
      vertices[2].pos = { -0.5773503f, 1.f, 0.4082483f };
      vertices[3].pos = { -0.5773503f, -1.f, 0.4082483f };
      vertices[4].pos = { 0.5773503,1.f,- 0.4082483f };
      vertices[5].pos = { 0.5773503f, - 1.f, - 0.4082483f };
      vertices[6].pos = { -1.154701, 0, - 0.4082483f };
      vertices[7].pos = { 0, 0, - 1.224745f };

      vertices[0].color = { 0.0f, 0.0f, 0.0f };
      vertices[1].color = { 1.0f, 0.0f, 0.0f };
      vertices[2].color = { 0.0f, 1.0f, 0.0f };
      vertices[3].color = { 0.0f, 0.0f, 1.0f };
      vertices[4].color = { 1.0f, 1.0f, 1.0f };
      vertices[5].color = { 0.0f, 0.5f, 0.5f };
      vertices[6].color = { 0.5f, 0.0f, 0.5f };
      vertices[7].color = { 0.5f, 0.5f, 0.0f };

      vertices[0].texPos = { 0.0,1.0 };
      vertices[1].texPos = { 1.0,1.0 };
      vertices[2].texPos = { 1.0,0.0 };
      vertices[3].texPos = { 0.0,1.0 };
      vertices[4].texPos = { 0.0,1.0 };
      vertices[5].texPos = { 1.0,1.0 };
      vertices[6].texPos = { 1.0,0.0 };
      vertices[7].texPos = { 0.0,1.0 };

      Shrink(2);

      indices = {
        0,1,4, 
        4,2,0,

        0,2,6, 
        6,3,0,

        0,3,5, 
        0,1,5,

        1,5,7, 
        1,4,7,

        2,4,7, 
        2,6,7,

        3,6,7, 
        3,5,7
      }; //indices

      texture = std::make_shared<Texture>(Texture(texturePath));
      GenerateNorm();
    }; //Cube Ctor
  }; //Cube

  struct Octahedron : Polytope {
    Octahedron(std::string texturePath) {
      edgeCount = 12;
      vertices.resize(6);

      vertices[0].pos = { 0, 0, 1.414214 };
      vertices[1].pos = { 1.414214, 0, 0 };
      vertices[2].pos = { 0, 1.414214, 0 };
      vertices[3].pos = { -1.414214, 0, 0 };
      vertices[4].pos = { 0, - 1.414214, 0 };
      vertices[5].pos = { 0, 0, - 1.414214 };
      
      vertices[0].color = { 0.0f, 0.0f, 0.0f };
      vertices[1].color = { 1.0f, 0.0f, 0.0f };
      vertices[2].color = { 0.0f, 1.0f, 0.0f };
      vertices[3].color = { 0.0f, 0.0f, 1.0f };
      vertices[4].color = { 1.0f, 1.0f, 1.0f };
      vertices[5].color = { 0.5f, 0.5f, 0.5f };


      indices = {
          0,1,2,
          0,2,3,
          0,3,4,
          0,4,1,
          1,4,5,
          1,5,2,
          2,5,3,
          3,5,4,
      }; //indices

      texture = std::make_shared<Texture>(Texture(texturePath));
      GenerateNorm();
      ProjectSphereUV(norms);
      Shrink(2);
    }; //Octahedron
    
  }; //Octahedron

  struct Icosahedron : Polytope {
    Icosahedron(std::string texturePath) {
      edgeCount = 30;
      vertices.resize(12);

      vertices[0].pos = { 0, 0, 1.175571 };
      vertices[1].pos = { 1.051462, 0, 0.5257311 };
      vertices[2].pos = { 0.3249197, 1, 0.5257311 };
      vertices[3].pos = { -0.8506508, 0.618034, 0.5257311 };
      vertices[4].pos = { -0.8506508, -0.618034, 0.5257311 };
      vertices[5].pos = { 0.3249197, -1, 0.5257311 };
      vertices[6].pos = { 0.8506508, 0.618034, -0.5257311 };
      vertices[7].pos = { 0.8506508, -0.618034, -0.5257311 };
      vertices[8].pos = { -0.3249197, 1, -0.5257311 };
      vertices[9].pos = { -1.051462, 0, -0.5257311 };
      vertices[10].pos = { -0.3249197, -1, -0.5257311 };
      vertices[11].pos = { 0, 0, -1.175571 };


      vertices[0].color = { 0.0f, 1.0f, 0.0f };
      vertices[1].color = { 0.0f, 1.0f, 1.0f };
      vertices[2].color = { 0.0f, 0.0f, 1.0f };
      vertices[3].color = { 1.0f, 0.0f, 1.0f };
      vertices[4].color = { 1.0f, 0.0f, 0.0f };
      vertices[5].color = { 1.0f, 1.0f, 0.0f };
      vertices[6].color = { 1.0f, 1.0f, 1.0f };

      vertices[7].color = { 0.0f, 0.0f, 0.0f };

      vertices[8].color = { 0.0f, 0.5f, 0.0f };
      vertices[9].color = { 0.0f, 0.5f, 0.5f };
      vertices[10].color = { 0.0f, 0.0f, 0.5f };
      vertices[11].color = { 0.5f, 0.0f, 0.5f };


      indices = {
        0,1,2,
        0,2,3,
        0,3,4,
        0,4,5,
        0,5,1,
        1,5,7,
        1,7,6,
        1,6,2,
        2,6,8,
        2,8,3,
        3,8,9,
        3,9,4,
        4,9,10,
        4,10,5,
        5,10,7,
        6,7,11,
        6,11,8,
        7,10,11,
        8,11,9,
        9,11,10,
      }; //indices
      
      texture = std::make_shared<Texture>(Texture(texturePath));
      Shrink(2);
      GenerateNorm();
      ProjectSphereUV(norms);
    }; //Icosahedron
  }; //Icosahedron

  struct Dodecahedron : Polytope {
    
    Dodecahedron(std::string texturePath) {
      edgeCount = 30;
      vertices.resize(20);

      vertices[0].pos = { 0, 0, 1.070466 };
      vertices[1].pos = { 0.7136442, 0, 0.7978784 };
      vertices[2].pos = { -0.3568221, 0.618034, 0.7978784 };
      vertices[3].pos = { -0.3568221, - 0.618034, 0.7978784 };
      vertices[4].pos = { 0.7978784, 0.618034, 0.3568221 };
      vertices[5].pos = { 0.7978784, - 0.618034, 0.3568221 };
      vertices[6].pos = { -0.9341724, 0.381966, 0.3568221 };
      vertices[7].pos = { 0.1362939, 1., 0.3568221 };
      vertices[8].pos = { 0.1362939, - 1., 0.3568221 };
      vertices[9].pos = { -0.9341724, - 0.381966, 0.3568221 };
      vertices[10].pos = { 0.9341724, 0.381966, - 0.3568221 };
      vertices[11].pos = { 0.9341724, - 0.381966, - 0.3568221 };
      vertices[12].pos = { -0.7978784, 0.618034, - 0.3568221 };
      vertices[13].pos = { -0.1362939, 1., - 0.3568221 };
      vertices[14].pos = { -0.1362939, - 1., - 0.3568221 };
      vertices[15].pos = { -0.7978784, - 0.618034, - 0.3568221 };
      vertices[16].pos = { 0.3568221, 0.618034, - 0.7978784 };
      vertices[17].pos = { 0.3568221, - 0.618034, - 0.7978784 };
      vertices[18].pos = { -0.7136442, 0, - 0.7978784 };
      vertices[19].pos = { 0, 0, - 1.070466 };


      vertices[0].color = { 0.0f, 1.0f, 0.0f };
      vertices[1].color = { 0.0f, 1.0f, 1.0f };
      vertices[2].color = { 0.0f, 0.0f, 1.0f };
      vertices[3].color = { 1.0f, 0.0f, 1.0f };
      vertices[4].color = { 1.0f, 0.0f, 0.0f };
      vertices[5].color = { 1.0f, 1.0f, 0.0f };
      vertices[6].color = { 1.0f, 1.0f, 1.0f };

      vertices[7].color = { 0.0f, 0.0f, 0.0f };

      vertices[8].color = { 0.0f, 0.5f, 0.0f };
      vertices[9].color = { 0.0f, 0.5f, 0.5f };
      vertices[10].color = { 0.0f, 0.0f, 0.5f };
      vertices[11].color = { 0.5f, 0.0f, 0.5f };
      vertices[12].color = { 0.5f, 0.0f, 0.0f };
      vertices[13].color = { 0.5f, 0.5f, 0.0f };
      vertices[14].color = { 0.5f, 0.5f, 0.5f };

      vertices[15].color = { 0.0f, 0.25f, 0.0f };
      vertices[16].color = { 0.0f, 0.25f, 0.25f };
      vertices[17].color = { 0.0f, 0.0f, 0.25f };
      vertices[18].color = { 0.25f, 0.0f, 0.25f };
      vertices[19].color = { 0.25f, 0.0f, 0.0f };

      indices = {
        0,1,4, //7,2
        2,7,4,
        0,2,4,

        0,2,6,
        3,9,6,
        0,3,6,

        0,3,8,
        1,5,8,
        0,1,8,

        1,5,11,
        4,10,11,
        1,4,11,

        2,7,13,
        6,12,13,
        2,6,13,

        3,9,15,
        8,14,15,
        3,8,15,

        4,10,16,
        7,13,16,
        4,7,16,

        5,8,14,
        11,17,14,
        5,11,14,

        6,12,18,
        9,15,18,
        6,9,18,

        10,11,17,
        16,19,17,
        10,16,17,

        12,13,16,
        18,19,16,
        12,18,16,

        14,15,18,
        17,19,18,
        14,17,18,
      }; //indices

      Shrink(2);
      GenerateNorm();
      texture = std::make_shared<Texture>(Texture(texturePath));
      ProjectSphereUV(norms);
    }; //Dodecahedron

  }; //Dodecahedron
  }; //PlatonicSolids

  namespace ArchemedianSolids {
    struct Cuboctahedron : Polytope {
      Cuboctahedron(std::string texturePath) {
        edgeCount = 24;
        vertices.resize(12);

        vertices[0].pos = { 0, 0, 1.154701 };
        vertices[1].pos = { 1., 0, 0.5773503 };
        vertices[2].pos = { 0.3333333, 0.942809, 0.5773503 };
        vertices[3].pos = { -1., 0, 0.5773503 };
        vertices[4].pos = { -0.3333333, - 0.942809, 0.5773503 };
        vertices[5].pos = { 1., 0, - 0.5773503, };
        vertices[6].pos = { 0.6666667, - 0.942809, 0 };
        vertices[7].pos = { -0.6666667, 0.942809, 0 };
        vertices[8].pos = { 0.3333333, 0.942809, - 0.5773503 };
        vertices[9].pos = { -1., 0, - 0.5773503 };
        vertices[10].pos = { -0.3333333, - 0.942809, - 0.5773503 };
        vertices[11].pos = { 0, 0, - 1.154701 };

        vertices[0].color = { 0.0f, 1.0f, 0.0f };
        vertices[1].color = { 0.0f, 1.0f, 1.0f };
        vertices[2].color = { 0.0f, 0.0f, 1.0f };
        vertices[3].color = { 1.0f, 0.0f, 1.0f };
        vertices[4].color = { 1.0f, 0.0f, 0.0f };
        vertices[5].color = { 1.0f, 1.0f, 0.0f };
        vertices[6].color = { 1.0f, 1.0f, 1.0f };

        vertices[7].color = { 0.0f, 0.0f, 0.0f };

        vertices[8].color = { 0.0f, 0.5f, 0.0f };
        vertices[9].color = { 0.0f, 0.5f, 0.5f };
        vertices[10].color = { 0.0f, 0.0f, 0.5f };
        vertices[11].color = { 0.5f, 0.0f, 0.5f };

        indices = {
            //Triangles
            0,1,2,
            0,3,4,
            1,6,5,
            2,8,7,
            3,7,9,
            4,10,6,
            5,11,8,
            9,11,10,

            //Squares
            0,2,7,
            0,3,7,

            0,4,6,
            0,1,6,

            1,5,8,
            1,2,8,

            3,9,10,
            3,4,10,

            5,6,10,
            5,11,10,

            7,8,11,
            7,9,11,
        }; //indices
      texture = std::make_shared<Texture>(Texture(texturePath));
      Shrink(2);
      GenerateNorm();
      ProjectSphereUV(norms);
      }; //Cuboctahedron
      
    }; //CubOctahedron

    struct Icosidodecahedron : Polytope {
      Icosidodecahedron(std::string texturePath) {
        edgeCount = 60;
        
        vertices.resize(30);

        vertices[0].pos = { 0, 0, 1.051462 };
          vertices[1].pos = { 0.618034, 0, 0.8506508 };
          vertices[2].pos = { 0.2763932, 0.5527864, 0.8506508 };
          vertices[3].pos = { -0.618034, 0, 0.8506508 };
          vertices[4].pos = { -0.2763932, -0.5527864, 0.8506508 };
          vertices[5].pos = { 1., 0, 0.3249197 };
          vertices[6].pos = { 0.7236068, -0.5527864, 0.5257311 };
          vertices[7].pos = { -0.1708204, 0.8944272, 0.5257311 };
          vertices[8].pos = { 0.4472136, 0.8944272, 0.3249197 };
          vertices[9].pos = { -1., 0, 0.3249197 };
          vertices[10].pos = { -0.7236068, 0.5527864, 0.5257311 };
          vertices[11].pos = { 0.1708204, - 0.8944272, 0.5257311 };
          vertices[12].pos = { -0.4472136, - 0.8944272, 0.3249197 };
          vertices[13].pos = { 1., 0, - 0.3249197 };
          vertices[14].pos = { 0.8944272, 0.5527864, 0 };
          vertices[15].pos = {0.5527864, - 0.8944272, 0};
          vertices[16].pos = {-0.5527864, 0.8944272, 0};
          vertices[17].pos = {0.4472136, 0.8944272, - 0.3249197};
          vertices[18].pos = { -1., 0, - 0.3249197 };
          vertices[19].pos = { -0.8944272, - 0.5527864, 0 };
          vertices[20].pos = {-0.4472136, - 0.8944272, - 0.3249197};
          vertices[21].pos = {0.618034, 0, - 0.8506508};
          vertices[22].pos = {0.7236068, - 0.5527864, - 0.5257311};
          vertices[23].pos = {0.1708204, - 0.8944272, - 0.5257311};
          vertices[24].pos = {-0.7236068, 0.5527864, - 0.5257311};
          vertices[25].pos = {-0.1708204, 0.8944272, - 0.5257311};
          vertices[26].pos = {0.2763932, 0.5527864, - 0.8506508};
          vertices[27].pos = {-0.618034, 0, - 0.8506508};
          vertices[28].pos = {-0.2763932, - 0.5527864, - 0.8506508};
          vertices[29].pos = {0, 0, - 1.051462};


          vertices[0].color = { 0.0f, 1.0f, 0.0f };
          vertices[1].color = { 0.0f, 1.0f, 1.0f };
          vertices[2].color = { 0.0f, 0.0f, 1.0f };
          vertices[3].color = { 1.0f, 0.0f, 1.0f };
          vertices[4].color = { 1.0f, 0.0f, 0.0f };
          vertices[5].color = { 1.0f, 1.0f, 0.0f };
          vertices[6].color = { 1.0f, 1.0f, 1.0f };

          vertices[7].color = { 0.0f, 0.0f, 0.0f };

          vertices[8].color = { 0.0f, 0.5f, 0.0f };
          vertices[9].color = { 0.0f, 0.5f, 0.5f };
          vertices[10].color = { 0.0f, 0.0f, 0.5f };
          vertices[11].color = { 0.5f, 0.0f, 0.5f };
          vertices[12].color = { 0.5f, 0.0f, 0.5f };
          vertices[13].color = { 0.5f, 0.5f, 0.0f };
          vertices[14].color = { 0.5f, 0.5f, 0.5f };

          vertices[15].color = { 0.0f, 0.25f, 0.0f };
          vertices[16].color = { 0.0f, 0.25f, 0.25f };
          vertices[17].color = { 0.0f, 0.0f, 0.25f };
          vertices[18].color = { 0.25f, 0.0f, 0.25f };
          vertices[19].color = { 0.25f, 0.0f, 0.25f };
          vertices[20].color = { 0.25f, 0.25f, 0.0f };
          vertices[21].color = { 0.25f, 0.25f, 0.25f };

          vertices[22].color = { 0.0f, 0.75f, 0.0f };
          vertices[23].color = { 0.0f, 0.75f, 0.75f };
          vertices[24].color = { 0.0f, 0.0f, 0.75f };
          vertices[25].color = { 0.75f, 0.0f, 0.75f };
          vertices[26].color = { 0.75f, 0.0f, 0.75f };
          vertices[28].color = { 0.75f, 0.75f, 0.0f };
          vertices[29].color = { 0.75f, 0.75f, 0.75f };

          indices = {
            //Triangles
            0,1,2,
            0,3,4,
            1,6,5,
            2,8,7,
            3,10,9,
            4,12,11,
            5,13,14,
            6,11,15,
            7,16,10,
            8,14,17,
            9,18,19,
            12,19,20,
            13,22,21,
            15,23,22,
            16,25,24,
            17,26,25,
            18,24,27,
            20,28,23,
            21,29,26,
            27,29,28,

            //Dodecahedron
            0,2,7,
            3,10,7,
            0,3,7,

            0,4,11,
            1,6,11,
            0,1,11,

            1,5,14,
            2,8,14,
            1,2,14,

            3,9,19,
            4,12,19,
            3,4,19,

            5,6,15,
            13,22,15,
            5,13,15,

            7,8,17,
            16,25,17,
            7,16,17,

            9,10,16,
            18,24,16,
            9,18,16,

            11,12,20,
            15,23,20,
            11,15,20,

            13,21,26,
            17,14,26,
            13,17,26,

            18,27,28,
            19,20,28,
            18,19,28,

            21,22,23,
            29,28,23,
            21,29,23,

            24,25,26,
            27,29,26,
            24,27,26
          }; //indices

          texture = std::make_shared<Texture>(Texture(texturePath));
          Shrink(2);
          GenerateNorm();
          ProjectSphereUV(norms);
      }; //Icosidodecahedron
    }; //Icosidodecahedron


    struct TruncatedTetrahedron : Polytope {
      TruncatedTetrahedron(std::string texturePath) {
        edgeCount = 18;

        vertices.resize(12);
        vertices[0].pos = { 0, 0, 1.105542 };
        vertices[1].pos = { 0.8528029, 0, 0.7035265 };
        vertices[2].pos = { -0.7106691, 0.4714045, 0.7035265 };
        vertices[3].pos = {0.3316456, - 0.7856742, 0.7035265};
        vertices[4].pos = {0.9949367, 0.4714045, - 0.1005038};
        vertices[5].pos = {-1.089693, 0.1571348, - 0.1005038};
        vertices[6].pos = {-0.5685352, 0.942809, - 0.1005038};
        vertices[7].pos = {-0.04737794, - 1.099944, - 0.1005038};
        vertices[8].pos = {0.6159132, 0.1571348, - 0.904534};
        vertices[9].pos = {0.2842676, 0.942809, - 0.5025189};
        vertices[10].pos = {-0.758047, - 0.6285394, - 0.5025189};
        vertices[11].pos = {0.09475587, - 0.6285394, - 0.904534};

        vertices[0].color = { 0.0f, 1.0f, 0.0f };
        vertices[1].color = { 0.0f, 1.0f, 1.0f };
        vertices[2].color = { 0.0f, 0.0f, 1.0f };
        vertices[3].color = { 1.0f, 0.0f, 1.0f };
        vertices[4].color = { 1.0f, 0.0f, 0.0f };
        vertices[5].color = { 1.0f, 1.0f, 0.0f };
        vertices[6].color = { 1.0f, 1.0f, 1.0f };

        vertices[7].color = { 0.0f, 0.0f, 0.0f };

        vertices[8].color = { 0.0f, 0.5f, 0.0f };
        vertices[9].color = { 0.0f, 0.5f, 0.5f };
        vertices[10].color = { 0.0f, 0.0f, 0.5f };
        vertices[11].color = { 0.5f, 0.0f, 0.5f };

        indices = {
          //Triangles
          0,3,1,
          2,6,5,
          4,8,9,
          7,10,11,

          //Hexagons
          0,1,4,
          0,2,4,
          9,2,4,
          9,6,2,

          0,2,5,
          0,3,5,
          10,3,5,
          10,7,3,

          1,3,7,
          1,4,7,
          11,4,7,
          11,8,4,

          5,6,9,
          5,10,9,
          8,10,9,
          8,11,10,
        }; //indices

        texture = std::make_shared<Texture>(Texture(texturePath));
        Shrink(2);
        GenerateNorm();
        ProjectSphereUV(norms);
      }; //TruncatedTetrahedron 
    }; //TruncatedTetrahedron 

    struct TruncatedCube : Polytope {
      TruncatedCube(std::string texturePath) {
        edgeCount = 36;
        vertices.resize(24);

        vertices[0].pos = { 0,0,1.042011 };
        vertices[1].pos = { 0.5621693, 0, 0.8773552 };
        vertices[2].pos = {-0.4798415, 0.2928932, 0.8773552};
        vertices[3].pos = {0.2569714, - 0.5, 0.8773552};
        vertices[4].pos = {0.8773552, 0.2928932, 0.4798415};
        vertices[5].pos = {-0.9014684, 0.2071068, 0.4798415};
        vertices[6].pos = {-0.5962706, 0.7071068, 0.4798415};
        vertices[7].pos = {0.1405423, - 0.9142136, 0.4798415};
        vertices[8].pos = {1.017898, 0.2071068, - 0.08232778};
        vertices[9].pos = {0.7609261, 0.7071068, 0.08232778};
        vertices[10].pos = {-1.017898, - 0.2071068, 0.08232778};
        vertices[11].pos = {-0.2810846, 1., 0.08232778};
        vertices[12].pos = {-0.2810846, - 1., 0.08232778};
        vertices[13].pos = {0.2810846, - 1., - 0.08232778};
        vertices[14].pos = {0.9014684, - 0.2071068, - 0.4798415};
        vertices[15].pos = {0.2810846, 1., - 0.08232778};
        vertices[16].pos = {-0.7609261, - 0.7071068, - 0.08232778};
        vertices[17].pos = {-0.8773552, - 0.2928932, - 0.4798415};
        vertices[18].pos = {-0.1405423, 0.9142136, - 0.4798415};
        vertices[19].pos = {0.5962706, - 0.7071068, - 0.4798415};
        vertices[20].pos = {0.4798415, - 0.2928932, - 0.8773552};
        vertices[21].pos = {-0.5621693, 0, - 0.8773552};
        vertices[22].pos = {-0.2569714, 0.5, - 0.8773552};
        vertices[23].pos = {0, 0, - 1.042011};


        vertices[0].color = { 0.0f, 1.0f, 0.0f };
        vertices[1].color = { 0.0f, 1.0f, 1.0f };
        vertices[2].color = { 0.0f, 0.0f, 1.0f };
        vertices[3].color = { 1.0f, 0.0f, 1.0f };
        vertices[4].color = { 1.0f, 0.0f, 0.0f };
        vertices[5].color = { 1.0f, 1.0f, 0.0f };
        vertices[6].color = { 1.0f, 1.0f, 1.0f };

        vertices[7].color = { 0.0f, 0.0f, 0.0f };

        vertices[8].color = { 0.0f, 0.5f, 0.0f };
        vertices[9].color = { 0.0f, 0.5f, 0.5f };
        vertices[10].color = { 0.0f, 0.0f, 0.5f };
        vertices[11].color = { 0.5f, 0.0f, 0.5f };
        vertices[12].color = { 0.5f, 0.0f, 0.5f };
        vertices[13].color = { 0.5f, 0.5f, 0.0f };
        vertices[14].color = { 0.5f, 0.5f, 0.5f };

        vertices[15].color = { 0.0f, 0.25f, 0.0f };
        vertices[16].color = { 0.0f, 0.25f, 0.25f };
        vertices[17].color = { 0.0f, 0.0f, 0.25f };
        vertices[18].color = { 0.25f, 0.0f, 0.25f };
        vertices[19].color = { 0.25f, 0.0f, 0.25f };
        vertices[20].color = { 0.25f, 0.25f, 0.0f };
        vertices[21].color = { 0.25f, 0.25f, 0.25f };

        vertices[22].color = { 0.0f, 0.75f, 0.0f };
        vertices[23].color = { 0.0f, 0.75f, 0.75f };


        indices = {
          //Triangles
          0,3,1,
          2,6,5,
          4,8,9,
          7,12,13,
          10,17,16,
          11,15,18,
          14,19,20,
          21,22,23,

          //Come Back to
          0,1,4,9,15,11,6,2,
          0,2,5,10,16,12,7,3,
          1,3,7,13,19,14,8,4,
          5,6,11,18,22,21,17,10,
          8,14,20,23,22,18,15,9,
          12,16,17,21,23,20,19,13,
        }; //indices
      }; //TruncatedCube
    }; //TruncatedCube


    struct TruncatedOctahedron : Polytope {
      TruncatedOctahedron(std::string texturePath) {
        edgeCount = 36;
        vertices.resize(24);

        vertices[0].pos = { 0,0,1.054093 };
        vertices[1].pos = { 0.6324555,0, 0.843274 };
        vertices[2].pos = {-0.421637, 0.4714045, 0.843274};
        vertices[3].pos = {-0.07027284, - 0.6285394, 0.843274};
        vertices[4].pos = {0.843274, 0.4714045, 0.421637};
        vertices[5].pos = {0.5621827, - 0.6285394, 0.6324555};
        vertices[6].pos = {-0.9135469, 0.3142697, 0.421637};
        vertices[7].pos = {-0.2108185, 0.942809, 0.421637};
        vertices[8].pos = {-0.5621827, - 0.7856742, 0.421637};
        vertices[9].pos = {0.9838197, 0.3142697, - 0.2108185};
        vertices[10].pos = {0.421637, 0.942809, 0.2108185};
        vertices[11].pos = {0.7027284, - 0.7856742, 0};
        vertices[12].pos = {-0.7027284, 0.7856742, 0};
        vertices[13].pos = {-0.9838197, - 0.3142697, 0.2108185};
        vertices[14].pos = {-0.421637, - 0.942809, - 0.2108185};
        vertices[15].pos = {0.5621827, 0.7856742, - 0.421637};
        vertices[16].pos = {0.9135469, - 0.3142697, - 0.421637};
        vertices[17].pos = {0.2108185, - 0.942809, - 0.421637};
        vertices[18].pos = {-0.5621827, 0.6285394, - 0.6324555};
        vertices[19].pos = {-0.843274, - 0.4714045, - 0.421637};
        vertices[20].pos = {0.07027284, 0.6285394, - 0.843274};
        vertices[21].pos = {0.421637, - 0.4714045, - 0.843274};
        vertices[22].pos = {-0.6324555, 0, - 0.843274};
        vertices[23].pos = {0, 0, - 1.054093};


        vertices[0].color = { 0.0f, 1.0f, 0.0f };
        vertices[1].color = { 0.0f, 1.0f, 1.0f };
        vertices[2].color = { 0.0f, 0.0f, 1.0f };
        vertices[3].color = { 1.0f, 0.0f, 1.0f };
        vertices[4].color = { 1.0f, 0.0f, 0.0f };
        vertices[5].color = { 1.0f, 1.0f, 0.0f };
        vertices[6].color = { 1.0f, 1.0f, 1.0f };

        vertices[7].color = { 0.0f, 0.0f, 0.0f };

        vertices[8].color = { 0.0f, 0.5f, 0.0f };
        vertices[9].color = { 0.0f, 0.5f, 0.5f };
        vertices[10].color = { 0.0f, 0.0f, 0.5f };
        vertices[11].color = { 0.5f, 0.0f, 0.5f };
        vertices[12].color = { 0.5f, 0.0f, 0.5f };
        vertices[13].color = { 0.5f, 0.5f, 0.0f };
        vertices[14].color = { 0.5f, 0.5f, 0.5f };

        vertices[15].color = { 0.0f, 0.25f, 0.0f };
        vertices[16].color = { 0.0f, 0.25f, 0.25f };
        vertices[17].color = { 0.0f, 0.0f, 0.25f };
        vertices[18].color = { 0.25f, 0.0f, 0.25f };
        vertices[19].color = { 0.25f, 0.0f, 0.25f };
        vertices[20].color = { 0.25f, 0.25f, 0.0f };
        vertices[21].color = { 0.25f, 0.25f, 0.25f };

        vertices[22].color = { 0.0f, 0.75f, 0.0f };
        vertices[23].color = { 0.0f, 0.75f, 0.75f };


        indices = {
          //Squares
          0,3,5,
          0,1,5,

          2,7,12,
          2,6,12,
          
          4,9,15,
          4,10,15,
          
          8,13,19,
          8,14,19,
          
          11,17,21,
          11,16,21,
          
          18,20,23,
          18,22,23,

          //Hexagon
          0,1,4,
          0,2,4,
          10,2,4,
          10,7,2,
          
          0,2,6,
          0,3,6,
          13,3,6,
          13,8,3,
          
          1,5,11,
          1,4,11,
          16,4,11,
          16,9,4,

          3,8,14,
          3,5,14,
          17,5,14,
          17,11,5,
          
          6,12,18,
          6,13,18,
          22,13,18,
          22,19,13,
          
          7,10,15,
          7,12,15,
          20,12,15,
          20,18,12,
          
          9,16,21,
          9,15,21,
          23,15,21,
          23,20,15,
          
          14,19,22,
          14,17,22,
          23,17,22,
          23,21,17
        }; //indices

        texture = std::make_shared<Texture>(Texture(texturePath));
        Shrink(2);
        GenerateNorm();
        ProjectSphereUV(norms);
      }; //TruncatedOctahedron


      struct TruncatedIcosahedron : Polytope {
        TruncatedIcosahedron(std::string texturePath) {
          edgeCount = 36;
          vertices.resize(24);

          vertices[0].pos = { 0,0,1.054093 };
          vertices[1].pos = { 0.6324555,0, 0.843274 };
          vertices[2].pos = { -0.421637, 0.4714045, 0.843274 };
          vertices[3].pos = { -0.07027284, -0.6285394, 0.843274 };
          vertices[4].pos = { 0.843274, 0.4714045, 0.421637 };
          vertices[5].pos = { 0.5621827, -0.6285394, 0.6324555 };
          vertices[6].pos = { -0.9135469, 0.3142697, 0.421637 };
          vertices[7].pos = { -0.2108185, 0.942809, 0.421637 };
          vertices[8].pos = { -0.5621827, -0.7856742, 0.421637 };
          vertices[9].pos = { 0.9838197, 0.3142697, -0.2108185 };
          vertices[10].pos = { 0.421637, 0.942809, 0.2108185 };
          vertices[11].pos = { 0.7027284, -0.7856742, 0 };
          vertices[12].pos = { -0.7027284, 0.7856742, 0 };
          vertices[13].pos = { -0.9838197, -0.3142697, 0.2108185 };
          vertices[14].pos = { -0.421637, -0.942809, -0.2108185 };
          vertices[15].pos = { 0.5621827, 0.7856742, -0.421637 };
          vertices[16].pos = { 0.9135469, -0.3142697, -0.421637 };
          vertices[17].pos = { 0.2108185, -0.942809, -0.421637 };
          vertices[18].pos = { -0.5621827, 0.6285394, -0.6324555 };
          vertices[19].pos = { -0.843274, -0.4714045, -0.421637 };
          vertices[20].pos = { 0.07027284, 0.6285394, -0.843274 };
          vertices[21].pos = { 0.421637, -0.4714045, -0.843274 };
          vertices[22].pos = { -0.6324555, 0, -0.843274 };
          vertices[23].pos = { 0, 0, -1.054093 };

          vertices[0].color = { 0.0f, 1.0f, 0.0f };
          vertices[1].color = { 0.0f, 1.0f, 1.0f };
          vertices[2].color = { 0.0f, 0.0f, 1.0f };
          vertices[3].color = { 1.0f, 0.0f, 1.0f };
          vertices[4].color = { 1.0f, 0.0f, 0.0f };
          vertices[5].color = { 1.0f, 1.0f, 0.0f };
          vertices[6].color = { 1.0f, 1.0f, 1.0f };

          vertices[7].color = { 0.0f, 0.0f, 0.0f };

          vertices[8].color = { 0.0f, 0.5f, 0.0f };
          vertices[9].color = { 0.0f, 0.5f, 0.5f };
          vertices[10].color = { 0.0f, 0.0f, 0.5f };
          vertices[11].color = { 0.5f, 0.0f, 0.5f };
          vertices[12].color = { 0.5f, 0.0f, 0.5f };
          vertices[13].color = { 0.5f, 0.5f, 0.0f };
          vertices[14].color = { 0.5f, 0.5f, 0.5f };

          vertices[15].color = { 0.0f, 0.25f, 0.0f };
          vertices[16].color = { 0.0f, 0.25f, 0.25f };
          vertices[17].color = { 0.0f, 0.0f, 0.25f };
          vertices[18].color = { 0.25f, 0.0f, 0.25f };
          vertices[19].color = { 0.25f, 0.0f, 0.25f };
          vertices[20].color = { 0.25f, 0.25f, 0.0f };
          vertices[21].color = { 0.25f, 0.25f, 0.25f };

          vertices[22].color = { 0.0f, 0.75f, 0.0f };
          vertices[23].color = { 0.0f, 0.75f, 0.75f };

          //1,2,3,
          //1,6,3,
          //4,6,3,
          //4,5,6

          indices = {
            //Pentagons
            0,3,8,
            1,5,8,
            0,1,8,

            2,7,15,
            6,13,15,
            2,6,15,
            
            4,10,18,
            11,20,18,
            4,11,18,
            
            9,14,23,
            17,27,23,
            9,17,23,
            
            12,21,31,
            19,29,31,
            12,19,31,
            
            16,26,36,
            25,35,36,
            16,25,36,
            
            22,32,42,
            33,43,42,
            22,33,42,
            
            24,30,40,
            34,44,40,
            24,34,40,
            
            28,39,49,
            38,48,49,
            28,38,49,
            
            37,47,55,
            46,54,55,
            37,46,55,
            
            41,45,53,
            50,57,53,
            41,50,53,
            
            51,52,56,
            58,59,56,
            51,58,56,

            //1,2,3,
            //1,6,3,
            //4,6,3,
            //4,5,6

            //Hexagon

          }; //indices
        }; //TruncatedOcosahedron
      }; //TruncatedOctahedron
    }; //TruncatedOctahedron



  }; //AchimediesSolids
}; //Polytopes

