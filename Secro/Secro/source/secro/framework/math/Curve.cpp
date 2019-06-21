#include "Curve.h"
#include <math.h>
#include <imgui.h>
#include <imgui/imgui-SFML.h>

const char* curveTypes[] = { "Linear", "Exponential", "Division", "Circle" };

float secro::Linear::calculate(float i)
{
	return i;
}

void secro::Linear::render()
{
}

float secro::Exponent::calculate(float i)
{
	return powf(i, exponent);
}

void secro::Exponent::render()
{
	ImGui::InputFloat("Exponent", &exponent);
}

float secro::Division::calculate(float i)
{
	return 1.f / i;
}

void secro::Division::render()
{
}

float secro::Circle::calculate(float i)
{
	return powf(1.f - powf(i, exponent), 1.f / exponent);
}

void secro::Circle::render()
{
	ImGui::InputFloat("Exponent", &exponent);
}

secro::Curve::Curve()
{
	currentFormula = curveTypes[0];
}

float secro::Curve::calculate(float input, bool clamp)
{
	float result = 0.f;
	input *= inputMultiplicant;
	input += inputAdd;

	std::visit([&result, input](auto& form) {
		result = form.calculate(input);
	}, formula);

	result *= resultMultiplicant;
	result += resultAdd;

	if (clamp)
	{
		if (result > 1.f)
			result = 1.f;
		else if (result < 0.f)
			result = 0.f;
	}

	return result;
}

void secro::Curve::renderCurveEditor(int precision)
{
	ImGui::PushID(this);

	float lineThickness = 2.f;
	float size = 250.f;

	auto* drawList = ImGui::GetWindowDrawList();

	//draw lines
	{
		ImGui::BeginChild("CurveGraph", ImVec2(size, size), true);

		auto pos = ImGui::GetWindowPos();

		float previousInVal = 0.f;
		float previousResult = 1.f - calculate(previousInVal);
		if (previousResult > 1.f)
			previousResult = 1.f;
		else if (previousResult < 0.f)
			previousResult = 0.f;

		//[0, precision]
		for (int i = 1; i <= precision; ++i)
		{
			float inVal = (float)i / (float)precision;
			float result = 1.f - calculate(inVal);
			if (result > 1.f)
				result = 1.f;
			else if (result < 0.f)
				result = 0.f;

			auto v1 = ImVec2(pos.x + previousInVal * size, pos.y + previousResult * size);
			auto winSize = ImGui::GetWindowSize();

			drawList->AddLine(v1,
				ImVec2(pos.x + inVal * size, pos.y + result * size), ImColor(1.f, 0.f, 0.f), lineThickness);

			//update previous vars
			previousInVal = inVal;
			previousResult = result;
		}

		ImGui::EndChild();
	}

	//draw attributes
	{
		//choose type
		bool changed = false;
		if (ImGui::BeginCombo("Formula Type", currentFormula))
		{
			for (int i = 0; i < IM_ARRAYSIZE(curveTypes); ++i)
			{
				bool selected = (currentFormula == curveTypes[i]);
				if (ImGui::Selectable(curveTypes[i], selected))
				{
					changed = true;
					currentFormula = curveTypes[i];

					switch (i)
					{
					case 0:
						setFormula(Linear());
						break;
					case 1:
						setFormula(Exponent());
						break;
					case 2:
						setFormula(Division());
						break;
					case 3:
						setFormula(Circle());
						break;
					default:
						break;
					}
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		//render curve type UI (usually none)
		std::visit([](auto& it) {
			it.render();
		}, formula);

		ImGui::InputFloat("Result Add", &resultAdd);
		ImGui::InputFloat("Result Multiplier", &resultMultiplicant);
		ImGui::InputFloat("Input Add", &inputAdd);
		ImGui::InputFloat("Input Multiplier", &inputMultiplicant);
	}

	ImGui::PopID();
}

