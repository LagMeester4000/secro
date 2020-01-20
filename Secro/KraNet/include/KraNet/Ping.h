#pragma once
#include "Detail/RingBufferStatic.h"

namespace kra {
	namespace net {
		class Ping {
		public:
			Ping();

			// Register a new ping
			void Add(double NewPing);

			// Get the average ping over the last x frames
			double Get() const;

		private:
			RingBufferStatic<double, (size_t)10> Pings;
		};
	}
}