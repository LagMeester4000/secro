#pragma once

namespace sen {
	class IUpdatable {
	public:
		virtual ~IUpdatable() {}
		virtual void setup() {}
		virtual void start() {}
		virtual void update() {}
		virtual void inspect() {}
	};
}