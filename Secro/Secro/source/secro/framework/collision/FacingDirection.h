#pragma once

namespace secro {
	enum class FacingDirection : char {
		Left,
		Right
	};

	inline FacingDirection flip(FacingDirection f)
	{
		if (f == FacingDirection::Left)
			return FacingDirection::Right;
		return FacingDirection::Left;
	}
}