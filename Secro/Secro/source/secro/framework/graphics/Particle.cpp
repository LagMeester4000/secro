#include "Particle.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include <SFML/Graphics.hpp>

secro::Particle::Particle()
{
}

void secro::Particle::update(float deltaTime)
{
	animation.update(sf::seconds(deltaTime));
	
	//update vals
	opacity += opacityOverTime * deltaTime;
	auto normScale = normalise(scale);
	scale += normScale * scaleOverTime * deltaTime;

	//udpate opacity
	auto color = animation.getColor();
	float newOpac = opacity;
	if (newOpac > 100.f)
		newOpac = 100.f;
	else if (newOpac < 0.f)
		newOpac = 0.f;
	color.a = 255 * (newOpac / 100.f);
	animation.setColor(color);

	//update scale
	animation.setScale(scale);
}

void secro::Particle::render(sf::RenderWindow & window)
{
	window.draw(animation);
}

bool secro::Particle::canBeDeleted()
{
	auto color = animation.getColor();
	return (!animation.isPlaying() && useAnimation) || opacity < 0.f || opacity > 100.f;
}
