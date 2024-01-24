//#pragma once
//#include <string>
//#include <vector>
//
//#include "tiny_gltf.h"
//
////#define TINYOBJLOADER_IMPLEMENTATION
//#define BUFFER_OFFSET(i) ((char *)NULL + (i))
//
//namespace UPCLASS {
//  namespace UPMATH {
//    struct upVector2 {
//      uint32_t u = 0;
//      uint32_t v = 0;
//    }; //upVector3
//
//    struct upVector3 {
//      uint32_t x = 0;
//      uint32_t y = 0;
//      uint32_t z = 0;
//
//      upVector3(uint32_t inX, uint32_t inY, uint32_t inZ) : x(inX), y(inY), z(inZ) {};
//    }; //upVector3
//
//    struct upVector4 {
//      uint32_t r = 0;
//      uint32_t g = 0;
//      uint32_t b = 0;
//      uint32_t a = 0;
//    }; //upVector3
//  }; //UpMath Namespace
//  
//  using namespace UPMATH;
//
//  struct upAsset {
//    std::vector < upVector4 > color;
//    std::vector < upVector3 > vertices;
//    std::vector < upVector3 > normals;
//    std::vector < upVector2 > textureCoordinates;
//    std::vector < uint32_t  > indices;
//
//    uint32_t faceCount;
//    uint32_t vertexCount;
//    uint32_t colorCount;
//    uint32_t indexCount;
//    uint32_t normalCount;
//  }; //upAsset
//
//  struct upClass {
//    std::vector<upAsset*> upAssets;
//    std::string err;
//    std::vector<std::string> warn;
//    uint32_t upAssetCount;
//
//    upClass() = default;
//    upClass(std::string filename, std::string extension) {  
//      if (extension == ".gltf" || extension == ".glb") {
//        tinygltf::Model model;
//        tinygltf::TinyGLTF loader;
//        std::string gltfErr;
//        std::string gltfWarn;
//        if (extension == ".gltf")loader.LoadASCIIFromFile(&model, &gltfErr, &gltfWarn, filename + extension);
//        if (extension == ".glb")loader.LoadBinaryFromFile(&model, &gltfErr, &gltfWarn, filename + extension);
//        //if (extension == ".bin")loader.LoadBinaryFromFile(&model, &err, &warn, filename + extension);
//
//        auto scene = model.scenes[model.defaultScene];
//
//        uint32_t positionAccessIndex;
//        uint32_t normalAccessIndex;
//        uint32_t indexAccessIndex;
//
//        uint32_t positionOffset;
//        uint32_t positionBuffView;
//        uint32_t positionBuffIndex;
//
//        for (auto& nodeIndex : scene.nodes) {
//          if (model.nodes[nodeIndex].mesh >= 0) {
//            for (auto& primitive : model.meshes[model.nodes[nodeIndex].mesh].primitives) {
//              for (auto& attrib : primitive.attributes) {
//                //Second is the index to the item in the "model" hiearchy
//                if (attrib.first == "POSITION") positionAccessIndex = attrib.second;
//                if (attrib.first == "NORMAL") normalAccessIndex = attrib.second;
//              }; //For every Attribute
//              indexAccessIndex = primitive.indices;
//            }; //primitives
//
//            auto asset = new upAsset();
//
//            asset->indexCount = model.accessors[indexAccessIndex].count;
//            asset->vertexCount = model.accessors[positionAccessIndex].count;
//            asset->normalCount = model.accessors[normalAccessIndex].count;
//
//            //Creating Indices
//            char* begin = BUFFER_OFFSET(model.accessors[indexAccessIndex].byteOffset);
//            char* end = BUFFER_OFFSET(model.accessors[indexAccessIndex].byteOffset) + model.accessors[indexAccessIndex].count;
//            if (!(model.accessors[indexAccessIndex].type == TINYGLTF_TYPE_SCALAR)) {
//              warn.push_back("Bad Accessor: Index");  continue;
//            };
//            if (model.accessors[indexAccessIndex].componentType == TINYGLTF_COMPONENT_TYPE_BYTE)
//              asset->indices = std::vector<uint32_t>((int8_t*)begin, (int8_t*)end);
//            if (model.accessors[indexAccessIndex].componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
//              asset->indices = std::vector<uint32_t>((uint8_t*)begin, (uint8_t*)end);
//            if (model.accessors[indexAccessIndex].componentType == TINYGLTF_COMPONENT_TYPE_SHORT)
//              asset->indices = std::vector<uint32_t>((int16_t*)begin, (int16_t*)end);
//            if (model.accessors[indexAccessIndex].componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
//              asset->indices = std::vector<uint32_t>((uint16_t*)begin, (uint16_t*)end);
//            if (model.accessors[indexAccessIndex].componentType == TINYGLTF_COMPONENT_TYPE_INT)
//              asset->indices = std::vector<uint32_t>((int32_t*)begin, (int32_t*)end);
//            if (model.accessors[indexAccessIndex].componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
//              asset->indices = std::vector<uint32_t>((uint32_t*)begin, (uint32_t*)end);
//            if (model.accessors[indexAccessIndex].componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
//              asset->indices = std::vector<uint32_t>((float*)begin, (float*)end);
//
//            //Creating Vertices
//            positionOffset = model.accessors[positionAccessIndex].byteOffset;
//            positionBuffView = model.accessors[positionAccessIndex].bufferView;
//            positionBuffIndex = model.bufferViews[positionBuffView].buffer;
//
//            if (!(model.accessors[indexAccessIndex].type == TINYGLTF_TYPE_VEC3)) {
//              warn.push_back("Bad Accessor: Vertex");  continue;
//            }; //Warning
//            if (model.accessors[indexAccessIndex].componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
//              uint_fast32_t i = 0;
//              auto verticeIndex = model.buffers[positionBuffIndex].data.at(0);
//              for (; i < model.bufferViews[positionBuffView].byteLength / model.accessors[positionAccessIndex].count; ++i) {
//                asset->vertices.emplace_back(
//                  UPMATH::upVector3(
//                    *(float*)verticeIndex,
//                    *(float*)verticeIndex += 4,
//                    *(float*)verticeIndex += 4));
//                verticeIndex += 4;
//              }; //for
//            } //if model
//            else warn.push_back("Bad Accessor: Vertex");  continue;
//            upAssets.emplace_back(asset);
//          }; //for every mesh
//        }//for every node
//      } //gltf, glb
//      //if (extension == ".obj") {
//      //  std::string objWarn;
//      //  std::string objErr; 
//      //  tinyobj::shape_t shapes;
//      //};
//    }; //upAsset ctor
//
//    void AddUpAsset(std::string file) {
//      
//    }; //AddUpAsset
//
//    void AddUpAsset(upAsset* asset) {
//      upAssets.emplace_back(asset);
//      ++upAssetCount;
//    }; //AddUpAsset
//
//    void AppendUpClass(upClass& upclass) {
//      upAssets.insert(--upAssets.end(), upclass.upAssets.begin(), upclass.upAssets.end());
//      upAssetCount = upAssets.size();
//    }; //AppendUpClass
//
//    upAsset* GetUpAsset(uint_fast8_t index) {
//      return upAssets[index];
//    }; //GetUpAsset
//
//    upAsset* GetLastUpAsset() {
//      return *(--upAssets.end());
//    }; //GetLastUpAsset
//
//  }; //Asset
//}; //UpClass