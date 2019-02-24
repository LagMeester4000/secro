#pragma once
#include "UITransform.h"
#include "UIResult.h"
#include <memory>
#include <vector>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class Controller;
	class UIElement;
	class UIManager;

	struct UIContext {
		UIManager* manager;
		UIElement* frame;
	};

	//base class for any UI element
	class UIElement {
	public:
		//returns if the element is active or not
		virtual bool isActive();

		//update various other things
		virtual void update(float deltaTime);

		//called if this element should receive input
		virtual UIResult processInput(const Controller& controller, UIContext context);

		//render the UI (if needed)
		virtual void render(UITransform &transform, sf::RenderWindow& window);

		//get the transform of the element
		const UITransform& getTransform();

		//get var
		bool isVisableWhenNotOnTop();

		//add a UIElement as a child
		void addChild(std::shared_ptr<UIElement> newElement);

		//get a child from the children
		std::shared_ptr<UIElement> getChild(size_t index);
	
	private:
		std::vector<std::shared_ptr<UIElement>> children;

	public:
		bool visableWhenNotOnTop;
		bool visable;

		UITransform transform;
	};
}