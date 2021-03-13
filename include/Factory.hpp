/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
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
	virtual std::unique_ptr<NiObject> Create() = 0;
	virtual std::unique_ptr<NiObject> Load(NiIStream& stream) = 0;
};

template<typename T>
class NiFactoryType : public NiFactory {
public:
	// Create new NiObject
	std::unique_ptr<NiObject> Create() override { return std::make_unique<T>(); }

	// Load new NiObject from file
	std::unique_ptr<NiObject> Load(NiIStream& stream) override {
		auto nio = std::make_unique<T>();
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
		m_registrations.emplace(T::BlockName, std::make_unique<NiFactoryType<T>>());
	}

	// Get block factory via header std::string
	NiFactory* GetFactoryByName(const std::string& name) {
		auto it = m_registrations.find(name);
		if (it != m_registrations.end())
			return it->second.get();

		return nullptr;
	}

	// Get static instance of factory register
	static NiFactoryRegister& Get();

protected:
	std::unordered_map<std::string, std::unique_ptr<NiFactory>> m_registrations;
};
} // namespace nifly
