#pragma once
#include "StateMachine.h"
#include "MovementState.h"
#include "PlayerAttributes.h"
#include "../Entity.h"
#include "../collision/FacingDirection.h"
#include "../input/Controller.h"
#include "AttackCollection.h"

class b2Body;

namespace sf {
	class RenderWindow;
}

namespace secro {
	class Controller;
	class HitboxCollection;
	class HitboxManager;

	class PlayerCharacter : public Entity {
		friend class StateMachine;
	public:
		PlayerCharacter();
		PlayerCharacter(HitboxManager* hitboxManager, b2Body* body, std::shared_ptr<Controller> controller);

		virtual void init();
		virtual void setupStates(StateMachine& stateMachine);
		virtual void setupAttacks(AttackCollection& attacks);
		virtual void update(float deltaTime);
		virtual void render(sf::RenderWindow& window);

		b2Vec2 getPosition() override;

	private: //movement
		void updateMovement(float deltaTime);
		bool snapToGround(float distance);

		//to test if the character should keep running forward, or stop adding velocity
		bool keepRunning();

		//the current movement state
		MovementState movementState;

		//the movement attributes of the player
		PlayerAttributes attributes;

		//the direction the player is facing
		FacingDirection facingDirection;

		//the body used for terrain collision
		b2Body* physicsBody;
		
	private:
		//movement switch functions
		void stateStartJump();
		void stateStartJumpSquat();
		void stateStartDash();
		void stateStartWalk();
		void endAttack();
		void stateStartNewAttack(PlayerState attack);

		//tests
		bool groundSpeedTooHigh();

		//debug
		void debugRenderAttributes(sf::RenderWindow& window);

	public:
		MovementState getMovementState();
		const PlayerAttributes& getPlayerAttributes();
		FacingDirection getFacingDirection() override;
		const b2Body* getPhysicsBody();
		static bool isEqual(FacingDirection facing, Direction dir);

		//knock the player back (no hitstun)
		void knockBack(b2Vec2 knockback);

	private: //hitstun
		void updateHitstun(float deltaTime);

		//how many seconds of hitstun there are left
		float hitstun;

	public:
		//check if the player is in hitstun
		bool IsInHitstun();

		//put the player in hitstun state
		void putInHitstun(float duration);

	private: //states
		void updateState(float deltaTime);

		//the actual state of the player
		PlayerState state;
		
		//a frame timer used to update states
		float stateTimer;

		//the state machine that keeps track of when the state should be changed
		StateMachine stateMachine;

	public:
		//return the current player state
		PlayerState getState();

		//check if the state timer of the player is smaller than 0
		bool IsStateTimerDone();

	protected:
		//set the frame timer
		void setStateTimer(float seconds);

	private: //input
		//input object
		std::shared_ptr<Controller> input;

	public:
		//get the input object
		std::shared_ptr<Controller> getInput();

	private: //damage
		//damage value that goes from 0-100 and beyond
		float damage;

	public:
		//get the damage of the character
		float getDamage();

		//get the damage of the character as a scalar value
		//most of the time between 0-1
		float getDamageScalar();

		//deal damage to the character
		void addDamage(float damage);

	private: //attacks
		//update the current attack
		void updateAttack(float deltaTime);

		//manager cor collision boxes
		HitboxManager* hitboxManager;

		//object with all attacks inside
		AttackCollection attackCollection;

		//timer for the attack
		float attackTimer;

		//the current attack
		std::shared_ptr<HitboxCollection> currentAttackHitbox;

		//the player's hurtbox
		std::shared_ptr<HitboxCollection> hurtbox;

		//the id the player got hit by last
		int lastHitId = -1;

	public:
		//get the attack timer value
		float getAttackTimer();

		//get last hit id
		int& getLastHitId();
	};
}