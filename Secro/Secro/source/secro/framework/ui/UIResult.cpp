#include "UIResult.h"

secro::UIResult::UIResult(Change change)
{
	this->change = change;
}

secro::UIResult::UIResult()
{
	change = Change::Null;
}
