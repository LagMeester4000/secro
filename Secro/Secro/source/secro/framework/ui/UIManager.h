#pragma once
#include <vector>
#include <memory>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class UIElement;
	class Controller;

	class UIManager {
	public:
		UIManager();

		void update(float deltaTime);
		
		void handleInput(const Controller& controller);

		void render(sf::RenderWindow& window);

		void pushTopFrame(std::shared_ptr<UIElement> frame);
		void popTopFrame();
		
		std::shared_ptr<UIElement> getTopFrame();

	private:
		std::vector<std::shared_ptr<UIElement>> frames;
	};
}