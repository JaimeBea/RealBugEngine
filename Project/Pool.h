#pragma once

#include <vector>
#include <unordered_map>

// Map optimized for iteration.
// Add: O(1) best case
// Remove: O(1) best case
// Access: O(1) best case
// Iteration: O(n) with contiguous memory
template<typename T>
class Pool
{
public:
	void Reserve(unsigned amount)
	{
		id_to_index_map.reserve(amount);
		index_to_id_map.reserve(amount);
		objects.reserve(amount);
	}

	unsigned Offer(T&& object)
	{
		unsigned id = next_id;
		size_t index = objects.size();

		id_to_index_map[id] = index;
		index_to_id_map[index] = id;
		objects.push_back(object);

		next_id += 1;

		return id;
	}

	void Remove(unsigned id)
	{
		size_t index = id_to_index_map[id];
		size_t last_index = objects.size() - 1;
		unsigned last_id = index_to_id_map[last_index];

		id_to_index_map[last_id] = index;
		index_to_id_map[index] = last_id;
		std::swap(objects[index], objects[last_index]);

		id_to_index_map.erase(id);
		index_to_id_map.erase(last_index);
		objects.erase(objects.end() - 1);
	}

	void Clear()
	{
		next_id = 0;
		id_to_index_map.clear();
		index_to_id_map.clear();
		objects.clear();
	}

	T* Data()
	{
		return objects.data();
	}

	const T* Data() const
	{
		return objects.data();
	}

	T& operator[](unsigned id)
	{
		unsigned index = id_to_index_map[index];
		return objects[index];
	}

	const T& operator[](unsigned id) const
	{
		unsigned index = id_to_index_map[index];
		return objects[index];
	}

	// Iteration

	typename std::vector<T>::iterator begin()
	{
		return objects.begin();
	}

	typename std::vector<T>::const_iterator begin() const
	{
		return objects.begin();
	}

	typename std::vector<T>::iterator end()
	{
		return objects.end();
	}

	typename std::vector<T>::const_iterator end() const
	{
		return objects.end();
	}

private:
	unsigned next_id = 0;
	std::unordered_map<unsigned, size_t> id_to_index_map;
	std::unordered_map<size_t, unsigned> index_to_id_map;
	std::vector<T> objects;
};
