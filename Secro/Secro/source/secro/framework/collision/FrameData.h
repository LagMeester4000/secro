#pragma once
#include "HitboxChange.h"

namespace secro {
	struct Frame {
		//time at which the change happens
		float time;

		//actual change to the hitbox
		std::vector<HitboxChange> changes;
	};

	struct FrameData {
		int amountOfHitboxes;
		float duration;
		float landingLag;
		std::vector<Frame> frames;
	};
}

#include <cereal/types/vector.hpp>
namespace cereal {
	template<typename T>
	void serialize(T& ar, secro::Frame& frame)
	{
		ar(
			cereal::make_nvp("time", frame.time),
			cereal::make_nvp("changes", frame.changes)
		);
	}

	template<typename T>
	void serialize(T& ar, secro::FrameData& f)
	{
		ar(
			cereal::make_nvp("amountOfHitboxes", f.amountOfHitboxes),
			cereal::make_nvp("duration", f.duration),
			cereal::make_nvp("landingLag", f.landingLag),
			cereal::make_nvp("frames", f.frames)
		);
	}
}