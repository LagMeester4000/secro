#pragma once

namespace sen {
	class Inspector {
	public:
		template<typename ... TT>
		void operator()(TT&& ...) {}
	};
}