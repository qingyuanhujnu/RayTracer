#ifndef COMMON_HPP
#define COMMON_HPP

#define EPS		0.000001
#define INF		9999999999
#define PI		3.14159265359
#define DEGRAD	0.017453292519943295

typedef unsigned int	UIndex;

const UIndex InvalidIndex = (UIndex)-1;

#ifdef DEBUG
	#include <crtdbg.h>

	void DebugAssert (bool expr);
	bool DebugError (bool expr);

	#define DBGASSERT(expr)		DebugAssert (expr)
	#define DBGERROR(expr)		DebugError (expr)
#else
	#define DBGASSERT(expr)	
	#define DBGERROR(expr)		expr
#endif

bool IsZero (double a);
bool IsPositive (double a);
bool IsNegative (double a);
bool IsEqual (double a, double b);
bool IsLower (double a, double b);
bool IsGreater (double a, double b);

#endif
