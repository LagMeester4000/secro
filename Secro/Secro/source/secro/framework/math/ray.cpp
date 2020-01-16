#include "ray.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include "fmath.h"

using namespace secro;

sfloat secro::rayIntersect(Vector2 a1, Vector2 a2, Vector2 b1, Vector2 b2)
{
	Vector2 rel1 = min(a2, a1);
	Vector2 rel2 = min(b2, b1);
	sfloat s1 = rel1.y / rel1.x;
	sfloat s2 = rel2.y / rel2.x;

	

	return 0.f;
}
