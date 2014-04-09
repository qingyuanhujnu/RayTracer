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

	UIndex					AddMesh (const Mesh& mesh);
	UIndex					AddMaterial (const Material& material);

	UIndex					MeshCount () const;
	UIndex					MaterialCount () const;

	const Mesh&				GetMesh (UIndex index) const;
	const Material&			GetMaterial (UIndex index) const;

private:
	std::vector<Mesh>		meshes;
	std::vector<Material>	materials;
};

#endif
