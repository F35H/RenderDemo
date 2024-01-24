#pragma once

#include "vulkan/Objects/Syncronization/CPUSyncFactory.h"
#include "Vulkan/Objects/ModelTypes/Polyhedra.h"
#include <UPCLASS.h>

namespace Polytopes {

  enum Mesh {
    Triangle = 0,
    Quad,
    Hexagon,

    Tetrahedron,
    Cube,
    Octahedron,
    Icosahedron,
    Dodecahedron,

    Cuboctahedron,
    Icosidodecahedron,
    TrucatedTetrahedron
    
  }; //Polyhedra

  struct PolytopeFactory {
    PolytopeFactory() = default;

    std::shared_ptr<Polytope> GetPolyhedra(Mesh mesh, std::string texture = "textures//Alias3//fabrics_papers//fabric_12_1.png") {
      switch (mesh) {
      case Triangle:            return std::make_shared<Polyhedra::Triangle>(                                 Polyhedra::Triangle(texture));
      case Quad:                return std::make_shared<Polyhedra::Quad>(                                     Polyhedra::Quad(texture));
      case Hexagon:             return std::make_shared<Polyhedra::Hexagon>(                                  Polyhedra::Hexagon(texture));
      case Tetrahedron:         return std::make_shared<Polyhedra::PlatonicSolids::Tetrahedron>(              Polyhedra::PlatonicSolids::Tetrahedron(texture));
      case Cube:                return std::make_shared<Polyhedra::PlatonicSolids::Cube>(                     Polyhedra::PlatonicSolids::Cube(texture));
      case Octahedron:          return std::make_shared<Polyhedra::PlatonicSolids::Octahedron>(               Polyhedra::PlatonicSolids::Octahedron(texture));
      case Icosahedron:         return std::make_shared<Polyhedra::PlatonicSolids::Icosahedron>(              Polyhedra::PlatonicSolids::Icosahedron(texture));
      case Dodecahedron:        return std::make_shared<Polyhedra::PlatonicSolids::Dodecahedron>(             Polyhedra::PlatonicSolids::Dodecahedron(texture));
      case Cuboctahedron:       return std::make_shared<Polyhedra::ArchemedianSolids::Cuboctahedron>(         Polyhedra::ArchemedianSolids::Cuboctahedron(texture));
      case Icosidodecahedron:   return std::make_shared<Polyhedra::ArchemedianSolids::Icosidodecahedron>(     Polyhedra::ArchemedianSolids::Icosidodecahedron(texture));
      case TrucatedTetrahedron: return std::make_shared<Polyhedra::ArchemedianSolids::TruncatedTetrahedron>(  Polyhedra::ArchemedianSolids::TruncatedTetrahedron(texture));
      }; //switch mesh
    }; //GetPolyhedra

    std::vector<Polytope> GetPolyhedra(std::string texture = "textures//Alias3//fabrics_papers//fabric_12_1.png") {
      return {
        Polyhedra::Triangle(texture),
        Polyhedra::Quad(texture),
        Polyhedra::Hexagon(texture),

        Polyhedra::PlatonicSolids::Tetrahedron(texture),
        Polyhedra::PlatonicSolids::Cube(texture),
        Polyhedra::PlatonicSolids::Octahedron(texture),
        Polyhedra::PlatonicSolids::Icosahedron(texture),
        Polyhedra::PlatonicSolids::Dodecahedron(texture),

        Polyhedra::ArchemedianSolids::Cuboctahedron(texture),
        Polyhedra::ArchemedianSolids::Icosidodecahedron(texture),
        Polyhedra::ArchemedianSolids::TruncatedTetrahedron(texture)
      };//polytopes;
    }; //GetModel

    std::vector<Polytope> GetFileModels(std::string dirFile) {
      folder = dirFile;
      for (auto dir : std::filesystem::directory_iterator("models//" + dirFile)) {
        if (!CheckExtension(dir.path().extension().string())) continue;
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
          dir.path().string(),
          /*aiProcess_JoinIdenticalVertices   |*/
          aiProcess_Triangulate |
          /*aiProcess_RemoveComponent       | Look into how this one works*/
          aiProcess_PreTransformVertices |
          //aiProcess_RemoveRedundantMaterials|
          //aiProcess_FindDegenerates         |
          //aiProcess_ValidateDataStructure   |
          //aiProcess_FindInvalidData         |
          //aiProcess_FindInstances           |
          /*aiProcess_OptimizeMeshes          |
          aiProcess_OptimizeGraph           |*/
          aiProcess_SortByPType
        ); //aiScene

        Polytope poly;

        for (uint_fast32_t j = scene->mNumMeshes - 1; j < scene->mNumMeshes; --j) {
          for (uint_fast32_t i = scene->mMeshes[j]->mNumFaces - 1; i < scene->mMeshes[j]->mNumFaces; --i) {
            if (scene->mMeshes[j]->mFaces[i].mNumIndices == 3) {
              poly.indices.emplace_back(scene->mMeshes[j]->mFaces[i].mIndices[0]);
              poly.indices.emplace_back(scene->mMeshes[j]->mFaces[i].mIndices[1]);
              poly.indices.emplace_back(scene->mMeshes[j]->mFaces[i].mIndices[2]);
            }
          }; //forLoop

          for (size_t i = scene->mMeshes[j]->mNumVertices - 1; i <= scene->mMeshes[j]->mNumVertices; --i) {
            auto vertice = Vertex();
            vertice.pos = {
              scene->mMeshes[j]->mVertices[i].x,
              (-scene->mMeshes[j]->mVertices[i].y),
              scene->mMeshes[j]->mVertices[i].z,
            }; //vertices

            if (scene->mMeshes[j]->HasVertexColors(i) && !scene->mMeshes[j]->mColors[i]->IsBlack()) {
              vertice.color = {
                scene->mMeshes[j]->mColors[i]->r,
                scene->mMeshes[j]->mColors[i]->g,
                scene->mMeshes[j]->mColors[i]->b,
                scene->mMeshes[j]->mColors[i]->a
              };
            }
            else {
              vertice.color = { 1.0,1.0,1.0,1.0 };
            }

            if (scene->mMeshes[j]->HasNormals()) {
              scene->mMeshes[j]->mNormals[i].Normalize();

              vertice.vertNorm = {
                scene->mMeshes[j]->mNormals[i].x,
                scene->mMeshes[j]->mNormals[i].y,
                scene->mMeshes[j]->mNormals[i].z,
              }; //vertices


              if (folder == "Stanford//bunny") vertice.pos *= 10;
              if (folder == "Planes") vertice.pos /= 10;

            }; //if not nullptr

            if (scene->mMeshes[j]->HasTextureCoords(i)) {
              vertice.texPos = {
                scene->mMeshes[j]->mTextureCoords[i]->x,
                scene->mMeshes[j]->mTextureCoords[i]->y
              }; //vertices
            }; //HasTextureCoords

            poly.vertices.emplace_back(vertice);
          }; //forLoop

          if (!scene->mMeshes[j]->HasNormals()) poly.GenerateNorm();

          if (scene->HasTextures()) {
            poly.texture = std::make_shared<Texture>(Texture(scene->mTextures[j]->mFilename.C_Str()));
          } //scene-HasTextures
          else {
            poly.texture = std::make_shared<Texture>(Texture("textures//Alias3//construction_materials//hex_256_1.png"));
            poly.ProjectCylanderUV(poly.norms);
          }; //Has textures

          std::cout << "Loaded Model: " << j << "\n"
            << " Scene: " << dir.path().string() << "\n"
            << " Tris: " << scene->mMeshes[j]->mNumFaces << "\n"
            << " Verts: " << scene->mMeshes[j]->mNumVertices << "\n";
          models.emplace_back(poly);
        }; //MuticePools

        //auto scene = UPCLASS::upClass(
        //  dir.path().stem().string(),
        //  dir.path().extension().string());

        //for (auto& asset : scene.upAssets) {
        //  Polytope poly;
        //  poly.indices = asset->indices;

        //  uint_fast32_t vCount = asset->vertexCount;
        //  poly.vertices.resize(asset->vertexCount);
        //  for (uint_fast32_t i = vCount; i < asset->vertexCount; ++i) {
        //    poly.vertices[i].pos.r = asset->vertices[i].x;
        //    poly.vertices[i].pos.g = asset->vertices[i].y;
        //    poly.vertices[i].pos.b = asset->vertices[i].z;
        //  }; //for assetIndices

        //  models.push_back(&poly);
        //}; //upAssets
        return models;
      }; //for every File
    }; //GetFileModels

  private:
    std::vector<Polytope> models;
    std::filesystem::directory_iterator dir;
    std::string* mutexIndice = new std::string();
    std::string* mIndex = new std::string();
    std::string folder = std::string();

    bool CheckExtension(std::string str) {
      if (str == ".bin") return false;
      if (str == ".jpg") return false;
      if (str == ".png") return false;
      return true;
    }; //CheckExtension

  }; //ModelFactory

}; //Polytopes

