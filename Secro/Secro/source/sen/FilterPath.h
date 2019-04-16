#pragma once
#include <string>
#include <string_view>
#include "MemberRef.h"

namespace sen {
	class FilterPath {
	public:
		FilterPath();
		FilterPath(std::string fullPath);
		FilterPath& operator=(const FilterPath& other);
		FilterPath& operator=(const std::string& other);

		void setFullPath(std::string fullPath);
		void addFolder(std::string folder);
		void popFolder();
		void empty();

		std::string_view operator[](size_t index);
		std::string_view getFolder(size_t index);

		template<typename T>
		void serialize(T& t)
		{
			t(MEMBER(path));
		}

	private:
		std::string path;
	};
}