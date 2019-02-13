#pragma once
#include <vector>
#include <SFML/Graphics/View.hpp>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class PlayerCharacter;

	class Camera {
	public:
		Camera();

		//update the position and zoom of the camera
		void update(float deltaTime, std::vector<PlayerCharacter*> players);

		//render the debug gui and change the viewport
		void render(sf::RenderWindow& window);

	private:
		sf::Vector2f getPlayerAveragePosition(std::vector<PlayerCharacter*>& players);
		sf::Vector2f getMaxWidthAndHeightDiff(std::vector<PlayerCharacter*>& players);
		void clamp();

	private: //tweakable
		//speed of the camera movement
		float speed;

		//camera viewing space frustom in game units
		float frustumWidth, frustumHeight, frustumDepth;

		//minimum and maximum distance between players
		float minPlayerDistance, maxPlayerDistance;
		
		float minZoomIn, maxZoomIn;

	private: //consts
		float screenWidth, screenHeight;

	private: //final
		float zoom;
		sf::Vector2f position;
	};
}