#include "Particle.h"
#include <SFML/Graphics.hpp>

secro::Particle::Particle()
{
}

void secro::Particle::update(float deltaTime)
{
	animation.update(sf::seconds(deltaTime));
}

void secro::Particle::render(sf::RenderWindow & window)
{
	window.draw(animation);
}

bool secro::Particle::canBeDeleted()
{
	return !animation.isPlaying();
}
