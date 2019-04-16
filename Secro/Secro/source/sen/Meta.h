#pragma once

namespace sen {
	template<typename ... Other>
	constexpr bool HasType = false;
	template<typename T>
	constexpr bool HasType<T> = false;
	template<typename T, typename ... Other>
	constexpr bool HasType<T, T, Other...> = true;
	template<typename T, typename First, typename ... Other>
	constexpr bool HasType<T, First, Other...> = HasType<T, Other...>;

	template<typename ... TT>
	struct TypeList {
		template<typename T>
		static bool hasType()
		{
			return HasType<T, TT...>;
		}
	};
}