#pragma once

namespace sen {
	class SerializerSave {
	public:
		template<typename ... TT>
		void operator()(TT... args)
		{
			int ex[] = { execute(args)... };
		}

	private:
		template<typename T>
		int execute(T& val)
		{

			return 0;
		}
	};

	class SerializerLoad {
	public:
		template<typename ... TT>
		void operator()(TT... args)
		{
			int ex[] = { execute(args)... };
		}

	private:
		template<typename T>
		int execute(T& val)
		{

			return 0;
		}
	};
}