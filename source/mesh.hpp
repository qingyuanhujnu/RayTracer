#ifndef MESH_HPP
#define MESH_HPP

#include "common.hpp"
#include "coord.hpp"
#include <vector>

class Triangle
{
public:
	Triangle ();
	Triangle (UIndex v0, UIndex v1, UIndex v2, UIndex mat);
	~Triangle ();

	UIndex		v0;
	UIndex		v1;
	UIndex		v2;
	UIndex		mat;
};

class Mesh
{
public:
	Mesh ();
	~Mesh ();

	UIndex					AddVertex (const Coord& coord);
	UIndex					AddTriangle (const Triangle& triangle);

	UIndex					VertexCount () const;
	UIndex					TriangleCount () const;

	const Coord&			GetVertex (UIndex index) const;
	const Triangle&			GetTriangle (UIndex index) const;
	const Coord&			GetNormal (UIndex index) const;

private:
	Coord					CalculateNormal (UIndex index);

	std::vector<Coord>		vertices;
	std::vector<Triangle>	triangles;
	std::vector<Coord>		normals;
};

#endif
