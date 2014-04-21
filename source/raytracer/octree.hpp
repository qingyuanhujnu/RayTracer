#ifndef OCTREE_HPP
#define OCTREE_HPP

#include "common.hpp"
#include "box.hpp"
#include <vector>

class Octree
{
public:
	class Node
	{
	public:
		Node ();
		Node (const Box& box);
		~Node ();

		void						Set (const Box& box);
		bool						AddTriangle (UIndex id, const Vec3& v0, const Vec3& v1, const Vec3& v2);

		const Box&					GetBox () const;
		const std::vector<UIndex>&	GetTriangles () const;
		const std::vector<Node>&	GetChildren () const;

	private:
		Box							box;
		std::vector<UIndex>			triangles;
		std::vector<Node>			children;
	};

	Octree ();
	~Octree ();

	void			Set (const Box& box);
	bool			AddTriangle (UIndex id, const Vec3& v0, const Vec3& v1, const Vec3& v2);

	const Node&		GetStartNode () const;

private:
	Node			startNode;
};

#endif
