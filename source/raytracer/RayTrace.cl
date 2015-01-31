
// OpenCL does not have float3 so I just use float4 and pack 1.0f in to the w component

typedef struct {
	float4 orig;
	float4 dir;
} ray;

typedef struct {
	float4 a, b, c;
} triangle;

float dotProd (float4 a, float4 b) 
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void printfloat4 (float4 a) {
	printf ("(%f, %f, %f, %f)\n", a.x, a.y, a.z, a.w);
}

bool intersects (const ray* r, const triangle* tri)
{
	float4 edge1Dir = tri->b - tri->a;
	float4 edge2Dir = tri->c - tri->a;
	float4 pVec = cross (r->dir, edge2Dir);

	float det = dotProd (edge1Dir, pVec);
	// TODO: front facing check (do I really want this?)

	float invDet = 1.0f / det;

	float4 tVector = r->orig - tri->a;
	float u = dotProd (tVector, pVec) * invDet;
	
	if (u < 0.f || u > 1.0f) {
		return false;
	}

	float4 qVector = cross (tVector, edge1Dir);
	float v = dotProd (r->dir, qVector) * invDet;
	if (v < 0.0f || (u + v) > 1.0) {
		return false;
	}

	float distance = dotProd (edge2Dir, qVector) * invDet;
	if (distance < 0.0f) {
		return false;
	}

	return true;
}

__kernel void get_field_color (__global const ray* rays,
								const int ray_count,
								__global const triangle* triangles,
								const int triangle_count,
								__global float4* color)
{
	const int idx = get_global_id (0);
		
	ray r = rays[idx];

	for (int j = 0; j < triangle_count; ++j) {		
		triangle tri = triangles[j];

		if (intersects (&r, &tri)) {
			color[idx].xyz = (1.0f);
			return;
		}
	}

	color[idx].xyz = (0.0f);
}