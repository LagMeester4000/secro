#pragma once
#include <string>
#include <string_view>

namespace sen {
	class FilterPath {
	public:
		FilterPath();
		FilterPath(std::string fullPath);

		void setFullPath(std::string fullPath);
		void addFolder(std::string folder);
		void popFolder();
		void empty();

		std::string_view operator[](size_t index);
		std::string_view getFolder(size_t index);

	private:
		std::string path;
	};
}