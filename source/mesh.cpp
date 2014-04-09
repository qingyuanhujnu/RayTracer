#include "mesh.hpp"

Triangle::Triangle () :
	v0 (-1),
	v1 (-1),
	v2 (-1)
{

}

Triangle::Triangle (UIndex v0, UIndex v1, UIndex v2, UIndex mat) :
	v0 (v0),
	v1 (v1),
	v2 (v2),
	mat (mat)
{

}

Triangle::~Triangle ()
{

}

Mesh::Mesh ()
{

}

Mesh::~Mesh ()
{

}

UIndex Mesh::AddVertex (const Coord& coord)
{
	vertices.push_back (coord);
	return vertices.size () - 1;
}

UIndex Mesh::AddTriangle (const Triangle& triangle)
{
	triangles.push_back (triangle);
	normals.push_back (CalculateNormal (triangles.size () - 1));
	return triangles.size () - 1;
}

UIndex Mesh::VertexCount () const
{
	return vertices.size ();
}

UIndex Mesh::TriangleCount () const
{
	return triangles.size ();
}

const Coord& Mesh::GetVertex (UIndex index) const
{
	return vertices[index];
}

const Triangle& Mesh::GetTriangle (UIndex index) const
{
	return triangles[index];
}

const Coord& Mesh::GetNormal (UIndex index) const
{
	return normals[index];
}

Coord Mesh::CalculateNormal (UIndex index)
{
	const Triangle& triangle = GetTriangle (index);
	const Coord& v0 = GetVertex (triangle.v0);
	const Coord& v1 = GetVertex (triangle.v1);
	const Coord& v2 = GetVertex (triangle.v2);

	Coord edgeDir1 = v1 - v0;
	Coord edgeDir2 = v2 - v0;
	return Normalize (edgeDir1 ^ edgeDir2);
}
