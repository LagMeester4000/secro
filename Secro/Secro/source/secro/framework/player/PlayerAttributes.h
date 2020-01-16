#pragma once
#include <cereal/cereal.hpp>
#include <secro/framework/math/Curve.h>

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
		
		//airdodge
		float airdodgeInvStart, airdodgeInvDuration;
		float airdodgeDuration;
		float airdodgeSpeed;
		float airdodgeLandingLag;
		Curve airdodgeSpeedCurve;

		//jump
		int jumpAmount;
		float jumpFullSpeed;
		float jumpShortSpeed;
		float doubleJumpSpeed;
		float jumpSquatDuration;

		//airdash
		float airDashSpeed;
		float airDashInputTimeFrame;
		float airDashCooldown;

		//dash (not the character dash, but the dash on the ground)
		float dashDuration;
		float dashInitialSpeed;
		Curve dashSpeedCurve;
		
		//ground move
		float runAcceleration;
		float walkMaxSpeed;
		float runMaxSpeed;
		float groundDeceleration;

		//tech
		float techInPlaceInvDuration;
		float techInPlaceDuration;
		float techRollInvDuration;
		float techRollDuration;
		float techRollSpeed;
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