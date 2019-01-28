#pragma once
#include <cereal/cereal.hpp>

namespace secro {
	enum class PlaterAttributesVersion {
		First,

	};

	struct PlayerAttributes {
		//air
		float fallSpeed;
		float airAcceleration;
		float airDeceleration;
		float airMaxSpeed;
		float fastfallSpeed;

		//jump
		int jumpAmount;
		float jumpFullSpeed;
		float jumpShortSpeed;
		float doubleJumpSpeed;
		float jumpSquatDuration;

		//dash
		float dashDuration;
		float dashInitialSpeed;
		
		//ground move
		float runAcceleration;
		float walkMaxSpeed;
		float runMaxSpeed;
		float groundDeceleration;
	};
}

#pragma region serialization

template<typename T>
void serialize(T& ar, secro::PlayerAttributes& attr, std::uint32_t const version)
{
	switch (version)
	{
	case (std::uint32_t)secro::PlaterAttributesVersion::First:
		ar(
			CEREAL_NVP(attr.fallSpeed),
			CEREAL_NVP(attr.airAcceleration),
			CEREAL_NVP(attr.airDeceleration),
			CEREAL_NVP(attr.airMaxSpeed),
			CEREAL_NVP(attr.fastfallSpeed),
			
			CEREAL_NVP(attr.jumpAmount),
			CEREAL_NVP(attr.jumpFullSpeed),
			CEREAL_NVP(attr.jumpShortSpeed),
			CEREAL_NVP(attr.doubleJumpSpeed),
			CEREAL_NVP(attr.jumpSquatDuration),
			
			CEREAL_NVP(attr.dashDuration),
			CEREAL_NVP(attr.dashInitialSpeed),

			CEREAL_NVP(attr.runAcceleration),
			CEREAL_NVP(attr.runMaxSpeed),
			CEREAL_NVP(attr.walkMaxSpeed),
			CEREAL_NVP(attr.groundDeceleration)
		);
		break;
	}
}

#pragma endregion