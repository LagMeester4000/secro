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
	class Level;

	class PlayerCharacter : public Entity {
		friend class StateMachine;
	public:
		PlayerCharacter();
		PlayerCharacter(Level* level, HitboxManager* hitboxManager, b2Body* body, std::shared_ptr<Controller> controller);
		void lateSetup(Level* level, HitboxManager* hitboxManager, b2Body* body, std::shared_ptr<Controller> controller);

		virtual void init();
		virtual void setupStates(StateMachine& stateMachine);
		virtual void setupAttacks(AttackCollection& attacks);
		virtual void update(float deltaTime);
		virtual void render(sf::RenderWindow& window);

		b2Vec2 getPosition() override;
		void setPosition(b2Vec2 pos, bool resetVelocity = false);
		void reset(b2Vec2 position);
		void freeze();

	protected: //essentials
		Level* level;

	private: //movement
		void updateMovement(float deltaTime);
		bool snapToGround(float distance, bool startAtBottom = false);
		void resizeVelocity(float newSize);

	public:
		//to test if the character should keep running forward, or stop adding velocity
		bool keepRunning();

	protected:
		//the current movement state
		MovementState movementState;
		
		//the previous movement state
		MovementState previousMovementState;

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

		//should the player be able to slide off platforms
		bool canSlideOffPlatforms = true;

		//the previous position of the player
		//used for the canSlideOffPlatforms functionality
		b2Vec2 previousPosition;

		//should the player have friction
		bool shouldHaveFriction = true;

		//deadzone on the x axis of the stick to enable movement
		float walkDeadzone;

	private:
		//movement switch functions
		void stateEndStand();
		void stateStartJump();
		void stateStartJumpSquat();
		void stateStartDash();
		void stateEndDash();
		void stateStartWalk();
		void stateStartShield();
		void stateEndShield();
		void stateUpdateDash(float deltaTime);
		void stateUpdateStand(float deltaTime);

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
		
		//getters
		float getWalkDeadzone();

		//knock the player back (no hitstun)
		void knockBack(b2Vec2 knockback);

	public:
		//function used to start and load a new attack state
		void stateStartNewAttack(PlayerState attack);

		//function called when an attack state is ended
		void endAttack();

		//function to try and double jump
		void tryDoubleJump(float deltaTime);

	protected:
		//set the current movement state
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

		//get the hitstun value
		float getHitstun();

	private: //states
		void updateState(float deltaTime);

		//the actual state of the player
		PlayerState state;
		
		//a frame timer used to update states
		float stateTimer;

		//to keep track of changes in the state timer
		float previousStateTimer;

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

		//the current attack state
		PlayerState currentAttackState;

		//the current attack
		std::shared_ptr<HitboxCollection> currentAttackHitbox;

		//frame data of the hurtbox
		FrameData hurtboxFrames;

		//the player's hurtbox
		std::shared_ptr<HitboxCollection> hurtbox;

		//to check if the attack has connected
		bool hasAttackHit;

	public:
		//get the attack timer value
		float getAttackTimer();

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

		//range of freezeShake
		float freezeShake = 0.1f;

		//how much freezeShake per hitlag
		float freezeShakePerHitlag = 0.5f;

		//how much base freezeShake
		float freezeShakeBase = 0.05f;

	public:
		//check if the player is in hitlag
		bool isInHitlag();

		//put the player in hitlag
		void putInHitlag(float duration);

		//returns random freezeshake radius
		b2Vec2 calcFreezeShake();

	public: //teching
		bool stateCanTech();
		bool stateCanTechEnd();

	private:
		void stateTechLeftBegin();
		void stateTechRightBegin();
		void stateTechInPlaceBegin();
		void stateTechEnd();
		bool stateCanKnockDown();
		void stateKnocdownBegin();
		void stateKnockdownEnd();
		bool stateCanKnockdownRollLeft();

		

	private: //airdodge
		bool stateCanAirdodge();
		bool stateCanEarlyAirdodge();
		void stateAirdodgeStart();
		void stateAirdodgeEnd();
		void stateUpdateAirdodge(float deltaTime);

	private: //invincibility
		//update the invincibility
		void updateInvincibility(float deltaTime);

		//timer to check if character is invincible
		float invincibilityTimer;
		
	protected:
		//make the character invincible
		void makeInvincible(float duration);

	public:
		//check if the character is invincible
		bool isInvincible();

	private: //DI
		//update the DI scalar
		void updateDI(float deltaTime);

		//apply the DI to the velocity
		void applyDI();

		//float variable for directional influence
		//as a scalar from -1 to 1
		float DI;

	public:
		//get the DI in degrees
		float getDI(float angle);

	private: //air dash
		float airDashTimer = 0.f;
		Direction airDashPreviousDirection;
		Direction airDashDirection;
		bool canAirDash = false;
		bool isAirDashUsed = false;

	protected:
		void updateAirDashTimer(float deltaTime);
		void conditionAirDash();
		void resetAirDashTimer();
	};
}