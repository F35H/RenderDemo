#pragma once

#include "vulkan/Objects/Syncronization/CPUSyncFactory.h"
#include "Vulkan/Objects/ModelTypes/Polyhedra.h"

namespace Polytopes {

  struct PolytopeFactory {
    PolytopeFactory() = default;

    std::vector<Polytope> GetPolyhedra(std::string texture) {
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
      dir = std::filesystem::directory_iterator("models//" + dirFile);
      folder = dirFile;
      //std::string id = ThreadPool::AddWork([this] { LoadScenes(); });
      LoadScenes();
      //ThreadPool::FinishWork({ id });
      //MuticePool::Unlock();
      return models;
    }; //GetPolyhedra

  private:
    std::vector<Polytope> models;
    std::filesystem::directory_iterator dir;
    std::string* mutexIndice = new std::string();
    std::string* mIndex = new std::string();
    std::string folder = std::string();

    void LoadScenes() {
      //*mIndex = MuticePool::Lock(mIndex);
      
      retry:
      if (dir == end(dir)) { /*MuticePool::Unlock(*mIndex);*/  return; };
      if (dir->path().extension() == ".bin" || 
        dir->path().extension() == ".jpg" ||
        dir->path().extension() == ".png") { ++dir; goto retry; };
      auto dirStr = dir->path().string();
      ++dir;
      if (dir == end(dir)) { /*MuticePool::Unlock(*mIndex);*/  return; };

      std::cout << "Loading Scene: " << dirStr << "\n"
        << " Thread: "<< std::this_thread::get_id() <<"\n";
      //MuticePool::Unlock(*mIndex);

      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile(
        dirStr, 
        /*aiProcess_JoinIdenticalVertices   |*/
        aiProcess_Triangulate             |
        /*aiProcess_RemoveComponent       | Look into how this one works*/
        aiProcess_PreTransformVertices    |
        //aiProcess_RemoveRedundantMaterials|
        //aiProcess_FindDegenerates         |
        //aiProcess_ValidateDataStructure   |
        //aiProcess_FindInvalidData         |
        //aiProcess_FindInstances           |
        /*aiProcess_OptimizeMeshes          |
        aiProcess_OptimizeGraph           |*/
        aiProcess_SortByPType
        ); 

      if (scene == NULL) throw std::runtime_error("Assimp Error:" + std::string(importer.GetErrorString()));

      //auto id = MuticePool::Lock();
      std::vector<std::string> threadIds;
      for (size_t j = scene->mNumMeshes - 1; j < scene->mNumMeshes; --j) {

        //Rememer to add the UV ones later

        auto threadFunc = [this, &importer, j, dirStr] {
          //auto id = MuticePool::Lock();
          auto scene = importer.GetScene();
          //MuticePool::Unlock(id);
          Polytope poly;
          for (size_t i = scene->mMeshes[j]->mNumFaces - 1; i < scene->mMeshes[j]->mNumFaces; --i) {
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
                scene->mMeshes[j]->mVertices[i].x,
                scene->mMeshes[j]->mVertices[i].y,
                scene->mMeshes[j]->mVertices[i].z
              };
            }
            else {
              vertice.color = { 1.0,1.0,1.0 };
            }

            if (scene->mMeshes[j]->HasNormals()) {
              scene->mMeshes[j]->mNormals[i].Normalize();

              vertice.norm = {
                scene->mMeshes[j]->mNormals[i].x,
                scene->mMeshes[j]->mNormals[i].y,
                scene->mMeshes[j]->mNormals[i].z,
              }; //vertices


              if (folder == "Stanford") vertice.pos *= 10;
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

          //*mutexIndice = MuticePool::Lock(mutexIndice);
          std::cout << "Loaded Model: " << j << "\n"
            << " Scene: " << dirStr << "\n"
            << " Tris: " << scene->mMeshes[j]->mNumFaces << "\n"
            << " Verts: " << scene->mMeshes[j]->mNumVertices << "\n";
          models.emplace_back(poly);
          //MuticePool::Unlock(*mutexIndice);
        };
        threadFunc();
        //threadIds.push_back(ThreadPool::AddWork(threadFunc));
      }; //for every mesh
      //MuticePool::Unlock(id);
      
      //*mIndex = MuticePool::Lock(mIndex);
      ThreadPool::FinishWork(threadIds);
      std::cout << "Loaded Scene: " << dirStr << "\n"
        << " Thread: " << std::this_thread::get_id() << "\n";
      //MuticePool::Unlock(*mIndex);
      LoadScenes();
    }; //LoadModels

  }; //ModelFactory

}; //Polytopes

