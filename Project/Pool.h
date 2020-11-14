#pragma once

#include <vector>
#include <unordered_map>

template<typename T>
class Pool
{
public:
	void Reserve(unsigned amount)
	{
		id_to_index_map.reserve(amount);
		objects.reserve(amount);
	}

	unsigned Offer(T&& object)
	{
		unsigned id = next_id;
		size_t index = objects.size();

		id_to_index_map[id] = index;
		objects.push_back(object);

		next_id += 1;

		return id;
	}

	void Remove(unsigned id)
	{
		size_t index = id_to_index_map[id];
		size_t last_index = objects.size() - 1;

		std::swap(objects[index], objects[last_index]);

		id_to_index_map.erase(id);
		objects.erase(objects.end() - 1);
	}

	void Clear()
	{
		next_id = 0;
		id_to_index_map.clear();
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
	std::vector<T> objects;
};
