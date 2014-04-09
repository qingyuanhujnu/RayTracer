#include "model.hpp"

Model::Model ()
{

}

Model::~Model ()
{

}

int Model::AddMesh (const Mesh& mesh)
{
	meshes.push_back (mesh);
	return meshes.size () - 1;
}

int Model::AddMaterial (const Material& material)
{
	materials.push_back (material);
	return materials.size () - 1;
}

int Model::MeshCount () const
{
	return meshes.size ();
}

int Model::MaterialCount () const
{
	return materials.size ();
}

const Mesh& Model::GetMesh (int index) const
{
	return meshes[index];
}

const Material& Model::GetMaterial (int index) const
{
	return materials[index];
}
