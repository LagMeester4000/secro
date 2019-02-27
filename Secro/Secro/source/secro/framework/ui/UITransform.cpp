#include "UITransform.h"

using namespace secro;

UITransform secro::makeTransform(const UITransform & parent, const UITransform & child)
{
	UITransform ret;
	ret.position = parent.position + mul(child.position, parent.scale); //change for rotation
	ret.scale = mul(parent.scale, child.scale);
	ret.rotation = parent.rotation + child.rotation;
	return ret;
}
