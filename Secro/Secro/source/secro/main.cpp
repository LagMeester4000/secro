#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <imgui/imgui-SFML.h>
#include <imgui.h>
#include "Game.h"
#include "secro/framework/detail/CommandArgs.h"
#include "secro/framework/GameInstance.h"
#include "sen/Test.h"
#include "framework/input/InputManager.h"
#include "framework/input/GamepadManager.h"

//windows only
#include <expandedresources.h>

void SetImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	style->WindowBorderSize = 0.f;
	style->ChildRounding = 0.f;
	style->WindowRounding = 0.f;
	style->FrameRounding = 0.f;
	style->GrabRounding = 0.f;
	style->PopupRounding = 0.f;
	style->ScrollbarRounding = 0.f;
	style->TabRounding = 0.f;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.211, 0.211, 0.211, 1.f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.952, 0.572, 0.286, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.858, 0.368, 0, 1.f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.952, 0.572, 0.286, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.952, 0.572, 0.286, 1.f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	//colors[ImGuiCol_Tab] = ImGui::ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	//colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	//colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	//colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}


const bool useVsync = false;
const bool useSleep = false;
const float FPS = 60.f; //The desired FPS. (The number of updates each second).
#define RENDER_MAX_FALSE
// change to RENDER_MAX_TRUE to turn on maximum framerate during rendering, but still 60hz gameplay loop

int main(int argc, char *argv[])
{
	//print compilation type
#if _DEBUG
	std::cout << "Running in DEBUG mode" << std::endl;
#else
	std::cout << "Running in RELEASE mode" << std::endl;
#endif

	//save args
	secro::CommandArgs args(argc, argv);
	secro::GameInstance::instance.isOnline = args.hasArg("online");

	//test SEN library
	sen::test();

	bool redraw = true;      //Do I redraw everything on the screen?

	auto screenType = sf::Style::Default;
	if (argc > 1)
	{
		if (args.hasArg("fullscreen"))
		{
			screenType = sf::Style::Fullscreen;
		}
	}

	//actual drawing window
	sf::RenderWindow window(sf::VideoMode(800, 450, 32), "Secro", screenType);
	ImGui::SFML::Init(window, true);
	ImGui::CreateContext();
	SetImGuiStyle();
	window.setView(sf::View(sf::FloatRect(-21.6f, -10.f, 31.6f, 20.f)));

	if (useVsync)
		window.setVerticalSyncEnabled(true);

	//game start object
	secro::GamepadManager::update();
	auto game = secro::Game::createGame(std::make_shared<secro::InputManager>());
	
	//clock for timing
	sf::Clock clock;

	sf::Event ev;
	while (window.isOpen())
	{
		//TEMP
		//stupid check to see if game is in game mode
		//BOOL has;
		//HasExpandedResources(&has);
		//if (has)
		//{
		//	std::cout << "IS GAME" << std::endl;
		//}
		//else
		//{
		//	std::cout << "IS NOT GAME" << std::endl;
		//}
		//

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
			else if (useSleep) //Sleep until next 1/60th of a second comes around
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
#ifndef RENDER_MAX_TRUE
			window.clear(sf::Color(0, 102, 204));

			//draw things here
			ImGui::SFML::Update(window, dtClock);
#endif

			//if (1.f / deltaTime < 55.f)
			//	std::cout << "FPS: " << 1.f / deltaTime << std::endl;
			
			secro::GamepadManager::update();
			game->update(deltaTime, window.hasFocus());

#ifndef RENDER_MAX_TRUE
			game->render(window);

			ImGui::SFML::Render(window);
			ImGui::EndFrame();
			window.display();
#endif

		}

#ifdef RENDER_MAX_TRUE
		window.clear(sf::Color(0, 102, 204));

		//draw things here
		ImGui::SFML::Update(window, dtClock);
		game->render(window);

		ImGui::SFML::Render(window);
		ImGui::EndFrame();
		window.display();
#endif
	}

	ImGui::SFML::Shutdown();
	return 0;
}