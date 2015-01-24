#ifndef MESH_HPP
#define MESH_HPP

#include "common.hpp"
#include "vec3.hpp"
#include "vec2.hpp"
#include "transformation.hpp"
#include "box.hpp"
#include "sphere.hpp"
#include "octree.hpp"
#include <vector>

class Mesh
{
public:
	static const UIndex NonCurved = (UIndex) -1;

	class Vertex {
	public:
		Vec3 pos;
		Vec2 texCoord;

		// TODO remove default parameter
		Vertex (const Vec3& pos, const Vec2& texCoord = Vec2 ());
	};

	class Triangle
	{
	public:
		enum NormalMode
		{
			UserDefined,
			Calculated
		};

		Triangle (UIndex vertex0, UIndex vertex1, UIndex vertex2, UIndex material, UIndex curveGroup = Mesh::NonCurved);
		Triangle (UIndex vertex0, UIndex vertex1, UIndex vertex2, UIndex normal1, UIndex normal2, UIndex normal3, UIndex material);
		~Triangle ();

		bool		Check (UIndex materialCount, UIndex vertexCount, UIndex userDefinedVertexNormalCount, UIndex calculatedVertexNormalCount) const;

		UIndex		vertex0;
		UIndex		vertex1;
		UIndex		vertex2;

		UIndex		normal0;
		UIndex		normal1;
		UIndex		normal2;

		UIndex		material;
		UIndex		curveGroup;
		
		NormalMode	normalMode;
	};

	Mesh ();
	~Mesh ();

	UIndex					AddVertex (const Vertex& vertex);
	void					SetVertex (UIndex index, const Mesh::Vertex& vertex);
	UIndex					AddNormal (const Vec3& normal);
	UIndex					AddTriangle (const Triangle& triangle);

	void					Transform (const Transformation& transformation);
	void					SetDoubleSided (bool isDoubleSided);

	void					Finalize ();

	UIndex					VertexCount () const;
	UIndex					TriangleCount () const;

	const Vertex&			GetVertex (UIndex index) const;
	const Triangle&			GetTriangle (UIndex index) const;

	bool					IsDoubleSided () const;
	Vec3					GetNormal (UIndex index, const Vec3& coord) const;
	const Box&				GetBoundingBox () const;
	const Sphere&			GetBoundingSphere () const;
	const Octree&			GetOctree () const;

	bool					Check (UIndex materialCount) const;

private:
	Vec3					CalculateTriangleNormal (UIndex index);
	void					CalculateVertexNormals ();
	void					CalculateBoundingShapes ();
	void					CalculateOctree ();

	std::vector<Vertex>		vertices;
	std::vector<Triangle>	triangles;

	std::vector<Vec3>		userDefinedVertexNormals;
	std::vector<Vec3>		calculatedTriangleNormals;
	std::vector<Vec3>		calculatedVertexNormals;

	bool					doubleSided;
	bool					finalized;

	Box						boundingBox;
	Sphere					boundingSphere;
	Octree					octree;
};

#endif
