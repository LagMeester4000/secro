#include "CommandArgs.h"
#include <cstring>

secro::CommandArgs::CommandArgs(int vargc, char * vargv[])
	: argc(vargc), argv(vargv)
{
}

bool secro::CommandArgs::hasArg(const char * name)
{
	if (argc < 2)
		return false;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], name) == 0)
		{
			return true;
		}
	}

	return false;
}
