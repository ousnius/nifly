/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "Object3d.hpp"

#include <filesystem>
#include <memory>
#include <string_view>

namespace nifly {
// Applies a vertex index renumbering map to p1, p2, and p3 of a vector of triangles.
// If a triangle has an index out of range of the map
// or if an index maps to a negative number, the triangle is removed.
template<typename IndexType1, typename IndexType2 = int>
void ApplyMapToTriangles(std::vector<Triangle>& tris,
						 const std::vector<IndexType1>& map,
						 std::vector<IndexType2>* deletedTris = nullptr) {
	const size_t mapsz = map.size();
	int di = 0;
	for (IndexType2 si = 0; si < static_cast<IndexType2>(tris.size()); ++si) {
		const Triangle& stri = tris[si];
		// Triangle's indices are unsigned, but IndexType might be signed.
		if (stri.p1 >= mapsz || stri.p2 >= mapsz || stri.p3 >= mapsz || map[stri.p1] < 0 || map[stri.p2] < 0
			|| map[stri.p3] < 0) {
			if (deletedTris)
				deletedTris->push_back(si);

			continue;
		}

		Triangle& dtri = tris[di];
		dtri.p1 = static_cast<uint16_t>(map[stri.p1]);
		dtri.p2 = static_cast<uint16_t>(map[stri.p2]);
		dtri.p3 = static_cast<uint16_t>(map[stri.p3]);
		++di;
	}

	tris.resize(di);
}

inline uint16_t CalcMaxTriangleIndex(const std::vector<Triangle>& v) {
	uint16_t maxind = 0;

	for (size_t i = 0; i < v.size(); ++i) {
		maxind = std::max(maxind, v[i].p1);
		maxind = std::max(maxind, v[i].p2);
		maxind = std::max(maxind, v[i].p3);
	}

	return maxind;
}

// 'indices' must be in sorted ascending order beforehand.
template<typename VectorType, typename IndexType>
void EraseVectorIndices(VectorType& v, const std::vector<IndexType>& indices) {
	if (indices.empty() || indices[0] >= v.size())
		return;

	size_t indi = 1;
	IndexType di = indices[0];
	IndexType si = di + 1;
	for (; si < v.size(); ++si) {
		if (indi < indices.size() && si == indices[indi])
			++indi;
		else
			v[di++] = std::move(v[si]);
	}

	v.resize(di);
}

// 'indices' must be in sorted ascending order beforehand.
template<typename VectorType, typename IndexType>
void InsertVectorIndices(VectorType& v, const std::vector<IndexType>& indices) {
	if (indices.empty() || indices.back() >= v.size() + indices.size())
		return;

	int64_t indi = static_cast<int64_t>(indices.size() - 1);
	IndexType di = v.size() + indices.size() - 1;
	IndexType si = v.size() - 1;
	v.resize(di + 1);

	while (true) {
		while (indi >= 0 && di == indices[indi])
			--di, --indi;

		if (indi < 0)
			break;

		v[di--] = std::move(v[si--]);
	}
}

// 'indices' must be in sorted ascending order beforehand.
template<typename IndexType1, typename IndexType2>
std::vector<int> GenerateIndexCollapseMap(const std::vector<IndexType1>& indices, const IndexType2 mapSize) {
	std::vector<int> map(mapSize);

	size_t indi = 0;
	for (IndexType2 si = 0, di = 0; si < mapSize; ++si) {
		if (indi < indices.size() && si == indices[indi]) {
			map[si] = -1;
			++indi;
		}
		else
			map[si] = static_cast<int>(di++);
	}

	return map;
}

// 'indices' must be in sorted ascending order beforehand.
template<typename IndexType1, typename IndexType2>
std::vector<int> GenerateIndexExpandMap(const std::vector<IndexType1>& indices, const IndexType2 mapSize) {
	std::vector<int> map(mapSize);

	size_t indi = 0;
	for (IndexType2 si = 0, di = 0; si < mapSize; ++si, ++di) {
		while (indi < indices.size() && di == indices[indi])
			++di, ++indi;

		map[si] = static_cast<int>(di);
	}
	return map;
}

// MapType is something like std::unordered_map<int, Data> or std::map<int, Data>.
// If a MapType-key k is in the indexMap, it is deleted if indexMap[k]
// is negative, or changed to indexMap[k] otherwise.
// If k is not in indexMap, defaultOffset is added to it.
template<typename MapType>
void ApplyIndexMapToMapKeys(MapType& keyMap, const std::vector<int> indexMap, const int defaultOffset) {
	using KeyType = typename MapType::key_type;
	MapType copy;

	for (auto& d : keyMap) {
		if (d.first >= indexMap.size()) {
			auto keyVal = static_cast<KeyType>(d.first + defaultOffset);
			copy[keyVal] = std::move(d.second);
		}
		else if (indexMap[d.first] >= 0) {
			auto keyVal = static_cast<KeyType>(indexMap[d.first]);
			copy[keyVal] = std::move(d.second);
		}
	}

	keyMap = std::move(copy);
}

// Strips with less than 3 points are skipped as they cannot become a triangle.
template<typename IndexType>
std::vector<Triangle> GenerateTrianglesFromStrips(const std::vector<std::vector<IndexType>>& strips) {
	std::vector<Triangle> tris;

	for (const std::vector<IndexType>& strip : strips) {
		if (strip.size() < 3)
			continue;

		uint16_t a = strip[0];
		uint16_t b = strip[1];
		for (size_t i = 2; i < strip.size(); ++i) {
			uint16_t c = strip[i];
			if (a != b && b != c && c != a) {
				if ((i & 1) == 0)
					tris.push_back(Triangle(a, b, c));
				else
					tris.push_back(Triangle(a, c, b));
			}

			a = b;
			b = c;
		}
	}

	return tris;
}

// Helper to check if a potentially non valid UTF8 path is relative
inline bool is_relative_path(std::string_view path) noexcept {
	try {
		return std::filesystem::u8path(path).is_relative();
	}
	catch (const std::exception&) {
		// ignore the exception
		// the path is invalid, but might be readable by the game
		return false;
	}
}

// Convenience wrapper for std::find
template<typename Container, typename Value = typename Container::value>
auto find(Container& cont, Value&& val) {
	return std::find(std::begin(cont), std::end(cont), std::forward<Value>(val));
}

// Convenience wrapper for std::find (const)
template<typename Container, typename Value = typename Container::value>
auto find(const Container& cont, Value&& val) {
	return std::find(std::cbegin(cont), std::cend(cont), std::forward<Value>(val));
}

// Convenience wrapper for std::find_if
template<typename Container, typename Pred>
auto find_if(Container& cont, Pred&& pred) {
	return std::find_if(std::begin(cont), std::end(cont), std::forward<Pred>(pred));
}

// Convenience wrapper for std::find_if (const)
template<typename Container, typename Pred>
auto find_if(const Container& cont, Pred&& pred) {
	return std::find_if(std::cbegin(cont), std::cend(cont), std::forward<Pred>(pred));
}

// Convenience wrapper for std::find
template<typename Container, typename Value = typename Container::value>
bool contains(const Container& cont, Value&& val) {
	return find(cont, std::forward<Value>(val)) != std::end(cont);
}

// Return new unique pointer and raw pointer to the same object as part of a pair.
// This way, the object can still be accessed using the raw pointer after moving the smart pointer.
// Usage: auto [triShapeS, triShape] = make_unique<NiTriShape>();
template<typename T>
std::pair<std::unique_ptr<T>, T*> make_unique() {
	auto ptr = std::make_unique<T>();
	auto raw = ptr.get();
	return std::make_pair(std::move(ptr), raw);
}

} // namespace nifly
