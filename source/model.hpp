#ifndef MODEL_HPP
#define MODEL_HPP

#include "mesh.hpp"
#include "material.hpp"

#include <vector>

class Model
{
public:
	Model ();
	~Model ();

	int						AddMesh (const Mesh& mesh);
	int						AddMaterial (const Material& material);

	int						MeshCount () const;
	int						MaterialCount () const;

	const Mesh&				GetMesh (int index) const;
	const Material&			GetMaterial (int index) const;

private:
	std::vector<Mesh>		meshes;
	std::vector<Material>	materials;
};

#endif
