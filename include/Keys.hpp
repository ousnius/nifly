/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"

namespace nifly {
enum KeyType : uint32_t { NO_INTERP, LINEAR_KEY, QUADRATIC_KEY, TBC_KEY, XYZ_ROTATION_KEY, CONST_KEY };

struct TBC {
	float tension = 0.0f;
	float bias = 0.0f;
	float continuity = 0.0f;
};

template<typename T>
struct Key {
	float time = 0.0f;
	T value{};
	T forward{};
	T backward{};
	TBC tbc;
};

template<typename T>
class KeyGroup {
private:
	uint32_t numKeys = 0;
	KeyType interpolation = NO_INTERP;
	std::vector<Key<T>> keys;

public:
	void Sync(NiStreamReversible& stream) {
		stream.Sync(numKeys);
		keys.resize(numKeys);

		if (numKeys > 0) {
			stream.Sync(interpolation);

			for (int i = 0; i < numKeys; i++) {
				auto& key = keys[i];
				stream.Sync(key.time);
				stream.Sync(key.value);

				switch (interpolation) {
					case QUADRATIC_KEY:
						stream.Sync(key.forward);
						stream.Sync(key.backward);
						break;
					case TBC_KEY: stream.Sync(key.tbc); break;
					default: break;
				}
			}
		}
	}

	KeyType GetInterpolationType() const { return interpolation; }

	void SetInterpolationType(const KeyType interp) { interpolation = interp; }

	uint32_t GetNumKeys() const { return numKeys; }

	Key<T> GetKey(const int id) const { return keys[id]; }

	void SetKey(const int id, const Key<T>& key) { keys[id] = key; }

	void AddKey(const Key<T>& key) {
		keys.push_back(key);
		numKeys++;
	}

	void RemoveKey(const int id) {
		keys.erase(keys.begin() + id);
		numKeys--;
	}

	void ClearKeys() {
		keys.clear();
		numKeys = 0;
	}
};
} // namespace nifly
