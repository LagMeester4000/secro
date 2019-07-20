#pragma once

namespace secro {
	using HandleT = int;

	//typed handle class
	//an object of this class can be passed to a manager to retrieve an object of type T
	//this class is templated so that it removes any chance of passing the wrong handle to a manager
	//if you try to pass a hitbox handle to the physics manager, it will give a compile error
	template<typename T>
	class Handle {
	public:
		Handle() {}
		Handle(HandleT val) : handle(val) {}

		void setIndex(HandleT newHandle)
		{
			handle = newHandle;
		}

		HandleT getIndex() const
		{
			return handle;
		}

		bool isValid() const
		{
			return handle != -1;
		}

		void makeInvalid()
		{
			handle = -1;
		}

	private:
		HandleT handle = -1;
	};

}