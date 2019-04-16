#pragma once
#include <string>
#include "Meta.h"

namespace sen {
	template<typename T, typename ... Atts>
	struct MemberRef {
		T& ref;
		const std::string name;
		
		using Attributes = TypeList<Atts...>;

		template<typename T>
		void serialize(T& t)
		{
			t.member(name, ref);
		}
	};

	template<typename T, typename ... Atts>
	MemberRef<T, Atts...> makeMemberRef(T& ref, const std::string name)
	{
		return MemberRef<T, Atts...>{ ref, name };
	}
}

#define MEMBER(Member) sen::makeMemberRef(Member, #Member)
#define MEMBER_A(Member, ...) sen::makeMemberRef<decltype(Member), __VA_ARGS__>(Member, #Member)