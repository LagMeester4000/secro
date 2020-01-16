#pragma once
#include <new>
#include <utility>
#include <stdint.h>
#include <assert.h>

namespace secro {
	template<typename Base, typename T>
	void copyFunc(const Base& toCopy, void* memPos)
	{
		new(memPos) T((const T&)toCopy);
	}
	
	template<typename Base, typename T>
	void moveFunc(Base&& toCopy, void* memPos)
	{
		new(memPos) T(std::move((T&&)toCopy));
	}

	//memory pool that holds objects from a virtual base class
	//restrictions for the classes stored in this
	template<typename T>
	class VirtualObjectPool {
	public:
		using BaseClass = T;
		struct MetaElement {
			//relative pointer (from objectMemory) to an element
			//if nullptr, element is destroyed
			BaseClass* element = nullptr;

			//current generation of this object
			//if new object is created in this slot, generation goes up by 1
			//used to check if handle is valid
			uint32_t generation = 0;

			//functions used to copy and move memory of the object
			void(*copy)(const BaseClass&, void*);
			void(*move)(BaseClass&&, void*);
		};
		struct FreeSpaceHeader {
			//size of the current memory block, including the size of the header
			size_t size = 0;
			
			//distance in bytes to the next free space
			size_t next = 0;
		};
		struct Handle {
			uint32_t index = 0; //might change to size_t
			uint32_t generation = UINT32_MAX;
		};

	public:
		VirtualObjectPool(size_t sizeOfMemory, size_t amountOfElements);
		~VirtualObjectPool();
		VirtualObjectPool(const VirtualObjectPool<T>& other);
		VirtualObjectPool<T>& operator=(const VirtualObjectPool<T>& other);
		VirtualObjectPool(VirtualObjectPool<T>&& other);
		VirtualObjectPool<T>& operator=(VirtualObjectPool<T>&& other);

		//add an object to the pool
		template<typename U>
		Handle add();

		//remove an object from the pool
		void remove(const Handle& handle);

		//returns nullptr if handle is invalid
		template<typename U>
		U* get(const Handle& handle);

	private:
		//allocate the memory for the pool
		void allocateMemory(size_t memorySize);

		//initialize the memory and setup the pointers
		void initMemory(size_t memorySize, size_t elementSize);

		//defragment the object pool
		void defragment();

	private:
		std::pair<FreeSpaceHeader*, size_t> findFreeMemory(size_t neededSize, FreeSpaceHeader* listElement);
		void* allocateObjectMemory(size_t neededSize);

	private:
		//memory pool
		uint8_t* memory = nullptr;
		size_t memorySize = 0;

		//meta element list (part of memory array)
		MetaElement* metaElements = nullptr;
		size_t metaElementsSize = 0;

		//object memory
		uint8_t* objectMemory = nullptr;
		size_t objectMemorySize = 0;

		//free space linked list
		FreeSpaceHeader* freeSpaceList = nullptr;
	};

	template<typename T>
	inline VirtualObjectPool<T>::VirtualObjectPool(size_t sizeOfMemory, size_t amountOfElements)
	{
		initMemory(sizeOfMemory, amountOfElements);
	}

	template<typename T>
	inline VirtualObjectPool<T>::~VirtualObjectPool()
	{
		//if pointer is valid, destroy memory
		if (memory)
		{
			::operator delete((void*)memory);
		}
	}

	template<typename T>
	inline VirtualObjectPool<T>::VirtualObjectPool(const VirtualObjectPool<T>& other)
	{
	}

	template<typename T>
	inline VirtualObjectPool<T>& VirtualObjectPool<T>::operator=(const VirtualObjectPool<T>& other)
	{
		// TODO: insert return statement here
	}

	template<typename T>
	inline VirtualObjectPool<T>::VirtualObjectPool(VirtualObjectPool<T>&& other)
	{
	}

	template<typename T>
	inline VirtualObjectPool<T>& VirtualObjectPool<T>::operator=(VirtualObjectPool<T>&& other)
	{
		// TODO: insert return statement here
	}

	template<typename T>
	inline void VirtualObjectPool<T>::remove(const Handle & handle)
	{
	}

	template<typename T>
	inline void VirtualObjectPool<T>::allocateMemory(size_t memorySize)
	{
		//make sure memory is not created
		assert(!memory);

		//allocate
		memory = (uint8_t*)::operator new(memorySize);
		this->memorySize = memorySize;
	}

	template<typename T>
	inline void VirtualObjectPool<T>::initMemory(size_t memorySize, size_t elementSize)
	{
		allocateMemory(memorySize);

		//meta elements at position 0
		metaElements = (MetaElement*)memory;
		metaElementsSize = elementSize;
		//set all data to 0
		std::memset((void*)metaElements, 0, elementSize * sizeof(MetaElement));

		//objects after meta elements
		objectMemory = (uint8_t*)metaElements + metaElementsSize;
		objectMemorySize = memorySize - metaElementsSize * sizeof(MetaElement);

		//initialize free object list at the start of object memory
		freeSpaceList = (FreeSpaceHeader*)objectMemory;
		freeSpaceList->next = 0;
		freeSpaceList->size = objectMemorySize;
	}

	template<typename T>
	inline void VirtualObjectPool<T>::defragment()
	{
		//TODO
	}

	template<typename T>
	inline std::pair<typename VirtualObjectPool<T>::FreeSpaceHeader*, size_t> VirtualObjectPool<T>::findFreeMemory(size_t neededSize, FreeSpaceHeader * listElement)
	{
		//if (listElement->size >= neededSize)
		//{
		//	return { listElement, 0 };
		//}
		//else 
		//{
		//	FreeSpaceHeader* next = (FreeSpaceHeader*)(objectMemory + listElement->next);
		//	auto toReturn = findFreeMemory(neededSize, next);
		//	if (toReturn.first == next)
		//	{
		//		//update this lists next value and possibly make a new node
		//
		//		listElement->next = next->next;
		//
		//		//check if next element 
		//
		//		listElement->size += next->size
		//	}
		//}
		//
		//return nullptr;
		return nullptr;
	}

	template<typename T>
	inline void * VirtualObjectPool<T>::allocateObjectMemory(size_t neededSize)
	{
		FreeSpaceHeader* prev = nullptr;
		FreeSpaceHeader* it = freeSpaceList;
		while (it)
		{
			


			prev = it;
			if (it->next)
				it = (FreeSpaceHeader*)(objectMemory + it->next);
			else
				it = nullptr;
		}

		return nullptr;
	}

	template<typename T>
	template<typename U>
	inline VirtualObjectPool<T>::Handle VirtualObjectPool<T>::add()
	{
		return Handle();
	}

	template<typename T>
	template<typename U>
	inline U * VirtualObjectPool<T>::get(const Handle & handle)
	{
		return nullptr;
	}
}