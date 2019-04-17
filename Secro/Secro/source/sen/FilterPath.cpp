#include "FilterPath.h"
#include <algorithm>

using namespace sen;

sen::FilterPath::FilterPath()
{
	path = "";
}

sen::FilterPath::FilterPath(std::string fullPath)
{
	path = fullPath;
}

FilterPath & sen::FilterPath::operator=(const FilterPath & other)
{
	path = other.path;
	return *this;
}

FilterPath & sen::FilterPath::operator=(const std::string & other)
{
	path = other;
	return *this;
}

void sen::FilterPath::setFullPath(std::string fullPath)
{
	path = fullPath;
}

void sen::FilterPath::addFolder(std::string folder)
{
	path += folder;
	path += "/";
}

void sen::FilterPath::popFolder()
{
	int first = 0;
	int find = -1;
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (path[i] == '/')
		{
			if (find != -1)
			{
				first = find;
			}

			find = i;
		}
	}
	
	if (find != -1)
	{
		path.erase(first, path.size() - find);
	}
}

void sen::FilterPath::empty()
{
	path = "";
}

std::string_view sen::FilterPath::operator[](size_t index)
{
	return getFolder(index);
}

std::string_view sen::FilterPath::getFolder(size_t index)
{
	int first = 0;
	int find = -1;
	int found = 0;
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (path[i] == '/')
		{
			if (find != -1)
			{
				first = find;
			}

			find = i;

			if (found == index)
			{
				break;
			}

			++found;
		}
	}

	return std::string_view(&path[first], path.length() - find);
}
