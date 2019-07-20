#pragma once

namespace secro {
	enum class ColliderProfile {
		Entity, //does not collide with other colliders of Entity or EntityNoPlatform, but does collide with GroundSolid or GroundPlatform
		EntityNoPlatform, //same as Entity, but does not collide with PlatformGround
		SolidGround, //only collides with Entity and EntityNoPlatform, stops its movement
		PlatformGround, //only collides with Entity, collision only happens when the resulting normal points up
	};
}