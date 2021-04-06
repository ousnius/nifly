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
const auto NiVec3Min = Vector3(NiFloatMin, NiFloatMin, NiFloatMin);
const auto NiVec4Min = Vector4(NiFloatMin, NiFloatMin, NiFloatMin, NiFloatMin);

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

	bool IsBethesda() const { return (file == V20_2_0_7 && user >= 11) || IsOB(); }

	bool IsOB() const {
		return ((file == V10_1_0_106 || file == V10_2_0_0) && user >= 3 && user < 11)
			   || (file == V20_0_0_4 && (user == 10 || user == 11)) || (file == V20_0_0_5 && user == 11);
	}

	bool IsFO3() const { return file == V20_2_0_7 && stream > 11 && stream < 83; }
	bool IsSK() const { return file == V20_2_0_7 && stream == 83; }
	bool IsSSE() const { return file == V20_2_0_7 && stream == 100; }
	bool IsFO4() const { return file == V20_2_0_7 && stream == 130; }
	bool IsFO76() const { return file == V20_2_0_7 && stream == 155; }

	static NiVersion getOB() { return NiVersion(NiFileVersion::V20_0_0_5, 11, 0); }
	static NiVersion getFO3() { return NiVersion(NiFileVersion::V20_2_0_7, 12, 82); }
	static NiVersion getSK() { return NiVersion(NiFileVersion::V20_2_0_7, 12, 83); }
	static NiVersion getSSE() { return NiVersion(NiFileVersion::V20_2_0_7, 12, 100); }
	static NiVersion getFO4() { return NiVersion(NiFileVersion::V20_2_0_7, 12, 130); }
	static NiVersion getFO76() { return NiVersion(NiFileVersion::V20_2_0_7, 12, 155); }
};

enum NiEndian : uint8_t { ENDIAN_BIG, ENDIAN_LITTLE };

class NiStreamBase {
private:
	NiVersion version;

public:
	explicit NiStreamBase() {}
	explicit NiStreamBase(NiVersion v)
		: version(std::move(v)) {}

	NiVersion& GetVersion() { return version; }
	const NiVersion& GetVersion() const { return version; }
};

class NiIStream : public NiStreamBase {
private:
	std::istream* stream = nullptr;

public:
	NiIStream(std::istream* s)
		: stream(s) {}

	NiIStream(std::istream* s, NiVersion v)
		: NiStreamBase(std::move(v))
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
	int blockSize = 0;

public:
	NiOStream(std::ostream* s, NiVersion v)
		: NiStreamBase(std::move(v))
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
	int GetBlockSize() { return blockSize; }
};

class NiStreamReversible {
public:
	enum class Mode { Reading, Writing };

	explicit NiStreamReversible(NiIStream* is, NiOStream* os, Mode mode)
		: istream(is)
		, ostream(os)
		, mode(mode) {}

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
	int index = NIF_NPOS; // Temporary index storage for load/save

public:
	NiStringRef() = default;
	NiStringRef(const std::string& s) { str = s; }

	std::string& get() { return str; }
	const std::string& get() const { return str; }

	size_t length() const { return str.length(); }

	int GetIndex() const { return index; }
	void SetIndex(const int id) { index = id; }

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

	SizeType size() const { return vec.size(); }
	bool empty() const { return vec.empty(); }

	void clear() { vec.clear(); }

	auto begin() { return vec.begin(); }
	auto cbegin() const { return vec.begin(); }

	auto end() { return vec.end(); }
	auto cend() const { return vec.end(); }

	void resize(SizeType size) { vec.resize(size); }

	auto& operator[](SizeType i) { return vec[i]; }

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

	void Read(NiIStream& stream) {
		size_t sz = 0;
		stream.read(reinterpret_cast<char*>(&sz), NumSize);

		Base::resize(sz);

		for (auto& e : *this)
			stream >> e;
	}

	void Write(NiOStream& stream) {
		if (!Base::empty() && Base::size() - 1 > MaxIndex)
			Base::resize(MaxIndex + 1);

		size_t sz = Base::size();
		stream.write(reinterpret_cast<char*>(&sz), NumSize);

		for (auto& e : *this)
			stream << e;
	}

	void Sync(NiStreamReversible& stream) {
		if (auto istream = stream.asRead())
			Read(*istream);
		else if (auto ostream = stream.asWrite())
			Write(*ostream);
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
		size_t sz = 0;
		stream.read(reinterpret_cast<char*>(&sz), NumSize);

		Base::resize(sz);

		for (auto& e : *this)
			e.Read(stream, stringSize);
	}

	void Write(NiOStream& stream) {
		if (!Base::empty() && Base::size() - 1 > MaxIndex)
			Base::resize(MaxIndex + 1);

		size_t sz = Base::size();
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
		size_t sz = 0;
		stream.read(reinterpret_cast<char*>(&sz), NumSize);

		Base::resize(sz);

		for (auto& e : *this)
			e.Read(stream);
	}

	void Write(NiOStream& stream) {
		if (!Base::empty() && Base::size() - 1 > MaxIndex)
			Base::resize(MaxIndex + 1);

		size_t sz = Base::size();
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

class NiRef {
public:
	int index = NIF_NPOS;

	void Clear() { index = NIF_NPOS; }
	bool IsEmpty() const { return index == NIF_NPOS; }

	bool operator==(const NiRef& rhs) const { return index == rhs.index; }
	bool operator!=(const NiRef& rhs) const { return !operator==(rhs); }

	bool operator==(const int rhs) const { return index == rhs; }
	bool operator!=(const int rhs) const { return !operator==(rhs); }
};

using NiPtr = NiRef;

template<typename T>
class NiBlockRef : public NiRef {
	using base = NiRef;

public:
	NiBlockRef() {}
	NiBlockRef(const int id) { NiRef::index = id; }

	void Sync(NiStreamReversible& stream) { stream.Sync(base::index); }
};

template<typename T>
using NiBlockPtr = NiBlockRef<T>;

class NiRefArray {
protected:
	int arraySize = 0;
	bool keepEmptyRefs = false;

public:
	virtual ~NiRefArray() {}

	int GetSize() const { return arraySize; }

	void SetKeepEmptyRefs(const bool keep = true) { keepEmptyRefs = keep; }

	virtual void Sync(NiStreamReversible& stream) = 0;

	virtual void AddBlockRef(const int id) = 0;
	virtual int GetBlockRef(const int id) const = 0;
	virtual void SetBlockRef(const int id, const int index) = 0;
	virtual void RemoveBlockRef(const int id) = 0;
	virtual void GetIndices(std::vector<int>& indices) = 0;
	virtual void GetIndexPtrs(std::set<NiRef*>& indices) = 0;
	virtual void SetIndices(const std::vector<int>& indices) = 0;
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

		arraySize = refs.size();
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

	void SetSize(const int size) {
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

	void AddBlockRef(const int index) override {
		refs.push_back(NiBlockRef<T>(index));
		arraySize++;
	}

	int GetBlockRef(const int id) const override {
		if (id >= 0 && refs.size() > id)
			return refs[id].index;

		return NIF_NPOS;
	}

	void SetBlockRef(const int id, const int index) override {
		if (id >= 0 && refs.size() > id)
			refs[id].index = index;
	}

	void RemoveBlockRef(const int id) override {
		if (id >= 0 && refs.size() > id) {
			refs.erase(refs.begin() + id);
			arraySize--;
		}
	}

	void GetIndices(std::vector<int>& indices) override {
		for (auto& r : refs)
			indices.push_back(r.index);
	}

	void GetIndexPtrs(std::set<NiRef*>& indices) override {
		for (auto& r : refs)
			indices.insert(&r);
	}

	void SetIndices(const std::vector<int>& indices) override {
		arraySize = indices.size();
		refs.resize(arraySize);

		for (int i = 0; i < arraySize; i++)
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

public:
	virtual ~NiObject() = default;

	static constexpr const char* BlockName = "NiUnknown";
	virtual const char* GetBlockName() { return BlockName; }

	virtual void notifyVerticesDelete(const std::vector<uint16_t>&) {}

	virtual void Get(NiIStream&) {}
	virtual void Put(NiOStream&) {}

	virtual void GetStringRefs(std::vector<NiStringRef*>&) {}
	virtual void GetChildRefs(std::set<NiRef*>&) {}
	virtual void GetChildIndices(std::vector<int>&) {}
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

class NiHeader : public NiCloneable<NiHeader, NiObject> {
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
	bool valid = false;
	std::streampos blockSizePos;

	NiVersion version;
	NiEndian endian = ENDIAN_LITTLE;
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
	NiHeader(){};

	static constexpr const char* BlockName = "NiHeader";
	const char* GetBlockName() override { return BlockName; }

	void Clear();

	bool IsValid() const { return valid; }

	NiVersion& GetVersion() { return version; }
	const NiVersion& GetVersion() const { return version; }

	void SetVersion(const NiVersion& ver) { version = ver; }

	std::string GetCreatorInfo() const;
	void SetCreatorInfo(const std::string& creatorInfo);

	std::string GetExportInfo() const;
	void SetExportInfo(const std::string& exportInfo);

	void SetBlockReference(std::vector<std::unique_ptr<NiObject>>* blockRef) { blocks = blockRef; }

	uint32_t GetNumBlocks() const { return numBlocks; }

	template<class T>
	T* GetBlock(const int blockId) const {
		if (blockId >= 0 && blockId < numBlocks)
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
	T* GetBlockUnsafe(const int blockId) const {
		if (blockId >= 0 && blockId < numBlocks)
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

	int GetBlockID(NiObject* block) const;

	void DeleteBlock(int blockId);
	void DeleteBlock(const NiRef& blockRef);
	void DeleteBlockByType(const std::string& blockTypeStr, const bool orphanedOnly = false);
	int AddBlock(std::unique_ptr<NiObject> newBlock);
	int ReplaceBlock(int oldBlockId, std::unique_ptr<NiObject> newBlock);
	void SetBlockOrder(std::vector<int>& newOrder);
	void FixBlockAlignment(const std::vector<NiObject*>& currentTree);

	// Swaps two blocks, updating references in other blocks that may refer to their old indices
	void SwapBlocks(const int blockIndexLo, const int blockIndexHi);
	bool IsBlockReferenced(const int blockId);
	int GetBlockRefCount(const int blockId);

	template<class T>
	bool DeleteUnreferencedBlocks(const int rootId, int* deletionCount = nullptr) {
		if (rootId == NIF_NPOS)
			return false;

		for (int i = 0; i < numBlocks; i++) {
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
	std::string GetBlockTypeStringById(const int blockId) const;
	uint16_t GetBlockTypeIndex(const int blockId) const;

	uint32_t GetBlockSize(const uint32_t blockId) const;
	std::streampos GetBlockSizeStreamPos() const;
	void ResetBlockSizeStreamPos();

	int GetStringCount() const;
	int FindStringId(const std::string& str) const;
	int AddOrFindStringId(const std::string& str, const bool addEmpty = false);
	std::string GetStringById(const int id) const;
	void SetStringById(const int id, const std::string& str);

	void ClearStrings();
	void UpdateMaxStringLength();
	void FillStringRefs();
	void UpdateHeaderStrings(const bool hasUnknown);

	static void BlockDeleted(NiObject* o, int blockId);
	static void BlockSwapped(NiObject* o, int blockIndexLo, int blockIndexHi);

	void Get(NiIStream& stream) override;
	void Put(NiOStream& stream) override;
};

class NiUnknown : public NiCloneableStreamable<NiUnknown, NiObject> {
public:
	std::vector<char> data;

	NiUnknown() {}
	NiUnknown(NiIStream& stream, const uint32_t size);
	NiUnknown(const uint32_t size);

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
