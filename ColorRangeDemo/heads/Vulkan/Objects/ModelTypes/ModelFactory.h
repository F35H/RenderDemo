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
      Polytopes::Tetrahedron(),
      Polytopes::Cube()
    }; //polytopes
  }; //PolygonFactory

  std::vector<Polyhedra> GetPolyhedra() {
    return polytopes;
  }; //GetPolyhedra


  //std::vector<Edges*> GetPolyhedraEdges() {
    //return polytopeEdges;
  //}; //GetPolyhedraEdges
}; //PolygonFactory