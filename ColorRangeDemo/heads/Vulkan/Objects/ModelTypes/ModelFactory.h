#pragma once

#include "Vulkan/GeneralIncludes.h"
#include "Vulkan/Objects/ModelTypes/Polyhedra.h"

struct PolygonFactory {
  enum {
    Triangle = 0,
    Quad,
    Hexagon,
    Tetrahedron,
    Cube
  }; //enum

  std::vector<Polyhedra> polytopes;
  //std::vector<EdgeMesh*> polytopeEdges;

  PolygonFactory() {
    polytopes = {
      Polytopes::Triangle(),
      Polytopes::Quad(),
      Polytopes::Hexagon(),

      Polytopes::PlatonicSolids::Tetrahedron(),
      Polytopes::PlatonicSolids::Cube(),
      Polytopes::PlatonicSolids::Octahedron(),
      Polytopes::PlatonicSolids::Icosahedron(),
      Polytopes::PlatonicSolids::Dodecahedron(),
    
      Polytopes::ArchemedianSolids::Cuboctahedron(),
      Polytopes::ArchemedianSolids::Icosidodecahedron(),
      Polytopes::ArchemedianSolids::TruncatedTetrahedron()
    }; //polytopes
  }; //PolygonFactory

  std::vector<Polyhedra> GetPolyhedra() {
    return polytopes;
  }; //GetPolyhedra


  //std::vector<Edges*> GetPolyhedraEdges() {
    //return polytopeEdges;
  //}; //GetPolyhedraEdges
}; //PolygonFactory