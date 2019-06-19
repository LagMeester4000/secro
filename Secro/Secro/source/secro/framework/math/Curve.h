#pragma once
#include <variant>

namespace secro {
	struct Linear {
		float calculate(float i);
		void render();
	};

	struct Exponent {
		float calculate(float i);
		void render();

		float exponent = 2.f;
	};

	struct Division {
		float calculate(float i);
		void render();
	};

	struct Circle {
		float calculate(float i);
		void render();

		float exponent = 2.f;
	};
	
	class Curve {
	public:
		Curve();

		//calculate the value from the curve
		float calculate(float input);

		//set the formula to a new function object
		template<typename T>
		void setFormula(T formulaObj)
		{
			formula = formulaObj;
		}

		//draw the curve editor
		void renderCurveEditor(int precision = 100);

	private:
		//variant with curve types
		std::variant<Linear, Exponent, Division, Circle> formula;

	public:
		//value to add to the value gotten from the formula
		float resultAdd = 0.f;

		//value to multiply the value gotte from the formula by
		float resultMultiplicant = 1.f;

		//value to add to the input
		float inputAdd = 0.f;

		//value to multiply the input by
		float inputMultiplicant = 1.f;

	private: //editor values
		const char* currentFormula = "";
	};
}