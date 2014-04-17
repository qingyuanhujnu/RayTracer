#ifndef MODEL_HPP
#define MODEL_HPP

#include "mesh.hpp"
#include "material.hpp"
#include "light.hpp"

#include <vector>

class Model
{
public:
	Model ();
	~Model ();

	UIndex					AddMesh (const Mesh& mesh);
	UIndex					AddMaterial (const Material& material);
	UIndex					AddLight (const Light& light);

	UIndex					MeshCount () const;
	UIndex					MaterialCount () const;
	UIndex					LightCount () const;

	const Mesh&				GetMesh (UIndex index) const;
	const Material&			GetMaterial (UIndex index) const;
	const Light&			GetLight (UIndex index) const;

	bool					Check () const;

private:
	std::vector<Mesh>		meshes;
	std::vector<Material>	materials;
	std::vector<Light>		lights;
};

#endif
