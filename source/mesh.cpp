#include "mesh.hpp"

Triangle::Triangle () :
	v0 (-1),
	v1 (-1),
	v2 (-1)
{

}

Triangle::Triangle (int v0, int v1, int v2, int mat) :
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

int Mesh::AddVertex (const Coord& coord)
{
	vertices.push_back (coord);
	return vertices.size () - 1;
}

int Mesh::AddTriangle (const Triangle& triangle)
{
	triangles.push_back (triangle);
	normals.push_back (CalculateNormal (triangles.size () - 1));
	return triangles.size () - 1;
}

int Mesh::VertexCount () const
{
	return vertices.size ();
}

int Mesh::TriangleCount () const
{
	return triangles.size ();
}

const Coord& Mesh::GetVertex (int index) const
{
	return vertices[index];
}

const Triangle& Mesh::GetTriangle (int index) const
{
	return triangles[index];
}

const Coord& Mesh::GetNormal (int index) const
{
	return normals[index];
}

Coord Mesh::CalculateNormal (int index)
{
	const Triangle& triangle = GetTriangle (index);
	const Coord& v0 = GetVertex (triangle.v0);
	const Coord& v1 = GetVertex (triangle.v1);
	const Coord& v2 = GetVertex (triangle.v2);

	Coord edgeDir1 = v1 - v0;
	Coord edgeDir2 = v2 - v0;
	return Normalize (edgeDir1 ^ edgeDir2);
}