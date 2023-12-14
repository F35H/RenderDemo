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

  struct ModelFactory {
    std::vector<Polytope> models;
    
    ModelFactory() {
      models = {

      }; //models
    }; //ModelFactory

    std::vector<Polytope> GetModels() {
      return models;
    }; //GetPolyhedra
  }; //ModelFactory

}; //Polytopes

