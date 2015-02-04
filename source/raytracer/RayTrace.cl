
// OpenCL does not have float3 so I just use float4 and pack 1.0f in to the w component

// ---------------DATA structs coming from the host------------------------------------------------

typedef struct {
	float4 orig;
	float4 dir;
} ray;

typedef struct {
	float4 a, b, c;
	float4 na, nb, nc;
	int matIdx;
	int doubleSided;
	int filler[2];		// 16 byte alignment
} triangle;

typedef struct {
	float4 pos;
	float4 color;
	float4 attenuation;
} light;

typedef struct {
	float4	color;

	float ambient;
	float diffuse;
	float specular;
	float shininess;

	float reflection;
	float transparency;
	float refractionIndex;

	int filler;			// 8 byte alignment
} material;

// ------------------------------------------------------------------------------------------------

#define MAX_DIST FLT_MAX / 16.0f
#define EPS 0.0001f

typedef struct {
	float4 pos;
	float dist;
	__global const triangle* tri;
} intersection;

bool intersects (const ray* r, __global const triangle* tri, intersection* outIsect)
{
	float4 edge1Dir = tri->b - tri->a;
	float4 edge2Dir = tri->c - tri->a;
	float4 pVec = cross (r->dir, edge2Dir);

	float det = dot (edge1Dir, pVec);
	if (det < EPS && !tri->doubleSided) {
		return false;
	}

	float invDet = 1.0f / det;

	float4 tVector = r->orig - tri->a;
	float u = dot (tVector, pVec) * invDet;
	
	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	float4 qVector = cross (tVector, edge1Dir);
	float v = dot (r->dir, qVector) * invDet;
	if (v < 0.0f || (u + v) > 1.0f) {
		return false;
	}

	float distance = dot (edge2Dir, qVector) * invDet;
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
	r.dir = normalize (l->pos - pos);
	r.orig = pos + EPS * r.dir;

	// Find the closest intersection.
	intersection minIsect;
	minIsect.dist = FLT_MAX;
	for (int j = 0; j < triangle_count; ++j) {
		__global const triangle* tri = &triangles[j];
		
		intersection isect;
		if (intersects (&r, tri, &isect)) {
			if (isect.dist < minIsect.dist)
				minIsect = isect;
		}
	}

	float lightDist = length (l->pos - pos);
	return lightDist < minIsect.dist;
}

float4 getReflectedDirection (const float4 direction, const float4 normal)
{
	float dotProduct = dot (normal, direction);
	return direction - (2.0f * normal * dotProduct);
}

float4 phongShading (const ray* ray,
						__global const light* light,
						__global const material* mat, 
						const intersection* isect, 
						const float4 normal)
{
	float4 lightDir = normalize (light->pos - isect->pos);
	float4 reflectionVector = getReflectedDirection (lightDir, normal);

	float lightNormalProduct = dot (lightDir, normal);	
	float diffuseCoeff = max (lightNormalProduct, 0.0f);

	float specularCoeff = pow (max (dot (reflectionVector, ray->dir), 0.0f), mat->shininess);

	float4 diffuseColor = light->color * mat->color;
	float4 color = diffuseColor * diffuseCoeff + mat->color * specularCoeff;		// TODO: mat.color should be the materials specular color
	
	// attenuation
	float intensity = 0.0f;
	float distance = length (isect->pos - light->pos);
	float denom = (light->attenuation.x + distance * light->attenuation.y + distance * light->attenuation.z * light->attenuation.z);
	if (denom < EPS) {
		intensity = 1.0f;
	}
	intensity = 1.0f / denom;
	color = color * intensity;

	return clamp (color, 0.0f, 1.0f);
}

float getTriangleArea (float a, float b, float c)
{
	float s = (a + b + c) / 2.0f;
	float areaSquare = s * (s - a) * (s - b) * (s - c);
	if (areaSquare < 0.0f) {
		return 0.0f;
	}
	return sqrt (areaSquare);
}

float4 barycentricInterpolation (float4 vertex0, float4 vertex1, float4 vertex2,
								float4 value0, float4 value1, float4 value2,
									   float4 interpolationVertex)
{
	float edge0 = length (vertex0 - vertex1);
	float edge1 = length (vertex1 - vertex2);
	float edge2 = length (vertex2 - vertex0);

	float distance0 = length (vertex0 - interpolationVertex);
	float distance1 = length (vertex1 - interpolationVertex);
	float distance2 = length (vertex2 - interpolationVertex);

	float area = getTriangleArea (edge0, edge1, edge2);
	if (area < EPS) {
		return value0;
	}

	float area0 = getTriangleArea (edge0, distance0, distance1);
	float area1 = getTriangleArea (edge1, distance1, distance2);
	float area2 = getTriangleArea (edge2, distance0, distance2);

	float4 interpolated = (value0 * area1 + value1 * area2 + value2 * area0) / area;
	return interpolated;
}

float4 trace_ray (const ray* ray,
				__global const triangle* triangles,
				const int triangle_count,
				__global const light* lights,
				const int light_count,
				__global const material* materials)		// This is an array but there is no need to know it's size, since we don't want to iterate over it.)
{
	float4 color = (float4) (0.0f, 0.0f, 0.0f, 0.0f);

	// Find the closest intersection.
	intersection minIsect;
	minIsect.dist = FLT_MAX;
	for (int i = 0; i < triangle_count; ++i) {
		__global const triangle* tri = &triangles[i];
		
		intersection isect;
		if (intersects (ray, tri, &isect)) {
			if (isect.dist < minIsect.dist)
				minIsect = isect;
		}
	}

	if (minIsect.dist < MAX_DIST) {		// no intersection
		__global const material* mat = &materials[minIsect.tri->matIdx];
		color += mat->ambient;

		// Light the point.
		for (int i = 0; i < light_count; ++i) {
			if (isPointLit (minIsect.pos, &lights[i], triangles, triangle_count)) {
				float4 normal = barycentricInterpolation (minIsect.tri->a, minIsect.tri->b, minIsect.tri->c,
															minIsect.tri->na, minIsect.tri->nb, minIsect.tri->nc,
															minIsect.pos);
			
				color += phongShading (ray, &lights[i], mat, &minIsect, normal);
			}
		}
	}

	return color;
}

__kernel void get_field_color (__global const ray* rays,
								const int ray_count,

								__global const triangle* triangles,
								const int triangle_count,

								__global const light* lights,
								const int light_count,

								__global const material* materials,		// This is an array but there is no need to know it's size, since we don't want to iterate over it.

								__global float4* color)
{
	const int idx = get_global_id (0);
	ray r = rays[idx];
		
	float4 col = trace_ray (&r, triangles, triangle_count, lights, light_count, materials);

	color[idx] = clamp (col, 0.0f, 1.0f);
}