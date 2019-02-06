#pragma once
#include <memory>
#include <functional>

namespace secro {
	struct Joystick {
		float x, y;
	};

	enum class Direction {
		Left, 
		Right,
		Up,
		Down,
		Neutral,
	};

	struct DirectionPress {
		Direction direction;
		bool hardPush;
		float speed;
	};
	
	class Controller {
		const static int buffer = 8;

	public:
		struct Input {
			Joystick leftStick = { 0.f, 0.f };
			Joystick rightStick = { 0.f, 0.f };

			bool aButton = false;
			bool bButton = false;
			bool xButton = false;
			bool yButton = false;

			bool lTrigger1 = false;
			bool lTrigger2 = false;

			bool rTrigger1 = false;
			bool rTrigger2 = false;

			bool start = false;
			bool select = false;
		};

	public:
		static std::shared_ptr<Controller> createController(int index, bool keyboard = false);

		//insert a fake controller
		void interceptController(std::function<void(Input&)> function);

		//read inputs into the controller
		void update();

		//movement
		const Joystick& getMovement() const;
		Direction getMovementDirection() const;
		Direction getMovementPushDirection() const;
		DirectionPress getMovementPushDirectionExt() const;

		//attack with the stick
		const Joystick& getDirAttack() const;
		Direction getDirAttackDirection() const;

		//attack
		bool attackPressed() const;
		bool attackHeld() const;
		bool attackReleased() const;

		//special
		bool specialPressed() const;
		bool specialHeld() const;
		bool specialReleased() const;

		//block
		bool blockPressed() const;
		bool blockHeld() const;
		bool blockReleased() const;

		//grab
		bool grabPressed() const;
		bool grabHeld() const;
		bool grabReleased() const;

		//jump
		bool jumpPressed() const;
		bool jumpHeld() const;
		bool jumpReleased() const;

	private:
		Controller(int index, bool keyboard);

		//swap the input buffer to the back to make place for a new frame
		void swapBack();

		//get the current input
		Input& current();
		const Input& current() const;

		//get one of the previous inputs
		Input& prev(size_t index);
		const Input& prev(size_t index) const;

		//get the previous input
		Input& prev();
		const Input& prev() const;

	public:
		//get the direction of a joystick
		Direction getDirection(const Joystick& stick) const;

		const Input& getInput(size_t index);

	private:
		int controllerIndex;
		bool useKeyboard;
		float deadzone = 20.f;

	private:
		Input inputs[buffer];

	private:
		//function to interupt controller pulling
		std::function<void(Input&)> interceptFunction;
		bool useIntercept = false;
	};
}