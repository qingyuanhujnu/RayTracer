#include "octree.hpp"

Octree::Node::Node ()
{

}

Octree::Node::Node (const Box& box)
{
	SetBox (box);
}

Octree::Node::~Node ()
{

}

void Octree::Node::SetBox (const Box& box)
{
	this->box = box;
}

static bool IsCoordInBox (const Box& box, const Vec3& coord)
{
	if (IsLower (coord.x, box.min.x) || IsLower (coord.y, box.min.y) || IsLower (coord.z, box.min.z)) {
		return false;
	}
	if (IsGreater (coord.x, box.max.x) || IsGreater (coord.y, box.max.y) || IsGreater (coord.z, box.max.z)) {
		return false;
	}
	return true;
}

static bool IsTriangleInBox (const Box& box, const Vec3& v0, const Vec3& v1, const Vec3& v2)
{
	if (!IsCoordInBox (box, v0) || !IsCoordInBox (box, v1) || !IsCoordInBox (box, v2)) {
		return false;
	}
	return true;
}

static Box ConstructBox (Vec3 start, Vec3 offset)
{
	return Box (start, start + offset);
}

bool Octree::Node::AddTriangle (UIndex id, const Vec3& v0, const Vec3& v1, const Vec3& v2)
{
	if (!IsTriangleInBox (box, v0, v1, v2)) {
		return false;
	}

	if (children.empty ()) {
		children.resize (8);
		Vec3 offset = (box.max - box.min) / 2.0;
		children[0].SetBox (ConstructBox (box.min + Vec3 (0.0, 0.0, 0.0), offset));
		children[1].SetBox (ConstructBox (box.min + Vec3 (offset.x, 0.0, 0.0), offset));
		children[2].SetBox (ConstructBox (box.min + Vec3 (0.0, offset.y, 0.0), offset));
		children[3].SetBox (ConstructBox (box.min + Vec3 (offset.x, offset.y, 0.0), offset));
		children[4].SetBox (ConstructBox (box.min + Vec3 (0.0, 0.0, offset.z), offset));
		children[5].SetBox (ConstructBox (box.min + Vec3 (offset.x, 0.0, offset.z), offset));
		children[6].SetBox (ConstructBox (box.min + Vec3 (0.0, offset.y, offset.z), offset));
		children[7].SetBox (ConstructBox (box.min + Vec3 (offset.x, offset.y, offset.z), offset));
	}

	for (UIndex i = 0; i < 8; i++) {
		if (children[i].AddTriangle (id, v0, v1, v2)) {
			return true;
		}
	}

	triangles.push_back (id);
	return true;
}

static bool NodeContainsTriangle (const Octree::Node& node)
{
	const std::vector<UIndex>& triangles = node.GetTriangles ();
	if (!triangles.empty ()) {
		return true;
	}

	const std::vector<Octree::Node>& children = node.GetChildren ();
	for (UIndex i = 0; i < children.size (); i++) {
		if (NodeContainsTriangle (children[i])) {
			return true;
		}
	}

	return false;
}

bool Octree::Node::ContainsTriangle () const
{
	return NodeContainsTriangle (*this);
}

const Box& Octree::Node::GetBox () const
{
	return box;
}

const std::vector<UIndex>& Octree::Node::GetTriangles () const
{
	return triangles;
}

const std::vector<Octree::Node>& Octree::Node::GetChildren () const
{
	return children;
}

Octree::Octree ()
{

}

Octree::~Octree ()
{
	
}

void Octree::SetBox (const Box& box)
{
	startNode.SetBox (box);
}

bool Octree::AddTriangle (UIndex id, const Vec3& v0, const Vec3& v1, const Vec3& v2)
{
	bool result = startNode.AddTriangle (id, v0, v1, v2);
	DBGASSERT (result == true);
	return result;
}

const Octree::Node& Octree::GetStartNode () const
{
	return startNode;
}
