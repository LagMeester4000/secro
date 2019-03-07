#pragma once
#include <vector>
#include <memory>
#include <SFML/Graphics/View.hpp>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class UIElement;
	class Controller;
	class Game;

	class UIManager {
	public:
		UIManager(Game* game);

		void update(float deltaTime);
		
		void handleInput(const Controller& controller);

		void render(sf::RenderWindow& window);

		void pushTopFrame(std::shared_ptr<UIElement> frame);
		void popTopFrame();
		
		std::shared_ptr<UIElement> getTopFrame();

		void setView(sf::View newView);

		Game* getGame();

	private:
		std::vector<std::shared_ptr<UIElement>> frames;
		sf::View view;
		Game* game;
	};
}