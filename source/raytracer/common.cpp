#include "common.hpp"
#include <math.h>

#ifdef DEBUG

void DebugAssert (bool expr)
{
	_ASSERT (expr);
}

bool DebugError (bool expr)
{
	_ASSERT (!expr);
	return expr;
}

#endif

bool IsZero (double a)
{
	return fabs (a) < EPS;
}

bool IsPositive (double a)
{
	return a > EPS;
}

bool IsNegative (double a)
{
	return a < -EPS;
}

bool IsEqual (double a, double b)
{
	return fabs (a - b) < EPS;
}

bool IsLower (double a, double b)
{
	return b - a > EPS;
}

bool IsGreater (double a, double b)
{
	return a - b > EPS;
}
