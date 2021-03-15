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

	std::string GetVersionInfo();
	std::string String() { return vstr; }

	NiFileVersion File() { return file; }
	void SetFile(NiFileVersion fileVer);

	uint32_t User() { return user; }
	void SetUser(const uint32_t userVer) { user = userVer; }

	uint32_t Stream() { return stream; }
	void SetStream(const uint32_t streamVer) { stream = streamVer; }

	uint32_t NDS() { return nds; }
	void SetNDS(const uint32_t ndsVer) { nds = ndsVer; }

	bool IsBethesda() { return (file == V20_2_0_7 && user >= 11) || IsOB(); }

	bool IsOB() {
		return ((file == V10_1_0_106 || file == V10_2_0_0) && user >= 3 && user < 11)
			   || (file == V20_0_0_4 && (user == 10 || user == 11)) || (file == V20_0_0_5 && user == 11);
	}

	bool IsFO3() { return file == V20_2_0_7 && stream > 11 && stream < 83; }
	bool IsSK() { return file == V20_2_0_7 && stream == 83; }
	bool IsSSE() { return file == V20_2_0_7 && stream == 100; }
	bool IsFO4() { return file == V20_2_0_7 && stream == 130; }
	bool IsFO76() { return file == V20_2_0_7 && stream == 155; }

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
	Mode GetMode() { return mode; }

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

template<typename Derived, typename Base, bool GetPut = false>
class NiObjectCRTP;

template<typename Derived, typename Base>
class NiObjectCRTP<Derived, Base, false> : public Base {
public:
	virtual ~NiObjectCRTP() = default;

	std::unique_ptr<Derived> Clone() const {
		return std::unique_ptr<Derived>(static_cast<Derived*>(this->Clone_impl()));
	}

private:
	virtual NiObjectCRTP* Clone_impl() const override { return new Derived(asDer()); }

	Derived& asDer() { return static_cast<Derived&>(*this); }
	const Derived& asDer() const { return static_cast<const Derived&>(*this); }
};

template<typename Derived, typename Base>
class NiObjectCRTP<Derived, Base, true> : public NiObjectCRTP<Derived, Base, false> {
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

class NiString {
private:
	std::string str;
	bool nullOutput = false; // append a null byte when writing the string

public:
	NiString(){};

	std::string GetString() { return str; }
	void SetString(const std::string& s) { this->str = s; }

	size_t GetLength() { return str.length(); }

	void SetNullOutput(const bool wantNullOutput = true) { nullOutput = wantNullOutput; }
	void Clear() { str.clear(); }

	void Get(NiIStream& stream, const int szSize);
	void Put(NiOStream& stream, const int szSize);

	void Sync(NiStreamReversible& stream, const int szSize) {
		if (auto istream = stream.asRead())
			Get(*istream, szSize);
		else if (auto ostream = stream.asWrite())
			Put(*ostream, szSize);
	}
};

class StringRef {
private:
	// Temporary index storage for load/save
	int index = NIF_NPOS;
	NiString str;

public:
	std::string GetString() { return str.GetString(); }

	void SetString(const std::string& s) { str.SetString(s); }

	int GetIndex() { return index; }

	void SetIndex(const int id) { index = id; }

	void Clear() {
		index = NIF_NPOS;
		str.Clear();
	}

	void Sync(NiStreamReversible& stream) {
		if (stream.GetVersion().File() < V20_1_0_1)
			str.Sync(stream, 4);
		else
			stream.Sync(index);
	}
};

class Ref {
protected:
	int index = NIF_NPOS;

public:
	int GetIndex() { return index; }

	void SetIndex(const int id) { index = id; }

	void Clear() { index = NIF_NPOS; }
};

template<typename T>
class BlockRef : public Ref {
	using base = Ref;

public:
	BlockRef() {}
	BlockRef(const int id) { Ref::index = id; }

	void Sync(NiStreamReversible& stream) { stream.Sync(base::index); }
};

class RefArray {
protected:
	int arraySize = 0;
	bool keepEmptyRefs = false;

public:
	virtual ~RefArray() {}

	int GetSize() { return arraySize; }

	void SetKeepEmptyRefs(const bool keep = true) { keepEmptyRefs = keep; }

	virtual void Sync(NiStreamReversible& stream) = 0;

	virtual void AddBlockRef(const int id) = 0;
	virtual int GetBlockRef(const int id) = 0;
	virtual void SetBlockRef(const int id, const int index) = 0;
	virtual void RemoveBlockRef(const int id) = 0;
	virtual void GetIndices(std::vector<int>& indices) = 0;
	virtual void GetIndexPtrs(std::set<Ref*>& indices) = 0;
	virtual void SetIndices(const std::vector<int>& indices) = 0;
};

template<typename T>
class BlockRefArray : public RefArray {
protected:
	using RefArray::arraySize;
	using RefArray::keepEmptyRefs;

	std::vector<BlockRef<T>> refs;

	void CleanInvalidRefs() {
		if (keepEmptyRefs)
			return;

		refs.erase(std::remove_if(refs.begin(),
								  refs.end(),
								  [](BlockRef<T> r) {
									  if (r.GetIndex() == NIF_NPOS)
										  return true;
									  else
										  return false;
								  }),
				   refs.end());

		arraySize = refs.size();
	}

public:
	typedef typename std::vector<BlockRef<T>>::iterator iterator;
	typedef typename std::vector<BlockRef<T>>::const_iterator const_iterator;

	typename std::vector<BlockRef<T>>::iterator begin() { return refs.begin(); }

	typename std::vector<BlockRef<T>>::iterator end() { return refs.end(); }

	typename std::vector<BlockRef<T>>::const_iterator begin() const { return refs.begin(); }

	typename std::vector<BlockRef<T>>::const_iterator end() const { return refs.end(); }

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
		refs.push_back(BlockRef<T>(index));
		arraySize++;
	}

	int GetBlockRef(const int id) override {
		if (id >= 0 && refs.size() > id)
			return refs[id].GetIndex();

		return NIF_NPOS;
	}

	void SetBlockRef(const int id, const int index) override {
		if (id >= 0 && refs.size() > id)
			refs[id].SetIndex(index);
	}

	void RemoveBlockRef(const int id) override {
		if (id >= 0 && refs.size() > id) {
			refs.erase(refs.begin() + id);
			arraySize--;
		}
	}

	void GetIndices(std::vector<int>& indices) override {
		for (auto& r : refs)
			indices.push_back(r.GetIndex());
	}

	void GetIndexPtrs(std::set<Ref*>& indices) override {
		for (auto& r : refs)
			indices.insert(&r);
	}

	void SetIndices(const std::vector<int>& indices) override {
		arraySize = indices.size();
		refs.resize(arraySize);

		for (int i = 0; i < arraySize; i++)
			refs[i].SetIndex(indices[i]);
	}
};

template<typename T>
class BlockRefShortArray : public BlockRefArray<T> {
public:
	using base = BlockRefArray<T>;
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

	virtual void GetStringRefs(std::vector<StringRef*>&) {}
	virtual void GetChildRefs(std::set<Ref*>&) {}
	virtual void GetChildIndices(std::vector<int>&) {}
	virtual void GetPtrs(std::set<Ref*>&) {}

	std::unique_ptr<NiObject> Clone() const {
		return std::unique_ptr<NiObject>(static_cast<NiObject*>(this->Clone_impl()));
	}

	template<typename T>
	bool HasType() {
		return dynamic_cast<const T*>(this) != nullptr;
	}

private:
	virtual NiObject* Clone_impl() const = 0;
};

class NiHeader : public NiObjectCRTP<NiHeader, NiObject> {
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

	bool IsValid() { return valid; }

	NiVersion& GetVersion() { return version; };

	void SetVersion(const NiVersion& ver) { version = ver; }

	std::string GetCreatorInfo();
	void SetCreatorInfo(const std::string& creatorInfo);

	std::string GetExportInfo();
	void SetExportInfo(const std::string& exportInfo);

	void SetBlockReference(std::vector<std::unique_ptr<NiObject>>* blockRef) { blocks = blockRef; };

	uint32_t GetNumBlocks() { return numBlocks; }

	template<class T>
	T* GetBlock(const int blockId) {
		if (blockId >= 0 && blockId < numBlocks)
			return dynamic_cast<T*>((*blocks)[blockId].get());

		return nullptr;
	}

	template<class T>
	T* GetBlockUnsafe(const int blockId) {
		if (blockId >= 0 && blockId < numBlocks)
			return static_cast<T*>((*blocks)[blockId].get());

		return nullptr;
	}

	int GetBlockID(NiObject* block);

	void DeleteBlock(int blockId);
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
	std::string GetBlockTypeStringById(const int blockId);
	uint16_t GetBlockTypeIndex(const int blockId);

	uint32_t GetBlockSize(const uint32_t blockId);
	std::streampos GetBlockSizeStreamPos();
	void ResetBlockSizeStreamPos();

	int GetStringCount();
	int FindStringId(const std::string& str);
	int AddOrFindStringId(const std::string& str, const bool addEmpty = false);
	std::string GetStringById(const int id);
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

class NiUnknown : public NiObjectCRTP<NiUnknown, NiObject, true> {
private:
	std::vector<char> data;

public:
	NiUnknown() {}
	NiUnknown(NiIStream& stream, const uint32_t size);
	NiUnknown(const uint32_t size);

	void Sync(NiStreamReversible& stream);

	std::vector<char> GetData() { return data; }
};
} // namespace nifly
