#pragma once

namespace secro {
	//identifier used to look up hitboxes
	struct HitboxId {
		static HitboxId None;
		int id = -1;
	};

	bool operator==(const HitboxId& v1, const HitboxId& v2);
	bool operator!=(const HitboxId& v1, const HitboxId& v2);
}