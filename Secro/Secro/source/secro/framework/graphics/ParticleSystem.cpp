#include "ParticleSystem.h"

using namespace secro;

secro::ParticleSystem::ParticleSystem()
{
}

void secro::ParticleSystem::update(float deltaTime)
{
	for (auto& it : particles)
	{
		if (it.has_value())
		{
			it->update(deltaTime);
		}
	}

	garbageCollect();
}

void secro::ParticleSystem::renderFront(sf::RenderWindow & window)
{
	for (auto& it : particles)
	{
		if (it.has_value())
		{
			if (it->inFrontOfCharacter)
				it->render(window);
		}
	}
}

void secro::ParticleSystem::renderBack(sf::RenderWindow & window)
{
	for (auto& it : particles)
	{
		if (it.has_value())
		{
			if (!it->inFrontOfCharacter)
				it->render(window);
		}
	}
}

Particle & secro::ParticleSystem::spawnParticle()
{
	for (auto& it : particles)
	{
		if (!it.has_value())
		{
			it.emplace();
			return *it;
		}
	}
	particles.emplace_back();
	auto& ret = particles[particles.size() - 1];
	ret.emplace();
	return *ret;
}

void secro::ParticleSystem::garbageCollect()
{
	for (auto& it : particles)
	{
		if (it.has_value())
		{
			if (it->canBeDeleted())
			{
				it.reset();
			}
		}
	}
}
