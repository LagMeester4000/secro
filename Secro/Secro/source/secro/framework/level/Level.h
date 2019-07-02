#pragma once
#include "secro/framework/physics/PhysicsManager.h"
#include "secro/framework/collision/HitboxManager.h"
#include "secro/gameplay/Camera.h"
#include "secro/framework/graphics/ParticleSystem.h"
#include <vector>
#include <memory>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class InputManager;
	class PlayerCharacter;
	class Entity;
	class RawSerializeBuffer;

	//level type that can have its own win condition
	class Level {
		friend class Game;
	public:
		Level(std::shared_ptr<InputManager> input);
		virtual ~Level() {}

		//start the level
		//called after all players have been added
		virtual void init();

		//update physics, camera, and players
		//shoul be called at the end of a child function
		virtual void update(float deltaTime);

		//render the stage, players, ui, and anything else
		virtual void render(sf::RenderWindow& window);
		void cameraRender(sf::RenderWindow& window);

		//adds a player to the level
		//called from the game/menu select
		virtual void addPlayer(std::shared_ptr<PlayerCharacter> newPlayer);

		//adds an entity to the level
		virtual void addEntity(std::shared_ptr<Entity> newEntity);

		//returns if the game is over
		//is virtual because each level can have their own win condition
		virtual bool isGameOver();

		//returns the player who won
		virtual std::shared_ptr<PlayerCharacter> getWinningPlayer();
		
		virtual void netSerSave(RawSerializeBuffer& buff);
		virtual void netSerLoad(RawSerializeBuffer& buff);

		//get the particle system
		ParticleSystem& getParticleSystem();

		//get the camera
		Camera& getCamera();

	protected:
		PhysicsManager physicsManager;
		std::shared_ptr<InputManager> inputManager;
		HitboxManager hitboxManager;
		ParticleSystem particleSystem;
		Camera camera;

		//entities
		std::vector<std::shared_ptr<PlayerCharacter>> players;
		std::vector<std::shared_ptr<Entity>> entities;
	};
}