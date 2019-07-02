#include "HitboxId.h"

using namespace secro;

HitboxId HitboxId::None = { -1 };

bool secro::operator==(const HitboxId & v1, const HitboxId & v2)
{
	return v1.id == v2.id;
}

bool secro::operator!=(const HitboxId & v1, const HitboxId & v2)
{
	return v1.id != v2.id;
}
