#include "PlayerManager.h"
#include "../framework/player/PlayerCharacter.h"

using namespace secro;

secro::PlayerManager::PlayerManager()
{
}

void secro::PlayerManager::setStageSize(float width, float height)
{
	stageWidth = width;
	stageHeight = height;
}

void secro::PlayerManager::addPlayer(PlayerCharacter * player)
{
	player->init();
	PlayerRecord push;
	push.player = std::shared_ptr<PlayerCharacter>(player);
	push.stocks = rules.stocks;
	players.push_back(push);
}

PlayerCharacter * secro::PlayerManager::getPlayer(int index)
{
	return &*players[index].player;
}

void secro::PlayerManager::update(float deltaTime)
{
	for (auto& it : players)
	{
		it.player->update(deltaTime);

		//death check
		auto pos = it.player->getPosition();
		if (pos.x < -stageWidth || pos.x > stageWidth || pos.y < -stageHeight || pos.y > stageHeight)
		{
			it.stocks--;
			
			if (it.stocks > 0)
				it.player->reset({ 0.f, 0.f });
		}

		if (it.stocks <= 0)
		{
			it.player->freeze();
		}
	}
}

void secro::PlayerManager::render(sf::RenderWindow & window)
{
	for (auto& it : players)
	{
		it.player->render(window);
	}
}

size_t secro::PlayerManager::size()
{
	return players.size();
}

const typename PlayerManager::PlayerRecord & secro::PlayerManager::operator[](size_t index)
{
	return players[index];
}

bool secro::PlayerManager::isGameDone()
{
	for (auto& it : players)
	{
		if (it.stocks <= 0)
			return true;
	}
	return false;
}

int secro::PlayerManager::getWinner()
{
	for (size_t i = 0; i < players.size(); ++i)
	{
		if (players[i].stocks <= 0)
			return i;
	}
	return -1;
}

void secro::PlayerManager::resetGame()
{
	for (auto& it : players)
	{
		it.player->reset({ 0.f, 0.f });
		it.stocks = rules.stocks;
	}
}
