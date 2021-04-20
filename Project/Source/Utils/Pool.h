#pragma once

#include "Globals.h"

#include "Math/myassert.h"

template<typename T>
class Pool {
public:
	~Pool() {
		Deallocate();
	}

	void Allocate(unsigned amount) {
		Deallocate();

		// Allocate
		capacity = amount;
		count = 0;
		data = (T*) ::operator new(amount * sizeof(T));
		nextFree = (T**) ::operator new(amount * sizeof(T*));
		firstFree = data;

		// Initialize free list
		for (size_t i = 0; i < amount; ++i) {
			nextFree[i] = data + (i + 1);
		}
	}

	void Deallocate() {
		capacity = 0;
		count = 0;
		if (data != nullptr) {
			::operator delete(data);
			data = nullptr;
		}
		if (nextFree != nullptr) {
			::operator delete(nextFree);
			nextFree = nullptr;
		}
		firstFree = nullptr;
	}

	template<typename... Args>
	T* Obtain(Args&&... args) {
		assert(firstFree != nullptr); // ERROR: The pool hasn't been initialized

		assert(firstFree != data + capacity); // ERROR: Pool overflow

		// Obtain a new object
		T* object = new (firstFree) T(std::forward<Args>(args)...);
		size_t index = object - data;
		firstFree = nextFree[index];
		nextFree[index] = nullptr;
		count += 1;

		return object;
	}

	void Release(T* object) {
		assert(object >= data && object < data + capacity); // ERROR: The object is not in the data array

		size_t index = object - data;

		assert(nextFree[index] == nullptr); // ERROR: The object is already free

		// Release the object
		object->~T();
		nextFree[index] = firstFree;
		firstFree = object;
		count -= 1;
	}

	void Clear() {
		// Reset count and free list
		count = 0;
		firstFree = data;

		// Initialize free list
		for (size_t i = 0; i < capacity; ++i) {
			nextFree[i] = data + (i + 1);
		}
	}

	size_t Capacity() {
		return capacity;
	}

	size_t Count() {
		return count;
	}

	// Iteration

	class Iterator {
	public:
		Iterator(const Pool<T>& pool__, size_t index__)
			: pool(&pool__)
			, index(index__) {}

		const Iterator& operator++() {
			index += 1;
			while (index < pool->capacity && pool->nextFree[index] != nullptr) {
				index += 1;
			}
			return *this;
		}

		bool operator!=(const Iterator& other) const {
			return index != other.index;
		}

		T& operator*() const {
			return pool->data[index];
		}

	private:
		const Pool<T>* pool;
		size_t index;
	};

	typename Pool<T>::Iterator begin() {
		size_t index = 0;
		while (index < capacity && nextFree[index] != nullptr) {
			index += 1;
		}
		return Pool<T>::Iterator(*this, index);
	}

	typename Pool<const T>::Iterator begin() const {
		size_t index = 0;
		while (index < capacity && nextFree[index] != nullptr) {
			index += 1;
		}
		return Pool<const T>::Iterator((Pool<const T>&) *this, index);
	}

	typename Pool<T>::Iterator end() {
		return Pool<T>::Iterator(*this, capacity);
	}

	typename Pool<const T>::Iterator end() const {
		return Pool<const T>::Iterator((Pool<const T>&) *this, capacity);
	}

private:
	size_t capacity = 0;	// Max number of objects in the pool.
	size_t count = 0;		// Current number of objects in the pool.
	T* data = nullptr;		// Data storage.
	T** nextFree = nullptr; // Linked list of free objects. Null pointers mean that the object isn't free. Last item points to the end of the data.
	T* firstFree = nullptr; // First free object in the linked list.
};
