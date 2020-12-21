#pragma once

#include "Globals.h"

#include "Math/myassert.h"

template<typename T>
class Pool
{
public:
	~Pool()
	{
		Clear();
	}

	void Allocate(unsigned amount)
	{
		Clear();

		// Allocate
		size = amount;
		count = 0;
		data = new T[amount];
		next_free = new T*[amount];
		first_free = data;

		// Initialize free list
		for (size_t i = 0; i < amount; ++i)
		{
			next_free[i] = data + (i + 1);
		}
	}

	void Clear()
	{
		size = 0;
		count = 0;
		RELEASE_ARRAY(data);
		RELEASE_ARRAY(next_free);
		first_free = nullptr;
	}

	T* Obtain()
	{
		assert(first_free != nullptr);

		// Check for pool overflow
		if (first_free == data + size)
		{
			return nullptr;
		}

		// Obtain a new object
		T* object = first_free;
		size_t index = object - data;
		first_free = next_free[index];
		next_free[index] = nullptr;

		return object;
	}

	void Release(T* object)
	{
		assert(object >= data && object < data + size);

		// Release the object
		size_t index = object - data;
		next_free[index] = first_free;
		first_free = object;
	}

	size_t Count()
	{
		return count;
	}

	// Iteration

	class Iterator
	{
	public:
		Iterator(const Pool<T>& pool__, size_t index__)
			: pool(&pool__)
			, index(index__) {}

		const Iterator& operator++()
		{
			index += 1;
			while (index < pool->size && pool->next_free[index] != nullptr)
			{
				index += 1;
			}
			return *this;
		}

		bool operator!=(const Iterator& other) const
		{
			return index != other.index;
		}

		T& operator*() const
		{
			return pool->data[index];
		}

	private:
		const Pool<T>* pool;
		size_t index;
	};

	typename Pool<T>::Iterator begin()
	{
		size_t index = 0;
		while (index < size && next_free[index] != nullptr)
		{
			index += 1;
		}
		return Pool<T>::Iterator(*this, index);
	}

	typename Pool<const T>::Iterator begin() const
	{
		size_t index = 0;
		while (index < size && next_free[index] != nullptr)
		{
			index += 1;
		}
		return Pool<const T>::Iterator((Pool<const T>&) *this, index);
	}

	typename Pool<T>::Iterator end()
	{
		return Pool<T>::Iterator(*this, size);
	}

	typename Pool<const T>::Iterator end() const
	{
		return Pool<const T>::Iterator((Pool<const T>&) *this, size);
	}

private:
	size_t size = 0; // Max number of objects in the pool.
	size_t count = 0; // Current number of objects in the pool.
	T* data = nullptr; // Data storage.
	T** next_free = nullptr; // Linked list of free objects. Null pointers mean that the object isn't free. Last item points to the end of the data.
	T* first_free = nullptr; // First free object in the linked list.
};
