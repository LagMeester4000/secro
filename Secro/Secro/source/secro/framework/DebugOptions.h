#pragma once

namespace sf {
	class RenderWindow;
}

namespace secro {
	//static class that holds and displays debug options
	class DebugOptions {
		struct Options {
			//editors
			bool enableHitboxEditor = false;
			bool enablePlayerAttributeEditor = false;
			bool enableInputEditor = false;
			bool enableGamepadEditor = false;
			bool enableGameplaySettingsEditor = false;
			bool enablePhysicsRender = false;
			bool enableHitboxRender = false;
			bool enableNetworkRender = false;
		};

	public:
		//initialize the options
		static void init();

		//update the options (likely does nothing)
		static void update(float deltaTime);

		//render the debug options
		static void render(sf::RenderWindow& window);

		//get the options, very important for enabling and disabling windows
		static const Options& getOptions();

	private:
		static Options options;
	};
}