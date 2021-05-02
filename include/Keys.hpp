/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"

namespace nifly {
class NiTextKey {
public:
	float time = 0.0f;
	NiStringRef value;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(time);
		value.Sync(stream);
	}

	void GetStringRefs(std::vector<NiStringRef*>& refs) { refs.emplace_back(&value); }
};

enum NiKeyType : uint32_t { NO_INTERP, LINEAR_KEY, QUADRATIC_KEY, TBC_KEY, XYZ_ROTATION_KEY, CONST_KEY };

struct TBC {
	float tension = 0.0f;
	float bias = 0.0f;
	float continuity = 0.0f;
};

template<typename T>
class NiAnimationKey {
public:
	NiKeyType type = NiKeyType::NO_INTERP; // no IO, used for Sync condition only

	float time = 0.0f;
	T value{};
	T forward{};
	T backward{};
	TBC tbc;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(time);
		stream.Sync(value);

		switch (type) {
			case NiKeyType::QUADRATIC_KEY:
				stream.Sync(forward);
				stream.Sync(backward);
				break;
			case NiKeyType::TBC_KEY: stream.Sync(tbc); break;
			default: break;
		}
	}
};

template<typename T>
class NiAnimationKeyGroup {
private:
	uint32_t numKeys = 0;
	NiKeyType interpolation = NO_INTERP;
	std::vector<NiAnimationKey<T>> keys;

public:
	void Sync(NiStreamReversible& stream) {
		stream.Sync(numKeys);
		keys.resize(numKeys);

		if (numKeys > 0) {
			stream.Sync(interpolation);

			for (uint32_t i = 0; i < numKeys; i++) {
				auto& key = keys[i];
				key.type = interpolation;
				key.Sync(stream);
			}
		}
	}

	NiKeyType GetInterpolationType() const { return interpolation; }

	void SetInterpolationType(const NiKeyType interp) { interpolation = interp; }

	uint32_t GetNumKeys() const { return numKeys; }

	NiAnimationKey<T> GetKey(const int id) const { return keys[id]; }

	void SetKey(const int id, const NiAnimationKey<T>& key) { keys[id] = key; }

	void AddKey(const NiAnimationKey<T>& key) {
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
