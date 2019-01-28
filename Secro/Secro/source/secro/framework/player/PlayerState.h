#pragma once

namespace secro {
	enum class PlayerState {
		None,

		//grounded states
		Stand,
		Dash,
		Run,
		Walk,
		JumpSquat,
		Shield,

		//disabling states
		LandingLag,
		Hitstun,

		//air states
		Jump,
		AirDodge,


		//attacks
		AFirstAttack,//NULL
		//ground attacks
		AJab,
		AUTilt,
		AFTilt,
		ADTilt,
		//air attacks
		ANAir,
		AUAir,
		AFAir,
		ABAir,
		ADAir,
		ALastAttack,//NULL


		//should always be the last state
		MaxState,
	};
}