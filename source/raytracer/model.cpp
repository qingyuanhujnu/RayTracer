#include "model.hpp"

Model::Model ()
{

}

Model::~Model ()
{

}

UIndex Model::AddMesh (const Mesh& mesh)
{
	geometry.push_back (mesh);
	return geometry.size () - 1;
}

UIndex Model::AddMaterial (const Material& material)
{
	materials.push_back (material);
	return materials.size () - 1;
}

UIndex Model::AddLight (const Light& light)
{
	lights.push_back (light);
	return lights.size () - 1;
}

UIndex Model::MeshCount () const
{
	return geometry.size ();
}

UIndex Model::MaterialCount () const
{
	return materials.size ();
}

UIndex Model::LightCount () const
{
	return lights.size ();
}

const Mesh& Model::GetMesh (UIndex index) const
{
	return geometry[index];
}

const Material& Model::GetMaterial (UIndex index) const
{
	return materials[index];
}

const Light& Model::GetLight (UIndex index) const
{
	return lights[index];
}

bool Model::Check () const
{
	UIndex materialCount = materials.size ();
	for (UIndex i = 0; i < geometry.size (); i++) {
		if (DBGERROR (!geometry[i].Check (materialCount))) {
			return false;
		}
	}
	return true;
}
