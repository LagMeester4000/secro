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
		void setPosition(b2Vec2 pos);
		void reset(b2Vec2 position);
		void freeze();

	private: //movement
		void updateMovement(float deltaTime);
		bool snapToGround(float distance);

		//to test if the character should keep running forward, or stop adding velocity
		bool keepRunning();

	protected:
		//the current movement state
		MovementState movementState;

		//the movement attributes of the player
		PlayerAttributes attributes;

		//the direction the player is facing
		FacingDirection facingDirection;

		//the body used for terrain collision
		b2Body* physicsBody;
		
		//the amount of jumps that the player has left
		int jumpsLeft;

		//should the player be influenced by gravity?
		bool useGravity = true;

	private:
		//movement switch functions
		void stateStartJump();
		void stateStartJumpSquat();
		void stateStartDash();
		void stateStartWalk();
		void endAttack();
		void stateStartNewAttack(PlayerState attack);
		void stateStartShield();
		void stateEndShield();

		//tests
		bool groundSpeedTooHigh();
		bool canDropThroughPlatform();

		//debug
		void debugRenderAttributes(sf::RenderWindow& window);

	public:
		MovementState getMovementState();
		const PlayerAttributes& getPlayerAttributes();
		FacingDirection getFacingDirection() override;
		const b2Body* getPhysicsBody();
		static bool isEqual(FacingDirection facing, Direction dir);
		static bool isOpposite(FacingDirection facing, Direction dir);

		//knock the player back (no hitstun)
		void knockBack(b2Vec2 knockback);

	protected:
		void setMovementState(MovementState m);

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

	protected: //input
		//input object
		std::shared_ptr<Controller> input;

	public:
		//get the input object
		std::shared_ptr<Controller> getInput();

	private: //damage
		//damage value that goes from 0-100 and beyond
		float damage;

		//DEBUG
		//the damage the player is reset to when killed
		float debugDamage = 0.f;

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

		//frame data of the hurtbox
		FrameData hurtboxFrames;

		//the player's hurtbox
		std::shared_ptr<HitboxCollection> hurtbox;

		//the id the player got hit by last
		int lastHitId = -1;

		//to check if the attack has connected
		bool hasAttackHit;

	public:
		//get the attack timer value
		float getAttackTimer();

		//get last hit id
		int& getLastHitId();

		//set the hasAttackHit
		void attackHasHit();

		//get the hasAttackHit
		bool getHasAttackHit();

		//check if the player is attacking
		bool isAttacking();

	private: //hitlag
		//update the hitlag variable
		void updateHitlag(float deltaTime);

		//the amount of hitlag that is left
		float hitlag;

		//the velocity before the hit
		b2Vec2 hitlagVelocity;

	public:
		//check if the player is in hitlag
		bool isInHitlag();

		//put the player in hitlag
		void putInHitlag(float duration);
	};
}