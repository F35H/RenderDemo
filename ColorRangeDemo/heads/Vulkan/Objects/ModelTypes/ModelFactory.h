#pragma once

struct PolygonFactory {
  enum {
    Triangle = 0,
    Quad,
    Hexagon,
    Tetrahedron,
    Cube
  }; //enum

  std::vector<Polyhedra*> polytopes;
  //std::vector<EdgeMesh*> polytopeEdges;

  PolygonFactory() {
    polytopes = {
      new Polytopes::Triangle(),
      new Polytopes::Quad(),
      new Polytopes::Hexagon(),
      new Polytopes::Tetrahedron(),
      new Polytopes::Cube()
    }; //polytopes
  }; //PolygonFactory

  std::vector<Polyhedra*> GetPolyhedra() {
    return polytopes;
  }; //GetPolyhedra


  //std::vector<Edges*> GetPolyhedraEdges() {
    //return polytopeEdges;
  //}; //GetPolyhedraEdges
}; //PolygonFactory