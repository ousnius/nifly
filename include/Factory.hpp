/*
BodySlide and Outfit Studio
See the included LICENSE file
*/

#pragma once

#include "Animation.hpp"
#include "BasicTypes.hpp"
#include "ExtraData.hpp"
#include "Geometry.hpp"
#include "Keys.hpp"
#include "Objects.hpp"
#include "Particles.hpp"
#include "Shaders.hpp"
#include "Skin.hpp"
#include "bhk.hpp"

#include <unordered_map>

namespace nifly {
class NiFactory {
public:
	virtual std::shared_ptr<NiObject> Create() = 0;
	virtual std::shared_ptr<NiObject> Load(NiStream& stream) = 0;
};

template<typename T>
class NiFactoryType : public NiFactory {
public:
	// Create new NiObject
	std::shared_ptr<NiObject> Create() override { return std::make_shared<T>(); }

	// Load new NiObject from file
	std::shared_ptr<NiObject> Load(NiStream& stream) override {
		auto nio = std::make_shared<T>();
		nio->Get(stream);
		return nio;
	}
};

class NiFactoryRegister {
public:
	// Constructor registers the block types
	NiFactoryRegister();

	template<typename T>
	void RegisterFactory() {
		// Any NiObject can be registered together with its block name
		m_registrations.emplace(T::BlockName, std::make_shared<NiFactoryType<T>>());
	}

	// Get block factory via header std::string
	std::shared_ptr<NiFactory> GetFactoryByName(const std::string& name) {
		auto it = m_registrations.find(name);
		if (it != m_registrations.end())
			return it->second;

		return nullptr;
	}

	// Get static instance of factory register
	static NiFactoryRegister& Get();

protected:
	std::unordered_map<std::string, std::shared_ptr<NiFactory>> m_registrations;
};
} // namespace nifly
