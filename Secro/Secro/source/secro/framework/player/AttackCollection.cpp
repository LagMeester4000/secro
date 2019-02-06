#include "AttackCollection.h"
#include <fstream>
#include <cereal/archives/json.hpp>
#include <experimental/filesystem>
#include <iostream>

using namespace secro;

secro::AttackCollection::AttackCollection()
{
}

void secro::AttackCollection::loadAttack(std::string fileName, PlayerState attack)
{
	if (std::experimental::filesystem::exists(fileName))
	{
		auto& spot = getAttack(attack);
		spot = FrameData();

		std::ifstream file(fileName);
		cereal::JSONInputArchive ar(file);
		ar(spot);
	}
	else
	{
		std::cout << "could not load '" << fileName << "'" << std::endl;
	}
}

FrameData secro::AttackCollection::loadRaw(std::string fileName)
{
	if (std::experimental::filesystem::exists(fileName))
	{
		FrameData spot;
		std::ifstream file(fileName);
		cereal::JSONInputArchive ar(file);
		ar(spot);
		return spot;
	}
	else
	{
		std::cout << "could not load '" << fileName << "'" << std::endl;
	}
	return FrameData();
}

FrameData & secro::AttackCollection::getAttack(PlayerState attack)
{
	return attacks[findIndex(attack)];
}

int secro::AttackCollection::findIndex(PlayerState state)
{
	return (int)state - (int)PlayerState::AFirstAttack - 1;
}
