#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <imgui/imgui-SFML.h>
#include <imgui.h>
#include "Game.h"

const bool useVsync = false;
const float FPS = 60.0f; //The desired FPS. (The number of updates each second).

int main()
{
	//game start object
	auto game = secro::Game::createGame();

	bool redraw = true;      //Do I redraw everything on the screen?

	//actual drawing window
	sf::RenderWindow window(sf::VideoMode(800, 800, 32), "Hello");
	ImGui::SFML::Init(window, true);
	ImGui::CreateContext();
	window.setView(sf::View(sf::FloatRect(-21.6, -10, 31.6, 20)));

	if (useVsync)
		window.setVerticalSyncEnabled(true);
	
	//clock for timing
	sf::Clock clock;

	sf::Event ev;
	while (window.isOpen())
	{
		auto dtClock = clock.getElapsedTime();
		float deltaTime = dtClock.asSeconds();
		
		if (useVsync)
			clock.restart();

		//Wait until 1/60th of a second has passed, then update everything.
		if (!useVsync)
		{
			if (deltaTime >= 1.0f / FPS)
			{
				redraw = true; //We're ready to redraw everything
				clock.restart();
			}
			else //Sleep until next 1/60th of a second comes around
			{
				sf::Time sleepTime = sf::seconds((1.0f / FPS) - clock.getElapsedTime().asSeconds());
				sf::sleep(sleepTime);
			}
		}
		else
		{
			redraw = true;
		}

		//Handle input
		while (window.pollEvent(ev))
		{
			ImGui::SFML::ProcessEvent(ev);

			if (ev.type == sf::Event::Closed) window.close();
		}

		//Draw stuff if ready
		if (redraw)
		{
			redraw = false;
			window.clear(sf::Color(0, 0, 0));

			//draw things here
			ImGui::SFML::Update(window, dtClock);
			game->update(deltaTime);
			game->render(window);

			ImGui::SFML::Render(window);
			ImGui::EndFrame();
			window.display();
		}
	}

	ImGui::SFML::Shutdown();
	return 0;
}