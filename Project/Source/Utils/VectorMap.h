#pragma once

#include <vector>
#include <unordered_map>

/*
* Map that saves values in a densely populated array backed by two hash tables. Allows cache-friendly O(n) iteration and fast (O(1)) access and removal.
* ADVANTAGES:
* - Much faster cache-friendly iteration.
* DISADVANTAGES:
* - Pointers are not stable.
* - Increased memory usage.
*/
template<typename K, typename V>
class VectorMap {
public:
	bool Has(K key) const {
		return keyToIndexMap.find(key) != keyToIndexMap.end();
	}

	V& Get(K key) {
		return values.at(keyToIndexMap.at(key));
	}

	V& Put(K key, const V& value) {
		keyToIndexMap[key] = values.size();
		indexToKeyMap[values.size()] = key;
		values.push_back(value);
		return values.back();
	}

	V& Put(K key, V&& value) {
		keyToIndexMap[key] = values.size();
		indexToKeyMap[values.size()] = key;
		values.push_back(value);
		return values.back();
	}

	void Remove(K key) {
		unsigned index = keyToIndexMap.at(key);
		unsigned lastIndex = values.size() - 1;
		K& lastKey = indexToKeyMap[lastIndex];
		keyToIndexMap[lastKey] = index;
		indexToKeyMap[index] = lastKey;
		keyToIndexMap.erase(key);
		indexToKeyMap.erase(lastIndex);
		std::swap(values[index], values[lastIndex]);
		values.pop_back();
	}

	// Iteration

	typename std::vector<V>::iterator begin() {
		return values.begin();
	}

	typename std::vector<V>::const_iterator begin() const {
		return values.begin();
	}

	typename std::vector<V>::iterator end() {
		return values.end();
	}

	typename std::vector<V>::const_iterator end() const {
		return values.end();
	}

private:
	std::vector<V> values;
	std::unordered_map<K, unsigned> keyToIndexMap;
	std::unordered_map<unsigned, K> indexToKeyMap;
};
