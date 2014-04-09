#include "model.hpp"

Model::Model ()
{

}

Model::~Model ()
{

}

UIndex Model::AddMesh (const Mesh& mesh)
{
	meshes.push_back (mesh);
	return meshes.size () - 1;
}

UIndex Model::AddMaterial (const Material& material)
{
	materials.push_back (material);
	return materials.size () - 1;
}

UIndex Model::MeshCount () const
{
	return meshes.size ();
}

UIndex Model::MaterialCount () const
{
	return materials.size ();
}

const Mesh& Model::GetMesh (UIndex index) const
{
	return meshes[index];
}

const Material& Model::GetMaterial (UIndex index) const
{
	return materials[index];
}
