#include "ParticleSystem.h"
#include "secro/netplay/RawSerializeBuffer.h"

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
	// DISABLE PARTICLES
	return NullPart;

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

void secro::ParticleSystem::netSerSave(RawSerializeBuffer & buff)
{
	size_t size = particles.size();
	buff.save(size);
	for (size_t i = 0; i < size; ++i)
	{
		buff.save(particles[i]);
	}
}

void secro::ParticleSystem::netSerLoad(RawSerializeBuffer & buff)
{
	std::optional<Particle> load;
	size_t size;
	buff.load(size);
	particles.clear();
	for (size_t i = 0; i < size; ++i)
	{
		buff.load(load);
		particles.push_back(load);
	}
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
