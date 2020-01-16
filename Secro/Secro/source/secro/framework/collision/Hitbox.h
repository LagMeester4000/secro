#pragma once
#include <functional>
#include "secro/framework/physics/Vector2.h"
#include "FacingDirection.h"

namespace sf {
	class RenderWindow;
}

namespace secro {
	class PlayerCharacter;
	struct HitboxChange;
	class RawSerializeBuffer;

	struct Box {
		Vector2 p1, p2;
	};

	//overlapping hitbox for colliding with other hitboxes
	class Hitbox {
	public:
		//update the hitbox
		void update(HitboxChange& change);

		//render the hitbox
		void render(sf::RenderWindow& window, Vector2 position, Vector2 scale);

		//get the relative space that the hitbox uses
		Box getSpace();

		//check for collision with another hitbox
		bool collide(Hitbox& other, Vector2 otherPosition, Vector2 otherScale, Vector2 position, Vector2 scale);

		//knock a player back
		void knockbackPlayer(PlayerCharacter* player, FacingDirection direction);

		//calculate the knockback vector resulting from a hit
		Vector2 getKnockback(PlayerCharacter* player);

		//net serialize save
		void netSerSave(RawSerializeBuffer& buff);

		//net serialize load
		void netSerLoad(RawSerializeBuffer& buff);

	public://collision properties
		//relative position
		Vector2 position;

		//size of hitbox
		float radius;

		//to check wether the box is a hit or hurbox
		bool isHitbox;

		//wether the hitbox is active
		bool isActive;

		//the number of the hit of a move
		//a player cannot be hit by a hitbox with the same hitNumber that he was hit by previously (unless it's from a different player)
		int hitNumber;

		//priority this hitbox has over hitboxes with the same owner 
		int relativePriority;

		//priority this hitbox has over hitboxes with another owner
		int absolutePriority;

		//if the hitbox is a shield
		bool isShieldBox;

		//if the hitbox is a grab
		bool isGrabBox;

	public://hit properties
		//damage dealt on hit
		float damage;

		//angle at which the opponent is hit back
		float knockbackAngle;

		//the power at which the opponent is knocked back 
		//knockbackPower = knockbackPowerBase + other.damage * knockbackPowerGrowth
		float knockbackPowerBase;
		float knockbackPowerGrowth;

		//flat amount of hitstun that is added (or removed) when the opponent is hit
		float hitstunAdjustment;

		//extra freezeframes
		float extraFreezeFrames;

		//extra functionality for special hit events
		std::function<void(PlayerCharacter*, Hitbox&)> hitEvent;

	};
}