/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "Object3d.hpp"
#include "half.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <streambuf>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace nifly {
constexpr auto NIF_NPOS = static_cast<uint32_t>(-1);

constexpr auto NiCharMin = std::numeric_limits<char>::min();
constexpr auto NiCharMax = std::numeric_limits<char>::max();
constexpr auto NiByteMin = std::numeric_limits<uint8_t>::min();
constexpr auto NiByteMax = std::numeric_limits<uint8_t>::max();
constexpr auto NiUShortMin = std::numeric_limits<uint16_t>::min();
constexpr auto NiUShortMax = std::numeric_limits<uint16_t>::max();
constexpr auto NiUIntMin = std::numeric_limits<uint32_t>::min();
constexpr auto NiUIntMax = std::numeric_limits<uint32_t>::max();
constexpr auto NiFloatMin = std::numeric_limits<float>::lowest();
constexpr auto NiFloatMax = std::numeric_limits<float>::max();
constexpr auto NiVec3Min = Vector3(NiFloatMin, NiFloatMin, NiFloatMin);
constexpr auto NiVec4Min = Vector4(NiFloatMin, NiFloatMin, NiFloatMin, NiFloatMin);

enum NiFileVersion : uint32_t {
	V2_3 = 0x02030000,
	V3_0 = 0x03000000,
	V3_03 = 0x03000300,
	V3_1 = 0x03010000,
	V3_3_0_13 = 0x0303000D,
	V4_0_0_0 = 0x04000000,
	V4_0_0_2 = 0x04000002,
	V4_1_0_12 = 0x0401000C,
	V4_2_0_2 = 0x04020002,
	V4_2_1_0 = 0x04020100,
	V4_2_2_0 = 0x04020200,
	V5_0_0_1 = 0x05000001,
	V10_0_0_0 = 0x0A000000,
	V10_0_1_0 = 0x0A000100,
	V10_0_1_2 = 0x0A000102,
	V10_0_1_3 = 0x0A000103,
	V10_1_0_0 = 0x0A010000,
	V10_1_0_101 = 0x0A010065,
	V10_1_0_104 = 0x0A010068,
	V10_1_0_106 = 0x0A01006A,
	V10_1_0_114 = 0x0A010072,
	V10_2_0_0 = 0x0A020000,
	V10_2_0_1 = 0x0A020001,
	V10_3_0_1 = 0x0A030001,
	V10_4_0_1 = 0x0A040001,
	V20_0_0_4 = 0x14000004,
	V20_0_0_5 = 0x14000005,
	V20_1_0_1 = 0x14010001,
	V20_1_0_3 = 0x14010003,
	V20_2_0_5 = 0x14020005,
	V20_2_0_7 = 0x14020007,
	V20_2_0_8 = 0x14020008,
	V20_3_0_1 = 0x14030001,
	V20_3_0_2 = 0x14030002,
	V20_3_0_3 = 0x14030003,
	V20_3_0_6 = 0x14030006,
	V20_3_0_9 = 0x14030009,
	V20_5_0_0 = 0x14050000,
	V20_6_0_0 = 0x14060000,
	V20_6_5_0 = 0x14060500,
	V30_0_0_2 = 0x1E000002,
	V30_1_0_3 = 0x1E010003,
	UNKNOWN = 0xFFFFFFFF
};

class NiVersion {
private:
	std::string vstr;
	NiFileVersion file = UNKNOWN;
	uint32_t user = 0;
	uint32_t stream = 0;
	uint32_t nds = 0;

public:
	NiVersion() = default;
	NiVersion(NiFileVersion _file, uint32_t _user, uint32_t _stream);

	// Construct a file version enumeration from individual values
	static NiFileVersion ToFile(uint8_t major, uint8_t minor, uint8_t patch, uint8_t internal) {
		return NiFileVersion((major << 24) | (minor << 16) | (patch << 8) | internal);
	}

	// Return file version as individual values
	static std::vector<uint8_t> ToArray(NiFileVersion file) {
		return {uint8_t(file >> 24), uint8_t(file >> 16), uint8_t(file >> 8), uint8_t(file)};
	}

	std::string GetVersionInfo() const;
	std::string String() const { return vstr; }

	NiFileVersion File() const { return file; }
	void SetFile(NiFileVersion fileVer);

	uint32_t User() const { return user; }
	void SetUser(const uint32_t userVer) { user = userVer; }

	uint32_t Stream() const { return stream; }
	void SetStream(const uint32_t streamVer) { stream = streamVer; }

	uint32_t NDS() const { return nds; }
	void SetNDS(const uint32_t ndsVer) { nds = ndsVer; }

	// Check if file is for a Bethesda title
	bool IsBethesda() const { return (file == V20_2_0_7 && user >= 11) || IsOB(); }

	// Check if file has a special but supported version range
	bool IsSpecial() const { return (file == V10_0_1_0 && user == 0); }

	// Check if file has an Oblivion version range
	bool IsOB() const {
		return
			((file == V10_1_0_106 || file == V10_2_0_0) && user >= 3 && user < 11) ||
			(file == V20_0_0_4 && (user == 10 || user == 11)) ||
			(file == V20_0_0_5 && user == 11);
	}

	// Check if file has a Fallout 3 version range
	bool IsFO3() const { return file == V20_2_0_7 && stream > 11 && stream < 83; }
	// Check if file has a Skyrim (LE) version range
	bool IsSK() const { return file == V20_2_0_7 && stream == 83; }
	// Check if file has a Skyrim (SE) version range
	bool IsSSE() const { return file == V20_2_0_7 && stream == 100; }
	// Check if file has a Fallout 4 version range
	bool IsFO4() const { return file == V20_2_0_7 && stream >= 130 && stream <= 139; }
	// Check if file has a Fallout 76 version range
	bool IsFO76() const { return file == V20_2_0_7 && stream == 155; }

	// Return an Oblivion file version
	static NiVersion getOB() { return NiVersion(NiFileVersion::V20_0_0_5, 11, 11); }
	// Return a Fallout 3 file version
	static NiVersion getFO3() { return NiVersion(NiFileVersion::V20_2_0_7, 11, 34); }
	// Return a Skyrim (LE) file version
	static NiVersion getSK() { return NiVersion(NiFileVersion::V20_2_0_7, 12, 83); }
	// Return a Skyrim (SE) file version
	static NiVersion getSSE() { return NiVersion(NiFileVersion::V20_2_0_7, 12, 100); }
	// Return a Fallout 4 file version
	static NiVersion getFO4() { return NiVersion(NiFileVersion::V20_2_0_7, 12, 130); }
	// Return a Fallout 76 file version
	static NiVersion getFO76() { return NiVersion(NiFileVersion::V20_2_0_7, 12, 155); }
};

enum NiEndian : uint8_t { ENDIAN_BIG, ENDIAN_LITTLE };

class NiHeaderBase {
protected:
	bool valid = false;
	std::streampos blockSizePos;

	NiVersion version;
	NiEndian endian = ENDIAN_LITTLE;

public:
	virtual ~NiHeaderBase() {}

	bool IsValid() const { return valid; }

	NiVersion& GetVersion() { return version; }
	const NiVersion& GetVersion() const { return version; }

	void SetVersion(const NiVersion& ver) { version = ver; }

	virtual uint32_t GetStringCount() const = 0;
	virtual uint32_t FindStringId(const std::string& str) const = 0;
	virtual uint32_t AddOrFindStringId(const std::string& str, const bool addEmpty = false) = 0;
	virtual std::string GetStringById(const uint32_t id) const = 0;
	virtual void SetStringById(const uint32_t id, const std::string& str) = 0;
};

class NiStreamBase {
private:
	NiHeaderBase* header = nullptr;

public:
	explicit NiStreamBase(NiHeaderBase* hdr)
		: header(hdr) {}

	NiVersion& GetVersion() { return header->GetVersion(); }
	const NiVersion& GetVersion() const { return header->GetVersion(); }

	NiHeaderBase& GetHeader() { return *header; }
	const NiHeaderBase& GetHeader() const { return *header; }
};

class NiIStream : public NiStreamBase {
private:
	std::istream* stream = nullptr;

public:
	NiIStream(std::istream* s, NiHeaderBase* hdr)
		: NiStreamBase(hdr)
		, stream(s) {}

	void read(char* ptr, std::streamsize count) { stream->read(ptr, count); }
	void getline(char* ptr, std::streamsize maxCount) { stream->getline(ptr, maxCount); }

	// Be careful with sizes of structs and classes
	template<typename T>
	NiIStream& operator>>(T& t) {
		read((char*) &t, sizeof(T));
		return *this;
	}
};

class NiOStream : public NiStreamBase {
private:
	std::ostream* stream = nullptr;
	std::streamsize blockSize = 0;

public:
	NiOStream(std::ostream* s, NiHeaderBase* hdr)
		: NiStreamBase(hdr)
		, stream(s) {}

	void write(const char* ptr, std::streamsize count) {
		stream->write(ptr, count);
		blockSize += count;
	}

	void writeline(const char* ptr, std::streamsize count) {
		stream->write(ptr, count);
		stream->write("\n", 1);
		blockSize += count + 1;
	}
	std::streampos tellp() { return stream->tellp(); }

	// Be careful with sizes of structs and classes
	template<typename T>
	NiOStream& operator<<(const T& t) {
		write((const char*) &t, sizeof(T));
		return *this;
	}

	void InitBlockSize() { blockSize = 0; }
	std::streamsize GetBlockSize() { return blockSize; }
};

class NiStreamReversible {
public:
	enum class Mode { Reading, Writing };

	explicit NiStreamReversible(NiIStream* is, NiOStream* os, Mode mode_)
		: istream(is)
		, ostream(os)
		, mode(mode_) {}

	void SetMode(Mode m) { mode = m; }
	Mode GetMode() const { return mode; }

	template<typename T>
	void Sync(T& t) {
		Sync(reinterpret_cast<char*>(&t), sizeof(T));
	}

	NiVersion& GetVersion() {
		if (mode == Mode::Reading)
			return istream->GetVersion();
		else
			return ostream->GetVersion();
	}

	const NiVersion& GetVersion() const {
		if (mode == Mode::Reading)
			return istream->GetVersion();
		else
			return ostream->GetVersion();
	}

	NiHeaderBase& GetHeader() {
		if (mode == Mode::Reading)
			return istream->GetHeader();
		else
			return ostream->GetHeader();
	}

	const NiHeaderBase& GetHeader() const {
		if (mode == Mode::Reading)
			return istream->GetHeader();
		else
			return ostream->GetHeader();
	}

	void Sync(char* ptr, std::streamsize count) {
		if (mode == Mode::Reading)
			istream->read(ptr, count);
		else
			ostream->write(ptr, count);
	}

	void SyncLine(char* ptr, std::streamsize count) {
		if (mode == Mode::Reading)
			istream->getline(ptr, count);
		else
			ostream->writeline(ptr, count);
	}

	void SyncHalf(float& fl) {
		half_float::half halfData;

		if (mode == Mode::Writing)
			halfData = fl;

		Sync(reinterpret_cast<char*>(&halfData), 2);

		if (mode == Mode::Reading)
			fl = halfData;
	}

	NiOStream* asWrite() { return ostream; }
	NiIStream* asRead() { return istream; }


private:
	NiIStream* istream;
	NiOStream* ostream;
	Mode mode;
};

template<typename Derived, typename Base>
class NiCloneable : public Base {
public:
	virtual ~NiCloneable() override = default;

	std::unique_ptr<Derived> Clone() const {
		return std::unique_ptr<Derived>(static_cast<Derived*>(this->Clone_impl()));
	}

private:
	virtual NiCloneable* Clone_impl() const override { return new Derived(asDer()); }

	Derived& asDer() { return static_cast<Derived&>(*this); }
	const Derived& asDer() const { return static_cast<const Derived&>(*this); }
};

template<typename Derived, typename Base>
class NiStreamable : public Base {
public:
	void Get(NiIStream& stream) override {
		Base::Get(stream);
		NiStreamReversible s(&stream, nullptr, NiStreamReversible::Mode::Reading);
		asDer().Sync(s);
	}

	void Put(NiOStream& stream) override {
		Base::Put(stream);
		NiStreamReversible s(nullptr, &stream, NiStreamReversible::Mode::Writing);
		asDer().Sync(s);
	}

private:
	Derived& asDer() { return static_cast<Derived&>(*this); }
	const Derived& asDer() const { return static_cast<const Derived&>(*this); }
};

template<typename Derived, typename Base>
class NiCloneableStreamable : public Base {
public:
	virtual ~NiCloneableStreamable() override = default;

	std::unique_ptr<Derived> Clone() const {
		return std::unique_ptr<Derived>(static_cast<Derived*>(this->Clone_impl()));
	}

	void Get(NiIStream& stream) override {
		Base::Get(stream);
		NiStreamReversible s(&stream, nullptr, NiStreamReversible::Mode::Reading);
		asDer().Sync(s);
	}

	void Put(NiOStream& stream) override {
		Base::Put(stream);
		NiStreamReversible s(nullptr, &stream, NiStreamReversible::Mode::Writing);
		asDer().Sync(s);
	}

private:
	virtual NiCloneableStreamable* Clone_impl() const override { return new Derived(asDer()); }

	Derived& asDer() { return static_cast<Derived&>(*this); }
	const Derived& asDer() const { return static_cast<const Derived&>(*this); }
};

class NiString {
private:
	std::string str;
	bool nullOutput = false; // append a null byte when writing the string

public:
	NiString() = default;
	NiString(const std::string& s, const bool wantNullOutput = false) {
		str = s;
		nullOutput = wantNullOutput;
	}

	std::string& get() { return str; }
	const std::string& get() const { return str; }

	size_t length() const { return str.length(); }

	void SetNullOutput(const bool wantNullOutput = true) { nullOutput = wantNullOutput; }
	void clear() { str.clear(); }

	void Read(NiIStream& stream, const int szSize);
	void Write(NiOStream& stream, const int szSize);

	void Sync(NiStreamReversible& stream, const int szSize) {
		if (auto istream = stream.asRead())
			Read(*istream, szSize);
		else if (auto ostream = stream.asWrite())
			Write(*ostream, szSize);
	}

	bool operator==(const NiString& rhs) const { return str == rhs.str; }
	bool operator!=(const NiString& rhs) const { return !operator==(rhs); }

	bool operator==(const std::string& rhs) const { return str == rhs; }
	bool operator!=(const std::string& rhs) const { return !operator==(rhs); }
};

class NiStringRef {
private:
	std::string str;
	uint32_t index = NIF_NPOS; // Temporary index storage for load/save

public:
	NiStringRef() = default;
	NiStringRef(const std::string& s) { str = s; }

	std::string& get() { return str; }
	const std::string& get() const { return str; }

	size_t length() const { return str.length(); }

	uint32_t GetIndex() const { return index; }
	void SetIndex(const uint32_t id) { index = id; }

	void clear() {
		index = NIF_NPOS;
		str.clear();
	}

	void Read(NiIStream& stream);
	void Write(NiOStream& stream);

	void Sync(NiStreamReversible& stream) {
		if (auto istream = stream.asRead())
			Read(*istream);
		else if (auto ostream = stream.asWrite())
			Write(*ostream);
	}

	bool operator==(const NiStringRef& rhs) const { return str == rhs.str; }
	bool operator!=(const NiStringRef& rhs) const { return !operator==(rhs); }

	bool operator==(const std::string& rhs) const { return str == rhs; }
	bool operator!=(const std::string& rhs) const { return !operator==(rhs); }
};

struct NiPlane {
	Vector3 normal;
	float constant = 0.0f;
};

class NiRef {
public:
	uint32_t index = NIF_NPOS;

	void Clear() { index = NIF_NPOS; }
	bool IsEmpty() const { return index == NIF_NPOS; }

	bool operator==(const NiRef& rhs) const { return index == rhs.index; }
	bool operator!=(const NiRef& rhs) const { return !operator==(rhs); }

	bool operator==(const uint32_t rhs) const { return index == rhs; }
	bool operator!=(const uint32_t rhs) const { return !operator==(rhs); }
};

using NiPtr = NiRef;

// Helper to reduce duplication
template<typename ValueType, typename SizeType>
class NiVectorBase {
private:
	std::vector<ValueType> vec;

protected:
	static constexpr size_t NumSize = sizeof(SizeType);
	static constexpr SizeType MaxIndex = std::numeric_limits<SizeType>::max() - 1;

public:
	NiVectorBase() = default;
	NiVectorBase(const SizeType size) { resize(size); }

	SizeType size() const { return static_cast<SizeType>(vec.size()); }
	bool empty() const { return vec.empty(); }

	void clear() { vec.clear(); }

	auto begin() { return vec.begin(); }
	auto cbegin() const { return vec.begin(); }

	auto end() { return vec.end(); }
	auto cend() const { return vec.end(); }

	void resize(SizeType size) { vec.resize(size); }

	void push_back(ValueType& val) { vec.push_back(val); }
	auto insert(SizeType index, ValueType& val) { vec.insert(vec.begin() + index, val); }

	auto& operator[](SizeType i) { return vec[i]; }

	ValueType* data() { return vec.data(); }
	const ValueType* data() const { return vec.data(); }

	auto erase(SizeType i) { return vec.erase(vec.begin() + i); }
};

template<typename ValueType, typename SizeType = uint32_t>
class NiVector : public NiVectorBase<ValueType, SizeType> {
	using Base = NiVectorBase<ValueType, SizeType>;
	using Base::MaxIndex;
	using Base::NumSize;

public:
	NiVector() = default;
	NiVector(const SizeType size)
		: Base(size) {}

	SizeType Sync(NiStreamReversible& stream) {
		SizeType sz = SyncSize(stream);
		SyncData(stream, sz);
		return sz;
	}

	SizeType SyncSize(NiStreamReversible& stream) {
		SizeType sz = 0;

		if (stream.GetMode() == NiStreamReversible::Mode::Writing) {
			if (!Base::empty() && Base::size() - 1 > MaxIndex)
				Base::resize(MaxIndex + 1);
		}

		sz = Base::size();

		stream.Sync(reinterpret_cast<char*>(&sz), NumSize);
		return sz;
	}


	void SyncData(NiStreamReversible& stream, const SizeType size) {
		Base::resize(size);

		for (auto& e : *this)
			stream.Sync(e);
	}

	void SyncByteArray(NiStreamReversible& stream) {
		SizeType sz = SyncSize(stream);
		Base::resize(sz);

		if (sz > 0)
			stream.Sync(reinterpret_cast<char*>(Base::data()), sz);
	}
};

template<typename ValueType, typename SizeType = uint32_t>
class NiSyncVector : public NiVectorBase<ValueType, SizeType> {
	using Base = NiVectorBase<ValueType, SizeType>;
	using Base::MaxIndex;
	using Base::NumSize;

public:
	NiSyncVector() = default;
	NiSyncVector(const SizeType size)
		: Base(size) {}

	SizeType Sync(NiStreamReversible& stream) {
		SizeType sz = SyncSize(stream);
		SyncData(stream, sz);
		return sz;
	}

	SizeType SyncSize(NiStreamReversible& stream) {
		SizeType sz = 0;

		if (stream.GetMode() == NiStreamReversible::Mode::Writing) {
			if (!Base::empty() && Base::size() - 1 > MaxIndex)
				Base::resize(MaxIndex + 1);
		}

		sz = Base::size();

		stream.Sync(reinterpret_cast<char*>(&sz), NumSize);
		return sz;
	}

	void SyncData(NiStreamReversible& stream, const SizeType size) {
		Base::resize(size);

		for (auto& e : *this)
			e.Sync(stream);
	}

	void GetStringRefs(std::vector<NiStringRef*>& refs) {
		for (auto& e : *this)
			e.GetStringRefs(refs);
	}

	void GetChildRefs(std::set<NiRef*>& refs) {
		for (auto& e : *this)
			e.GetChildRefs(refs);
	}

	void GetChildIndices(std::vector<uint32_t>& indices) {
		for (auto& e : *this)
			e.GetChildIndices(indices);
	}

	void GetPtrs(std::set<NiPtr*>& ptrs) {
		for (auto& e : *this)
			e.GetPtrs(ptrs);
	}
};

template<typename SizeType = uint32_t, const int stringSize = 4>
class NiStringVector : public NiVectorBase<NiString, SizeType> {
private:
	using Base = NiVectorBase<NiString, SizeType>;
	using Base::MaxIndex;
	using Base::NumSize;

public:
	NiStringVector() = default;
	NiStringVector(const SizeType size) { Base::resize(size); }

	void Read(NiIStream& stream) {
		SizeType sz = 0;
		stream.read(reinterpret_cast<char*>(&sz), NumSize);

		Base::resize(sz);

		for (auto& e : *this)
			e.Read(stream, stringSize);
	}

	void Write(NiOStream& stream) {
		if (!Base::empty() && Base::size() - 1 > MaxIndex)
			Base::resize(MaxIndex + 1);

		SizeType sz = Base::size();
		stream.write(reinterpret_cast<char*>(&sz), NumSize);

		for (auto& e : *this)
			e.Write(stream, stringSize);
	}

	void Sync(NiStreamReversible& stream) {
		if (auto istream = stream.asRead())
			Read(*istream);
		else if (auto ostream = stream.asWrite())
			Write(*ostream);
	}
};

template<typename SizeType = uint32_t>
class NiStringRefVector : public NiVectorBase<NiStringRef, SizeType> {
private:
	using Base = NiVectorBase<NiStringRef, SizeType>;
	using Base::MaxIndex;
	using Base::NumSize;

public:
	NiStringRefVector() = default;
	NiStringRefVector(const SizeType size) { resize(size); }

	void Read(NiIStream& stream) {
		SizeType sz = 0;
		stream.read(reinterpret_cast<char*>(&sz), NumSize);

		Base::resize(sz);

		for (auto& e : *this)
			e.Read(stream);
	}

	void Write(NiOStream& stream) {
		if (!Base::empty() && Base::size() - 1 > MaxIndex)
			Base::resize(MaxIndex + 1);

		SizeType sz = Base::size();
		stream.write(reinterpret_cast<char*>(&sz), NumSize);

		for (auto& e : *this)
			e.Write(stream);
	}

	void Sync(NiStreamReversible& stream) {
		if (auto istream = stream.asRead())
			Read(*istream);
		else if (auto ostream = stream.asWrite())
			Write(*ostream);
	}
};

template<typename T>
class NiBlockRef : public NiRef {
	using base = NiRef;

public:
	NiBlockRef() {}
	NiBlockRef(const uint32_t id) { NiRef::index = id; }

	void Sync(NiStreamReversible& stream) { stream.Sync(base::index); }
};

template<typename T>
using NiBlockPtr = NiBlockRef<T>;

class NiRefArray {
protected:
	uint32_t arraySize = 0;
	bool keepEmptyRefs = false;

public:
	virtual ~NiRefArray() {}

	uint32_t GetSize() const { return arraySize; }

	void SetKeepEmptyRefs(const bool keep = true) { keepEmptyRefs = keep; }

	virtual void Sync(NiStreamReversible& stream) = 0;

	virtual void AddBlockRef(const uint32_t id) = 0;
	virtual uint32_t GetBlockRef(const uint32_t id) const = 0;
	virtual void SetBlockRef(const uint32_t id, const uint32_t index) = 0;
	virtual void RemoveBlockRef(const uint32_t id) = 0;
	virtual void GetIndices(std::vector<uint32_t>& indices) = 0;
	virtual void GetIndexPtrs(std::set<NiRef*>& indices) = 0;
	virtual void SetIndices(const std::vector<uint32_t>& indices) = 0;
};

template<typename T>
class NiBlockRefArray : public NiRefArray {
protected:
	using NiRefArray::arraySize;
	using NiRefArray::keepEmptyRefs;

	std::vector<NiBlockRef<T>> refs;

	void CleanInvalidRefs() {
		if (keepEmptyRefs)
			return;

		refs.erase(std::remove_if(refs.begin(), refs.end(), [](NiBlockRef<T> r) { return r.IsEmpty(); }),
				   refs.end());

		arraySize = static_cast<uint32_t>(refs.size());
	}

public:
	using iterator = typename std::vector<NiBlockRef<T>>::iterator;
	using const_iterator = typename std::vector<NiBlockRef<T>>::const_iterator;

	iterator begin() { return refs.begin(); }

	iterator end() { return refs.end(); }

	const_iterator cbegin() const { return refs.cbegin(); }

	const_iterator cend() const { return refs.cend(); }

	void Clear() {
		refs.clear();
		arraySize = 0;
		keepEmptyRefs = false;
	}

	void SetSize(const uint32_t size) {
		arraySize = size;
		refs.resize(arraySize);
	}

	void Sync(NiStreamReversible& stream) override {
		if (stream.GetMode() == NiStreamReversible::Mode::Writing)
			CleanInvalidRefs();

		stream.Sync(arraySize);
		refs.resize(arraySize);

		for (auto& r : refs)
			r.Sync(stream);
	}

	void AddBlockRef(const uint32_t index) override {
		refs.push_back(NiBlockRef<T>(index));
		arraySize++;
	}

	uint32_t GetBlockRef(const uint32_t id) const override {
		if (id != NIF_NPOS && refs.size() > id)
			return refs[id].index;

		return NIF_NPOS;
	}

	void SetBlockRef(const uint32_t id, const uint32_t index) override {
		if (id != NIF_NPOS && refs.size() > id)
			refs[id].index = index;
	}

	void RemoveBlockRef(const uint32_t id) override {
		if (id != NIF_NPOS && refs.size() > id) {
			refs.erase(refs.begin() + id);
			arraySize--;
		}
	}

	void GetIndices(std::vector<uint32_t>& indices) override {
		for (auto& r : refs)
			indices.push_back(r.index);
	}

	void GetIndexPtrs(std::set<NiRef*>& indices) override {
		for (auto& r : refs)
			indices.insert(&r);
	}

	void SetIndices(const std::vector<uint32_t>& indices) override {
		arraySize = static_cast<uint32_t>(indices.size());
		refs.resize(arraySize);

		for (uint32_t i = 0; i < arraySize; i++)
			refs[i].index = indices[i];
	}
};

template<typename T>
using NiBlockPtrArray = NiBlockRefArray<T>;

template<typename T>
class NiBlockRefShortArray : public NiBlockRefArray<T> {
public:
	using base = NiBlockRefArray<T>;
	using base::arraySize;
	using base::refs;

	void Sync(NiStreamReversible& stream) override {
		if (stream.GetMode() == NiStreamReversible::Mode::Writing)
			base::CleanInvalidRefs();

		stream.Sync(reinterpret_cast<char*>(&arraySize), 2);
		refs.resize(arraySize);

		for (auto& r : refs)
			r.Sync(stream);
	}
};

template<typename T>
using NiBlockPtrShortArray = NiBlockRefShortArray<T>;

class NiObject {
protected:
	uint32_t blockSize = 0;
	uint32_t groupID = 0;

public:
	virtual ~NiObject() = default;

	static constexpr const char* BlockName = "NiUnknown";
	virtual const char* GetBlockName() { return BlockName; }

	virtual void notifyVerticesDelete(const std::vector<uint16_t>&) {}

	virtual void Get(NiIStream& stream) {
		if (stream.GetVersion().File() >= V10_0_0_0 && stream.GetVersion().File() < V10_1_0_114)
			stream.read(reinterpret_cast<char*>(&groupID), 4);
	}

	virtual void Put(NiOStream& stream) {
		if (stream.GetVersion().File() >= V10_0_0_0 && stream.GetVersion().File() < V10_1_0_114)
			stream.write(reinterpret_cast<const char*>(&groupID), 4);
	}

	virtual void GetStringRefs(std::vector<NiStringRef*>&) {}
	virtual void GetChildRefs(std::set<NiRef*>&) {}
	virtual void GetChildIndices(std::vector<uint32_t>&) {}
	virtual void GetPtrs(std::set<NiPtr*>&) {}

	std::unique_ptr<NiObject> Clone() const {
		return std::unique_ptr<NiObject>(static_cast<NiObject*>(this->Clone_impl()));
	}

	template<typename T>
	bool HasType() const {
		return dynamic_cast<const T*>(this) != nullptr;
	}

private:
	virtual NiObject* Clone_impl() const = 0;
};

class NiHeader : public NiHeaderBase, public NiCloneable<NiHeader, NiObject> {
	/*
	Minimum supported
	Version:			20.2.0.7
	User Version:		11
	User Version 2:		26

	Maximum supported
	Version:			20.2.0.7
	User Version:		12
	User Version 2:		155
	*/

private:
	NiString creator;
	uint32_t unkInt1 = 0;
	NiString exportInfo1;
	NiString exportInfo2;
	NiString exportInfo3;

	std::string copyright1;
	std::string copyright2;
	std::string copyright3;

	uint32_t embedDataSize = 0;
	std::vector<uint8_t> embedData;

	// Foreign reference to the blocks list in NifFile.
	std::vector<std::unique_ptr<NiObject>>* blocks = nullptr;

	uint32_t numBlocks = 0;
	uint16_t numBlockTypes = 0;
	std::vector<NiString> blockTypes;
	std::vector<uint16_t> blockTypeIndices;
	std::vector<uint32_t> blockSizes;

	uint32_t numStrings = 0;
	uint32_t maxStringLen = 0;
	std::vector<NiString> strings;

	uint32_t numGroups = 0;
	std::vector<uint32_t> groupSizes;

public:
	static constexpr const char* BlockName = "NiHeader";
	const char* GetBlockName() override { return BlockName; }

	void Clear();

	std::string GetCreatorInfo() const;
	void SetCreatorInfo(const std::string& creatorInfo);

	std::string GetExportInfo() const;

	// Sets export info string (automatically split into three members after 256 characters each)
	void SetExportInfo(const std::string& exportInfo);

	// Sets pointer to all blocks in the file
	void SetBlockReference(std::vector<std::unique_ptr<NiObject>>* blockRef) { blocks = blockRef; }

	uint32_t GetNumBlocks() const { return numBlocks; }

	template<class T>
	T* GetBlock(const uint32_t blockId) const {
		if (blockId != NIF_NPOS && blockId < numBlocks)
			return dynamic_cast<T*>((*blocks)[blockId].get());

		return nullptr;
	}

	template<class T>
	T* GetBlock(const NiBlockRef<T>& blockRef) const {
		return GetBlock<T>(blockRef.index);
	}

	template<class T>
	T* GetBlock(const NiBlockRef<T>* blockRef) const {
		if (blockRef)
			return GetBlock<T>(blockRef->index);
		return nullptr;
	}

	template<class T>
	T* GetBlock(const NiRef& blockRef) const {
		return GetBlock<T>(blockRef.index);
	}

	template<class T>
	T* GetBlock(const NiRef* blockRef) const {
		if (blockRef)
			return GetBlock<T>(blockRef->index);
		return nullptr;
	}

	template<class T>
	T* GetBlockUnsafe(const uint32_t blockId) const {
		if (blockId != NIF_NPOS && blockId < numBlocks)
			return static_cast<T*>((*blocks)[blockId].get());

		return nullptr;
	}

	template<class T>
	T* GetBlockUnsafe(const NiBlockRef<T>& blockRef) const {
		return GetBlockUnsafe<T>(blockRef.index);
	}

	template<class T>
	T* GetBlockUnsafe(const NiBlockRef<T>* blockRef) const {
		if (blockRef)
			return GetBlockUnsafe<T>(blockRef->index);
		return nullptr;
	}

	template<class T>
	T* GetBlockUnsafe(const NiRef& blockRef) const {
		return GetBlockUnsafe<T>(blockRef.index);
	}

	template<class T>
	T* GetBlockUnsafe(const NiRef* blockRef) const {
		if (blockRef)
			return GetBlockUnsafe<T>(blockRef->index);
		return nullptr;
	}

	// Returns the index of a block in the file (or NIF_NPOS)
	uint32_t GetBlockID(NiObject* block) const;

	// Deletes a block and notifies all other blocks
	void DeleteBlock(const uint32_t blockId);
	// Deletes a block and notifies all other blocks
	void DeleteBlock(const NiRef& blockRef);

	// Deletes all blocks with the specified block type name.
	// "orphanedOnly" makes sure no blocks that are still referenced by other blocks are deleted.
	void DeleteBlockByType(const std::string& blockTypeStr, const bool orphanedOnly = false);

	// Adds a new block to the file. Pointer is moved to the file.
	uint32_t AddBlock(std::unique_ptr<NiObject> newBlock);

	// Replaces an existing block in the file. Pointer is moved to the file.
	// This is not the same as deleting and adding a new block.
	uint32_t ReplaceBlock(const uint32_t oldBlockId, std::unique_ptr<NiObject> newBlock);

	void SetBlockOrder(std::vector<uint32_t>& newOrder);

	bool IsBlockReferenced(const uint32_t blockId, bool includePtrs = true);
	int GetBlockRefCount(const uint32_t blockId, bool includePtrs = true);

	// Deletes all unreferenced (loose) blocks of the given type starting at the specified root.
	// Use template type "NiObject" for all block types.
	// Sets the amount of deleted blocks (or 0) in "deletionCount".
	template<class T>
	bool DeleteUnreferencedBlocks(const uint32_t rootId, uint32_t* deletionCount = nullptr) {
		if (rootId == NIF_NPOS)
			return false;

		for (uint32_t i = 0; i < numBlocks; i++) {
			if (i != rootId) {
				// Only check blocks of provided template type
				auto block = GetBlock<T>(i);
				if (block && !IsBlockReferenced(i)) {
					DeleteBlock(i);

					if (deletionCount)
						(*deletionCount)++;

					// Deleting a block can cause others to become unreferenced
					return DeleteUnreferencedBlocks<T>(rootId > i ? rootId - 1 : rootId, deletionCount);
				}
			}
		}

		return true;
	}

	uint16_t AddOrFindBlockTypeId(const std::string& blockTypeName);
	std::string GetBlockTypeStringById(const uint32_t blockId) const;
	uint16_t GetBlockTypeIndex(const uint32_t blockId) const;

	uint32_t GetBlockSize(const uint32_t blockId) const;
	std::streampos GetBlockSizeStreamPos() const;
	void ResetBlockSizeStreamPos();

	uint32_t GetStringCount() const override;
	uint32_t FindStringId(const std::string& str) const override;

	// Adds a new string to the header (or finds a matching one).
	// "addEmpty" allows for adding an empty string, which is usually not required.
	// Returns the string index that can then be assigned to a block's member.
	uint32_t AddOrFindStringId(const std::string& str, const bool addEmpty = false) override;

	// Returns string at the specified string index (or empty string)
	std::string GetStringById(const uint32_t id) const override;

	// Sets string at the specified string index (or does nothing)
	void SetStringById(const uint32_t id, const std::string& str) override;

	void ClearStrings();
	void UpdateMaxStringLength();

	// Fills all string references with their corresponding header string (index -> string)
	void FillStringRefs();

	// Creates header strings for all string references or updates existing ones (string -> index)
	void UpdateHeaderStrings(const bool hasUnknown);

	static void BlockDeleted(NiObject* o, const uint32_t blockId);

	void Get(NiIStream& stream) override;
	void Put(NiOStream& stream) override;
};

// Used for all unknown block types
class NiUnknown : public NiCloneableStreamable<NiUnknown, NiObject> {
public:
	std::vector<char> data;

	NiUnknown() {}
	NiUnknown(NiIStream& stream, const uint32_t size);
	NiUnknown(const uint32_t size);

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
