#include "fmath.h"

float secro::clampOne(float f)
{
	if (f < 0.f)
		return 0.f;
	else if (f > 1.f)
		return 1.f;
	return f;
}
