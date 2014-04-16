#include "random.hpp"
#include <time.h>
#include <random>

std::mt19937 eng ((unsigned int)time (NULL));                         // Mersenne Twister generator with a different seed at each run
std::uniform_real_distribution<double> dist (0.0, 1.0);

double random ()		// thread safe
{
	return dist (eng);
}