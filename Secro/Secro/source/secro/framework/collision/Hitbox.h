#pragma once
#include <functional>
#include "Box.h"
#include "FacingDirection.h"

namespace sf {
	class RenderWindow;
}

namespace secro {
	class PlayerCharacter;
	struct HitboxChange;

	//overlapping hitbox for colliding with other hitboxes
	class Hitbox {
	public:
		//update the hitbox
		virtual void update(HitboxChange& change);

		//render the hitbox
		virtual void render(sf::RenderWindow& window, b2Vec2 position, b2Vec2 scale) = 0;

		//get the relative space that the hitbox uses
		virtual Box getSpace() = 0;

		//check for collision with another hitbox
		virtual bool collide(Hitbox& other, b2Vec2 otherPosition, b2Vec2 otherScale, b2Vec2 position, b2Vec2 scale) = 0;

		//knock a player back
		void knockbackPlayer(PlayerCharacter* player, FacingDirection direction);

	public://collision properties
		//relative position
		b2Vec2 position;

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

		//extra functionality for special hit events
		std::function<void(PlayerCharacter*, Hitbox&)> hitEvent;

	};
}