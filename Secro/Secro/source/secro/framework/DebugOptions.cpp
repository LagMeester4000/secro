#include "DebugOptions.h"
#include <imgui.h>

using namespace secro;

typename DebugOptions::Options DebugOptions::options = typename DebugOptions::Options();

void secro::DebugOptions::init()
{
}

void secro::DebugOptions::update(float deltaTime)
{
}

void secro::DebugOptions::render(sf::RenderWindow & window)
{
	if (ImGui::BeginMainMenuBar())
	{
		//editor bools menu
		if (ImGui::BeginMenu("Editors"))
		{
			ImGui::Checkbox("Hitbox", &options.enableHitboxEditor);
			ImGui::Checkbox("Player Attributes", &options.enablePlayerAttributeEditor);
			ImGui::Checkbox("CPU Input", &options.enableInputEditor);
			ImGui::Checkbox("Gameplay Settings", &options.enableGameplaySettingsEditor);
			ImGui::Checkbox("Render Physics", &options.enablePhysicsRender);
			ImGui::Checkbox("Render Hitboxes", &options.enableHitboxRender);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

const typename secro::DebugOptions::Options & secro::DebugOptions::getOptions()
{
	return options;
}
