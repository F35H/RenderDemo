#pragma once

#include "Vulkan/GeneralIncludes.h"
#include "Vulkan/Objects/ModelTypes/Polyhedra.h"

namespace Polytopes {
  struct PolygonFactory {
    enum {
      Triangle = 0,
      Quad,
      Hexagon,
      Tetrahedron,
      Cube
    }; //enum

    std::vector<Polytope> polytopes;
    //std::vector<EdgeMesh*> polytopeEdges;

    PolygonFactory() {
      polytopes = {
        Polyhedra::Triangle(),
        Polyhedra::Quad(),
        Polyhedra::Hexagon(),

        Polyhedra::PlatonicSolids::Tetrahedron(),
        Polyhedra::PlatonicSolids::Cube(),
        Polyhedra::PlatonicSolids::Octahedron(),
        Polyhedra::PlatonicSolids::Icosahedron(),
        Polyhedra::PlatonicSolids::Dodecahedron(),
    
        Polyhedra::ArchemedianSolids::Cuboctahedron(),
        Polyhedra::ArchemedianSolids::Icosidodecahedron(),
        Polyhedra::ArchemedianSolids::TruncatedTetrahedron()
      }; //polytopes
    }; //PolygonFactory

    std::vector<Polytope> GetPolyhedra() {
      return polytopes;
    }; //GetPolyhedra

  }; //PolygonFactory

  struct PolytopeFactory {
    uint16_t recurseIndex = 0;

    PolytopeFactory() = default;

    std::vector<Polytope> GetPolyhedra() {
      return {
        Polyhedra::Triangle(),
        Polyhedra::Quad(),
        Polyhedra::Hexagon(),

        Polyhedra::PlatonicSolids::Tetrahedron(),
        Polyhedra::PlatonicSolids::Cube(),
        Polyhedra::PlatonicSolids::Octahedron(),
        Polyhedra::PlatonicSolids::Icosahedron(),
        Polyhedra::PlatonicSolids::Dodecahedron(),

        Polyhedra::ArchemedianSolids::Cuboctahedron(),
        Polyhedra::ArchemedianSolids::Icosidodecahedron(),
        Polyhedra::ArchemedianSolids::TruncatedTetrahedron()
      }; //polytopes;
    }; //GetModel

    std::vector<Polytope> GetFileModels(std::string dir) {
      LoadModels();
      return models;
    }; //GetPolyhedra

  private:
    Assimp::Importer importer;
    std::vector<Polytope> models;
    void LoadModels() {

    }; //LoadModels
  }; //ModelFactory

}; //Polytopes

