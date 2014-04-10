#ifndef MESH_HPP
#define MESH_HPP

#include "common.hpp"
#include "coord.hpp"
#include <vector>


class Mesh
{
public:
	static const UIndex NonCurved = (UIndex)-1;

	class Triangle
	{
	public:
		Triangle (UIndex vertex0, UIndex vertex1, UIndex vertex2, UIndex material, UIndex curveGroup = Mesh::NonCurved);
		~Triangle ();

		UIndex		vertex0;
		UIndex		vertex1;
		UIndex		vertex2;

		UIndex		normal0;
		UIndex		normal1;
		UIndex		normal2;

		UIndex		material;
		UIndex		curveGroup;
	};

	Mesh ();
	~Mesh ();

	UIndex					AddVertex (const Coord& coord);
	UIndex					AddTriangle (const Triangle& triangle);
	void					Finalize ();

	UIndex					VertexCount () const;
	UIndex					TriangleCount () const;

	const Coord&			GetVertex (UIndex index) const;
	const Triangle&			GetTriangle (UIndex index) const;

	Coord					GetNormal (UIndex index, const Coord& coordinate) const;

private:
	Coord					CalculateTriangleNormal (UIndex index);
	void					CalculateVertexNormals ();

	std::vector<Coord>		vertices;
	std::vector<Triangle>	triangles;
	std::vector<Coord>		triangleNormals;
	std::vector<Coord>		vertexNormals;
};

#endif
