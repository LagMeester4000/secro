#pragma once
#include <memory>

namespace secro {
	class UIFrame;

	class UIResult {
	public:
		enum Change {
			Null,
			Back,
			AddFrame,
		};

	public:
		UIResult(Change change);
		UIResult();

	public:
		Change change;
	};
}