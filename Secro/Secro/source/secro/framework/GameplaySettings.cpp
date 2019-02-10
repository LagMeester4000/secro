#include "GameplaySettings.h"
#include <imgui.h>
#include "DebugOptions.h"

float secro::GameplaySettings::hitstunScalar = 0.03f;
float secro::GameplaySettings::DIInfluence = 15.f;

void secro::GameplaySettings::render()
{
	if (!DebugOptions::getOptions().enableGameplaySettingsEditor)
		return;

	if (ImGui::Begin("Gameplay Settings"))
	{
		ImGui::InputFloat("Hitstun scalar", &hitstunScalar);
		ImGui::InputFloat("Max DI", &DIInfluence);
	}
	ImGui::End();
}

float secro::GameplaySettings::calculateKnockback(float playerDamage, float moveBaseKB, float moveKBGrowth)
{
	return playerDamage * moveKBGrowth + moveBaseKB;
}

float secro::GameplaySettings::calculateHitstun(float hitDamage, float playerDamage, float hitKnockback, float adjustment)
{
	return hitKnockback * hitstunScalar + adjustment;
}
