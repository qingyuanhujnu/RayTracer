#ifndef RAY_HPP
#define RAY_HPP

#include "common.hpp"
#include "model.hpp"
#include "box.hpp"
#include "sphere.hpp"

class Ray
{
public:
	Ray (const Vec3& startPoint, const Vec3& rayDirection);

	const Vec3&		GetOrigin () const;
	const Vec3&		GetDirection () const;

	virtual bool	IsLengthReached (double currentLength) const = 0;

protected:
	Vec3			origin;
	Vec3			direction;
};

class SectorRay : public Ray
{
public:
	SectorRay (const Vec3& startPoint, const Vec3& endPoint);
	virtual bool IsLengthReached (double currentLength) const override;

private:
	double	length;
};

class InfiniteRay : public Ray
{
public:
	InfiniteRay (const Vec3& startPoint, const Vec3& rayDirection);
	virtual bool IsLengthReached (double currentLength) const override;
};

#endif
