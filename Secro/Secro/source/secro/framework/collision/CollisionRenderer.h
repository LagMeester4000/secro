#pragma once
#include "FrameData.h"

namespace sf {
	class RenderWindow;
}

namespace secro {
	class CollisionRenderer {
	public:
		CollisionRenderer();

		void render(sf::RenderWindow& window, FrameData& framedata, float time, b2Vec2 position, float scale = 1.f);

		static void render(sf::RenderWindow& window, HitboxChange& change, bool isSelected = false, b2Vec2 scale = { 1.f, 1.f });
		void render(sf::RenderWindow& window, FrameData& framedata, float time);

		void renderFrameDataEditor(sf::RenderWindow& window);

	private:
		FrameData frameData;

		float time = 0.f;
		int selectedFrame;
		int selectedChange;
		int selectedHitbox;
	};
}