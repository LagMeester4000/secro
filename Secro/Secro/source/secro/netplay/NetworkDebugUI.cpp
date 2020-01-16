#include "NetworkDebugUI.h"
#include "secro/framework/DebugOptions.h"
#include <imgui.h>
#include <string>

void secro::NetworkDebugUI::update(float ping, float fps, float timeDiff, int frame)
{
	pingOverTime[current % arSize] = ping;
	fpsOverTime[current % arSize] = fps;
	timeDiffOverTime[current % arSize] = timeDiff;
	++current;

	currentFrame = frame;
}

void secro::NetworkDebugUI::render()
{
	if (!DebugOptions::getOptions().enableNetworkRender)
		return;

	if (ImGui::Begin("Network"))
	{
		ImVec2 s(200.f, 80.f);
		ImGui::PlotLines("Ping", pingOverTime, 120, 0, 0, 0.f, 200.f, s);
		ImGui::PlotLines("Time Diff", timeDiffOverTime, 120, 0, 0, -10.f, 10.f, s);
		ImGui::PlotLines("FPS", fpsOverTime, 120, 0, 0, 0.f, 60.f, s);
		ImGui::InputInt("Current Frame", &currentFrame, 1, 100, ImGuiInputTextFlags_ReadOnly);
	}
	ImGui::End();
}
