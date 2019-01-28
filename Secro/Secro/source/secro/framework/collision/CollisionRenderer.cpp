#include "CollisionRenderer.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>

secro::CollisionRenderer::CollisionRenderer()
{
	frameData.amountOfHitboxes = 1;
}

void secro::CollisionRenderer::render(sf::RenderWindow & window, FrameData & framedata, float time, b2Vec2 position, float scale)
{
}

void secro::CollisionRenderer::render(sf::RenderWindow & window, HitboxChange & change, bool isSelected, b2Vec2 scale)
{
	sf::Vector2f aScale = { scale.x, scale.y };

	//actual hitbox
	sf::CircleShape c(change.width * aScale.y);
	if (isSelected)
		c.setFillColor(sf::Color(0, 255, 0, 126));
	else
		c.setFillColor(sf::Color(255, 0, 0, 126));
	sf::Vector2f cPos = { change.position.x - change.width, change.position.y - change.width };
	cPos.x *= aScale.x;
	cPos.y *= aScale.y;
	c.setPosition(cPos);
	window.draw(c);
	
	//angle
	sf::RectangleShape arr({ change.width * aScale.y, 0.1f });
	sf::Vector2f norm;
	norm.x = cos(change.knockbackAngle);
	norm.y = sin(change.knockbackAngle);
	sf::Vector2f arrPos = { change.position.x, change.position.y };
	arrPos.x *= aScale.x;
	arrPos.y *= aScale.y;
	arr.setPosition(arrPos);
	arr.setRotation(change.knockbackAngle);
	window.draw(arr);
}

void secro::CollisionRenderer::render(sf::RenderWindow & window, FrameData & framedata, float time)
{
	std::vector<HitboxChange> toRender(framedata.amountOfHitboxes);

	//find all current changes and put them in the toRender list
	for (auto& it : framedata.frames)
	{
		if (it.time <= time)
		{
			for (auto& change : it.changes)
			{
				if (toRender.size() > change.index)
					toRender[change.index] = change;
			}
		}
	}

	for (auto& change : toRender)
	{
		if (change.index == selectedHitbox)
			render(window, change, true);
		else
			render(window, change);
	}
}

static char filePath[32];

#include <iostream>
#include <fstream>
#include <istream>
#include <experimental/filesystem>
#include <cereal/archives/json.hpp>
void secro::CollisionRenderer::renderFrameDataEditor(sf::RenderWindow & window)
{
	namespace fs = std::experimental::filesystem;

	if (ImGui::Begin("Frame Data Editor"))
	{
		ImGui::InputText("File", filePath, 32);

		if (ImGui::Button("Save"))
		{
			//if (!fs::exists(filePath))
			{
				std::ofstream file(filePath);
				cereal::JSONOutputArchive out(file);

				out(frameData);
			}
			//else
			//{
			//	std::cout << "file '" << filePath << "' already exists" << std::endl;
			//}
		}

		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			if (fs::exists(filePath))
			{
				std::ifstream file(filePath);
				cereal::JSONInputArchive in(file);

				in(frameData);
			}
			else
			{
				std::cout << "file '" << filePath << "' doesn't exist" << std::endl;
			}
		}

		ImGui::Separator();

		ImGui::InputFloat("LandingLag", &frameData.landingLag);
		ImGui::InputInt("AmountOfHitboxes", &frameData.amountOfHitboxes);
		ImGui::InputFloat("MoveDuration", &frameData.duration);
		ImGui::SliderFloat("Time", &time, 0.f, frameData.duration);
		if (ImGui::Button("Add New Frame"))
		{
			frameData.frames.push_back(Frame());
		}
		if (ImGui::Button("Sort"))
		{
			std::sort(frameData.frames.begin(), frameData.frames.end(), [](Frame& f1, Frame& f2)
			{
				return f1.time < f2.time;
			});
		}

		ImGui::Separator();

		for (size_t i = 0; i < frameData.frames.size(); ++i)
		{
			bool disp = i == selectedFrame;
			if (ImGui::Selectable(std::to_string(i).c_str(), &disp))
			{
				selectedFrame = i;
			}
		}

		ImGui::End();
	}

	if (ImGui::Begin("Frame"))
	{
		if (selectedFrame < frameData.frames.size())
		{
			auto& selectedF = frameData.frames[selectedFrame];

			ImGui::SliderFloat("TimeFrame", &selectedF.time, 0.f, frameData.duration);

			if (ImGui::Button("Add New Frame"))
			{
				selectedF.changes.push_back(HitboxChange());
				
			}
			if (ImGui::Button("Sort"))
			{
				std::sort(selectedF.changes.begin(), selectedF.changes.end(), [](HitboxChange& f1, HitboxChange& f2)
				{
					return f1.index < f2.index;
				});
			}

			for (size_t i = 0; i < selectedF.changes.size(); ++i)
			{
				bool disp = i == selectedChange;
				if (ImGui::Selectable(std::to_string(i).c_str(), &disp))
				{
					selectedChange = i;
					selectedHitbox = selectedF.changes[i].index;
				}
			}
		}

		ImGui::End();
	}

	if (ImGui::Begin("Change"))
	{
		if (selectedFrame < frameData.frames.size())
		{
			auto& selectedF = frameData.frames[selectedFrame];
			if (selectedChange < selectedF.changes.size())
			{
				auto& selectedC = selectedF.changes[selectedChange];

				if (ImGui::SliderInt("Index", &selectedC.index, 0, frameData.amountOfHitboxes - 1))
				{
					if (selectedC.index > frameData.amountOfHitboxes)
					{
						frameData.amountOfHitboxes = selectedC.index;
					}
				}

				ImGui::InputFloat2("Position", (float*)&selectedC.position);
				ImGui::Checkbox("IsActive", &selectedC.isActive);
				ImGui::Checkbox("IsHitbox (or hurtbox)", &selectedC.isHitbox);
				ImGui::InputInt("HitNumber", &selectedC.hitNumber);
				ImGui::InputFloat("Width", &selectedC.width);
				ImGui::InputFloat("Height", &selectedC.height);
				
				ImGui::InputInt("RelativePriority", &selectedC.relativePriority);
				ImGui::InputInt("AbsolutePriority", &selectedC.absolutePriority);
				
				ImGui::Separator();
				ImGui::InputFloat("Damage", &selectedC.damage);
				ImGui::InputFloat("KnockbackAngle", &selectedC.knockbackAngle);
				ImGui::InputFloat("KnockbackPowerBase", &selectedC.knockbackPowerBase);
				ImGui::InputFloat("KnockbackPowerGrowth", &selectedC.knockbackPowerGrowth);
				ImGui::InputFloat("HitstunAdjustment", &selectedC.hitstunAdjustment);
			}
		}

		ImGui::End();
	}



	render(window, frameData, time);
}
