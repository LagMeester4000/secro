#pragma once

namespace secro {
	class CommandArgs {
	public:
		CommandArgs(int argc, char* argv[]);

		bool hasArg(const char* name);

	private:
		int argc;
		char** argv;
	};
}