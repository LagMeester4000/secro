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
		Airdodge,
		SpecialFall,


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
		//other
		AGrab,
		AGrabAir,
		KnockdownGetupAttack,

		//specials
		SpecialU,
		SpecialF,
		SpecialD,
		SpecialN,
		ALastAttack,//NULL

		//teching states
		TechInPlace,
		TechRollLeft,
		TechRollRight,

		//knockdonw states
		Knockdown,
		KnockdownGetup,
		KnockdownRollLeft,
		KnockdownRollRight,

		//should always be the last state
		MaxState,
	};
}