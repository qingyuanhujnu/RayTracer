
// OpenCL does not have float3 so I just use float4 and pack 1.0f in to the w component

// ---------------DATA structs coming from the host------------------------------------------------

typedef struct {
	float4 orig;
	float4 dir;
} ray;

typedef struct {
	float4 a, b, c;
	float4 na, nb, nc;
} triangle;

typedef struct {
	float4 pos;
	float4 color;
} light;

// ------------------------------------------------------------------------------------------------

#define MAX_DIST FLT_MAX / 16.0f
#define EPS 0.0001f

typedef struct {
	float4 pos;
	float dist;
	const triangle* tri;
} intersection;

float dotProd (float4 a, float4 b) 
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

float length3 (float4 v)
{
	return sqrt (v.x * v.x + v.y * v.y + v.z * v.z);
}

float4 normalize3 (float4 v)
{
	float invLen = 1.0 / length3 (v);
	float4 ret = (float4) (v.x*invLen, v.y*invLen, v.z*invLen, 1.0);

	return ret;
}

void printfloat4 (float4 a) {
	printf ("(%f, %f, %f, %f)\n", a.x, a.y, a.z, a.w);
}

bool intersects (const ray* r, const triangle* tri, intersection* outIsect)
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

	outIsect->dist = distance;
	outIsect->pos = r->orig + (distance * r->dir);
	outIsect->tri = tri;

	return true;
}

bool isPointLit (float4 pos, 
				__global const light* l, 
				__global const triangle* triangles, 
				const int triangle_count) 
{
	// Get ray from point to light
	ray r;
	r.dir = normalize3 (l->pos - pos);
	r.orig = pos + EPS * r.dir;

	// Find the closest intersection.
	intersection minIsect;
	minIsect.dist = FLT_MAX;
	for (int j = 0; j < triangle_count; ++j) {
		triangle tri = triangles[j];
		
		intersection isect;
		if (intersects (&r, &tri, &isect)) {
			if (isect.dist < minIsect.dist)
				minIsect = isect;
		}
	}

	float lightDist = length3 (l->pos - pos);
	return lightDist < minIsect.dist;
}

float4 phongShading (__global const light* l, const intersection* isect, const float4 normal)
{
	float4 lightDir = normalize3 (l->pos - isect->pos);
	float lightNormalProduct = dotProd (lightDir, normal);
	
	float diffuseCoeff = max (lightNormalProduct, 0.0f);
	float4 diffuseColor = l->color * (float4) (1.0, 1.0, 1.0, 0.0) /* material diffuse color */;

	return diffuseCoeff * diffuseColor;
}

__kernel void get_field_color (__global const ray* rays,
								const int ray_count,

								__global const triangle* triangles,
								const int triangle_count,

								__global const light* lights,
								const int light_count,

								__global float4* color)
{
	const int idx = get_global_id (0);
	color[idx].xyz = (0.0f);
	ray r = rays[idx];
		
	// Find the closest intersection.
	intersection minIsect;
	minIsect.dist = FLT_MAX;
	for (int i = 0; i < triangle_count; ++i) {
		triangle tri = triangles[i];
		
		intersection isect;
		if (intersects (&r, &tri, &isect)) {
			if (isect.dist < minIsect.dist)
				minIsect = isect;
		}
	}

	if (minIsect.dist > MAX_DIST) {		// no intersection
		return;
	}

	// Light the point.
	for (int i = 0; i < light_count; ++i) {
		if (isPointLit (minIsect.pos, &lights[i], triangles, triangle_count)) {
			color[idx] += phongShading (&lights[i], &minIsect, minIsect.tri->na);
		}
	}

	color[idx] = clamp (color[idx], 0.0f, 1.0f);
}