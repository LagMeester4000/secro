#pragma once
#include <cereal/cereal.hpp>
#include <Box2D/Common/b2Math.h>

namespace secro {
	enum class HitboxChangeVersion : std::uint32_t {
		First,

	};

	struct HitboxChange {
		int index = 0;
		b2Vec2 position = { 0.f, 0.f };
		bool isActive = true;
		bool isHitbox = true;//else is hurtbox
		int hitNumber = 0;
		float width = 1.f, height = 1.f;
		int relativePriority, absolutePriority;

		float damage = 1.f;
		float knockbackAngle = -45;
		float knockbackPowerBase = 1;
		float knockbackPowerGrowth = 1;
		float hitstunAdjustment = 0.f;
	};
}

#pragma region serializetion
namespace cereal {
	template<typename T>
	void serialize(T& ar, b2Vec2& vec)
	{
		ar(cereal::make_nvp("x", vec.x), cereal::make_nvp("y", vec.y));
	}

	template<typename T>
	void serialize(T& ar, secro::HitboxChange& change, std::uint32_t const version)
	{
		switch (version)
		{
		case (std::uint32_t)secro::HitboxChangeVersion::First:
			ar(
				cereal::make_nvp("index", change.index),
				cereal::make_nvp("position", change.position),
				cereal::make_nvp("isActive", change.isActive),
				cereal::make_nvp("isHitbox", change.isHitbox),
				cereal::make_nvp("hitNumber", change.hitNumber),
				cereal::make_nvp("width", change.width),
				cereal::make_nvp("height", change.height),
				cereal::make_nvp("relativePriority", change.relativePriority),
				cereal::make_nvp("absolutePriority", change.absolutePriority),

				cereal::make_nvp("damage", change.damage),
				cereal::make_nvp("knockbackAngle", change.knockbackAngle),
				cereal::make_nvp("knockbackPowerBase", change.knockbackPowerBase),
				cereal::make_nvp("knockbackPowerGrowth", change.knockbackPowerGrowth),
				cereal::make_nvp("hitstunAdjustment", change.hitstunAdjustment)
			);
			break;
		}
	}
}
#pragma endregion