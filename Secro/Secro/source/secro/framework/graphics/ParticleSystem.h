#pragma once
#include "Particle.h"
#include <vector>
#include <optional>

namespace secro {
	class RawSerializeBuffer;

	class ParticleSystem {
	public:
		ParticleSystem();

		//update the particles
		void update(float deltaTime);

		//render all of the particles
		void renderFront(sf::RenderWindow& window);
		void renderBack(sf::RenderWindow& window);

		//spawn a particle in the world
		Particle& spawnParticle();

		void netSerSave(RawSerializeBuffer& buff);
		void netSerLoad(RawSerializeBuffer& buff);

	private:
		void garbageCollect();

	private:
		std::vector<std::optional<Particle>> particles;

		Particle NullPart;
	};
}