/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "NifFile.hpp"
#include "bhk.hpp"
#include "NifUtil.hpp"

#include <fstream>
#include <regex>
#include <set>
#include <unordered_set>
#include <queue>

using namespace nifly;

uint32_t NifFile::GetBlockID(NiObject* block) const {
	auto it = find_if(blocks, [&block](const auto& ptr) { return ptr.get() == block; });

	if (it != blocks.end())
		return static_cast<uint32_t>(std::distance(blocks.begin(), it));

	return NIF_NPOS;
}

NiNode* NifFile::GetParentNode(NiObject* childBlock) const {
	if (childBlock != nullptr) {
		int childId = GetBlockID(childBlock);
		for (auto& block : blocks) {
			auto node = dynamic_cast<NiNode*>(block.get());
			if (node) {
				auto children = node->childRefs;
				for (auto& c : children) {
					if (c == childId)
						return node;
				}
			}
		}
	}

	return nullptr;
}

void NifFile::SetParentNode(NiObject* childBlock, NiNode* newParent) {
	if (!childBlock)
		return;

	if (!newParent) {
		newParent = GetRootNode();

		if (!newParent)
			return;
	}

	if (childBlock == newParent)
		return;

	uint32_t childId = GetBlockID(childBlock);
	for (auto& block : blocks) {
		auto node = dynamic_cast<NiNode*>(block.get());
		if (!node)
			continue;

		auto& children = node->childRefs;
		for (uint32_t ci = 0; ci < children.GetSize(); ++ci) {
			if (childId != children.GetBlockRef(ci))
				continue;

			// We have now found the node's old parent
			if (newParent != node) {
				children.RemoveBlockRef(ci);
				newParent->childRefs.AddBlockRef(childId);
			}

			return;
		}
	}

	// If we get here, the node's old parent was not found.
	newParent->childRefs.AddBlockRef(childId);
}

std::vector<NiNode*> NifFile::GetNodes() const {
	std::vector<NiNode*> outList;
	for (auto& block : blocks) {
		auto node = dynamic_cast<NiNode*>(block.get());
		if (node)
			outList.push_back(node);
	}

	return outList;
}

void NifFile::CopyFrom(const NifFile& other) {
	if (isValid)
		Clear();

	isValid = other.isValid;
	hasUnknown = other.hasUnknown;
	isTerrain = other.isTerrain;

	hdr = NiHeader(other.hdr);

	size_t nBlocks = other.blocks.size();
	blocks.resize(nBlocks);

	for (uint32_t i = 0; i < nBlocks; i++)
		blocks[i] = other.blocks[i]->Clone();

	hdr.SetBlockReference(&blocks);
	LinkGeomData();
}

void NifFile::LinkGeomData() {
	for (auto& block : blocks) {
		if (auto geom = dynamic_cast<NiGeometry*>(block.get())) {
			// NiGeometry refers to geometry data within the nif file
			auto geomData = hdr.GetBlock(geom->DataRef());
			if (geomData)
				geom->SetGeomData(geomData);
			
		}
		// NOTE: BSGeometry is it's own geometry data... need explicit linking here?
	}
}

void NifFile::RemoveInvalidTris() const {
	for (auto& shape : GetShapes()) {
		std::vector<Triangle> tris;
		if (shape->GetTriangles(tris)) {
			uint16_t numVerts = shape->GetNumVertices();
			tris.erase(std::remove_if(tris.begin(),
									  tris.end(),
									  [&](auto& t) {
										  return t.p1 >= numVerts || t.p2 >= numVerts || t.p3 >= numVerts;
									  }),
					   tris.end());

			shape->SetTriangles(tris);
		}
	}
}

size_t NifFile::GetVertexLimit() {
	constexpr size_t maxVertIndex = std::numeric_limits<uint16_t>::max();
	return maxVertIndex;
}

size_t NifFile::GetTriangleLimit() const {
	size_t maxTriIndex = std::numeric_limits<uint32_t>::max();
	if (hdr.GetVersion().User() >= 12 && hdr.GetVersion().Stream() < 130)
		maxTriIndex = std::numeric_limits<uint16_t>::max();

	return maxTriIndex;
}

void NifFile::Create(const NiVersion& version) {
	Clear();
	hdr.SetVersion(version);
	hdr.SetBlockReference(&blocks);

	auto rootNode = std::make_unique<NiNode>();
	rootNode->name.get() = "Scene Root";
	hdr.AddBlock(std::move(rootNode));

	isValid = true;
}

void NifFile::Clear() {
	isValid = false;
	hasUnknown = false;
	isTerrain = false;

	blocks.clear();
	hdr.Clear();
}

int NifFile::Load(const std::filesystem::path& fileName, const NifLoadOptions& options) {
	std::ifstream file(fileName, std::ios::in | std::ios::binary);
	return Load(file, options);
}

int NifFile::Load(std::istream& file, const NifLoadOptions& options) {
	Clear();

	isTerrain = options.isTerrain;

	if (file) {
		NiIStream stream(&file, &hdr);
		hdr.Get(stream);

		if (!hdr.IsValid()) {
			Clear();
			return 1;
		}

		NiVersion& version = hdr.GetVersion();
		if (!(version.IsOB() || version.IsFO3() || version.IsSK() || version.IsSSE() || version.IsFO4() || version.IsFO76() || version.IsSF() || version.IsSpecial())) {
			// Unsupported file version
			Clear();
			return 2;
		}

		uint32_t nBlocks = hdr.GetNumBlocks();
		blocks.resize(nBlocks);

		auto& nifactories = NiFactoryRegister::Get();
		for (uint32_t i = 0; i < nBlocks; i++) {
			std::string blockTypeStr = hdr.GetBlockTypeStringById(i);

			auto nifactory = nifactories.GetFactoryByName(blockTypeStr);
			if (nifactory) {
				blocks[i] = nifactory->Load(stream);
			}
			else {
				if (version.File() < V20_2_0_5) {
					// Loading unknown blocks w/o block sizes isn't possible
					Clear();
					return 3;
				}

				hasUnknown = true;
				blocks[i] = std::make_unique<NiUnknown>(stream, hdr.GetBlockSize(i));
			}
		}

		hdr.SetBlockReference(&blocks);
	}
	else {
		Clear();
		return 1;
	}

	PrepareData();
	isValid = true;
	return 0;
}

void NifFile::SetShapeOrder(const std::vector<std::string>& order) {
	if (hasUnknown)
		return;

	if (order.empty())
		return;

	auto shapes = GetShapes();
	if (order.size() != shapes.size())
		return;

	SortState sortState{};
	sortState.newIndices.resize(hdr.GetNumBlocks());
	for (size_t i = 0; i < sortState.newIndices.size(); i++)
		sortState.newIndices[i] = static_cast<uint32_t>(i);

	for (auto& s : order) {
		auto shape = FindBlockByName<NiShape>(s);
		if (shape)
			sortState.rootShapeOrder.push_back(GetBlockID(shape));
	}

	auto root = GetRootNode();
	if (root) {
		sortState.newIndex = GetBlockID(root);
		SetSortIndices(sortState.newIndex, sortState);
	}

	for (size_t i = 0; i < sortState.newIndices.size(); i++) {
		uint32_t index = static_cast<uint32_t>(i);
		if (sortState.visitedIndices.count(index) == 0) {
			sortState.newIndices[i] = sortState.newIndex++;
			sortState.visitedIndices.insert(index);
		}
	}

	hdr.SetBlockOrder(sortState.newIndices);
}

void NifFile::SetSortIndices(const NiRef& ref, SortState& sortState) {
	SetSortIndices(ref.index, sortState);
}

void NifFile::SetSortIndices(const NiRef* ref, SortState& sortState) {
	if (ref)
		SetSortIndices(ref->index, sortState);
}

void NifFile::SetSortIndices(uint32_t refIndex, SortState& sortState) {
	auto obj = hdr.GetBlock<NiObject>(refIndex);
	if (!obj)
		return;

	bool fullySorted = sortState.visitedIndices.count(refIndex) > 0;

	if (!fullySorted) {
		auto collision = dynamic_cast<NiCollisionObject*>(obj);
		if (collision) {
			SortCollision(collision, refIndex, sortState);
			fullySorted = true;
		}
		else {
			// Assign new sort index
			sortState.newIndices[refIndex] = sortState.newIndex++;
			sortState.visitedIndices.insert(refIndex);
		}
	}

	if (!fullySorted) {
		auto node = dynamic_cast<NiNode*>(obj);
		if (node) {
			SortGraph(node, sortState);
			fullySorted = true;
		}
	}

	if (!fullySorted) {
		auto shape = dynamic_cast<NiShape*>(obj);
		if (shape) {
			SortShape(shape, sortState);
			fullySorted = true;
		}
	}

	if (!fullySorted) {
		auto controller = dynamic_cast<NiTimeController*>(obj);
		if (controller) {
			SortController(controller, sortState);
			fullySorted = true;
		}
	}

	if (!fullySorted) {
		auto shader = dynamic_cast<NiShader*>(obj);
		if (shader) {
			SortNiObjectNET(shader, sortState);
			SetSortIndices(shader->TextureSetRef(), sortState);
			fullySorted = true;
		}
	}

	if (!fullySorted) {
		// Default child sorting
		std::vector<uint32_t> childIndices;
		obj->GetChildIndices(childIndices);

		for (auto& child : childIndices)
			SetSortIndices(child, sortState);

		fullySorted = true;
	}
}

void NifFile::SortNiObjectNET(NiObjectNET* objnet, SortState& sortState) {
	for (auto& r : objnet->extraDataRefs)
		SetSortIndices(r, sortState);

	SetSortIndices(objnet->controllerRef, sortState);

	auto controller = hdr.GetBlock<NiTimeController>(objnet->controllerRef);
	if (controller)
		SortController(controller, sortState);
}

void NifFile::SortAVObject(NiAVObject* avobj, SortState& sortState) {
	SortNiObjectNET(avobj, sortState);

	for (auto& r : avobj->propertyRefs)
		SetSortIndices(r, sortState);

	auto col = hdr.GetBlock<NiCollisionObject>(avobj->collisionRef);
	if (col)
		SortCollision(col, avobj->collisionRef.index, sortState);
}

void NifFile::SortController(NiTimeController* controller, SortState& sortState) {
	std::vector<uint32_t> childIndices;
	controller->GetChildIndices(childIndices);

	for (auto& index : childIndices) {
		SetSortIndices(index, sortState);

		auto controllerSequence = hdr.GetBlock<NiControllerSequence>(index);
		if (controllerSequence) {
			for (auto& cb : controllerSequence->controlledBlocks) {
				auto interp = hdr.GetBlock<NiInterpolator>(cb.interpolatorRef);
				if (interp)
					SetSortIndices(cb.interpolatorRef, sortState);

				auto subController = hdr.GetBlock<NiTimeController>(cb.controllerRef);
				if (subController)
					SetSortIndices(cb.controllerRef, sortState);
			}

			SetSortIndices(controllerSequence->textKeyRef, sortState);

			auto animNotes = hdr.GetBlock<BSAnimNotes>(controllerSequence->animNotesRef);
			if (animNotes) {
				SetSortIndices(controllerSequence->animNotesRef, sortState);

				for (auto& an : animNotes->animNoteRefs)
					SetSortIndices(an, sortState);
			}

			for (auto& ar : controllerSequence->animNotesRefs) {
				animNotes = hdr.GetBlock<BSAnimNotes>(ar);
				if (animNotes) {
					SetSortIndices(ar, sortState);

					for (auto& an : animNotes->animNoteRefs)
						SetSortIndices(an, sortState);
				}
			}
		}
	}
}

void NifFile::SortCollision(NiObject* parent, uint32_t parentIndex, SortState& sortState) {
	auto constraint = dynamic_cast<bhkConstraint*>(parent);
	if (constraint) {
		for (auto& entityId : constraint->entityRefs) {
			auto entity = hdr.GetBlock<NiObject>(entityId);
			if (entity && sortState.visitedIndices.count(entityId.index) == 0)
				SortCollision(entity, entityId.index, sortState);
		}
	}

	auto constraintChain = dynamic_cast<bhkBallSocketConstraintChain*>(parent);
	if (constraintChain) {
		for (auto& entityId : constraintChain->chainedEntityRefs) {
			auto entity = hdr.GetBlock<NiObject>(entityId);
			if (entity && sortState.visitedIndices.count(entityId.index) == 0)
				SortCollision(entity, entityId.index, sortState);
		}

		auto entityA = hdr.GetBlock<NiObject>(constraintChain->entityARef);
		if (entityA && sortState.visitedIndices.count(constraintChain->entityARef.index) == 0)
			SortCollision(entityA, constraintChain->entityARef.index, sortState);

		auto entityB = hdr.GetBlock<NiObject>(constraintChain->entityBRef);
		if (entityB && sortState.visitedIndices.count(constraintChain->entityBRef.index) == 0)
			SortCollision(entityB, constraintChain->entityBRef.index, sortState);
	}

	std::vector<uint32_t> childIndices;
	parent->GetChildIndices(childIndices);

	for (auto& id : childIndices) {
		auto child = hdr.GetBlock<NiObject>(id);
		if (child && sortState.visitedIndices.count(id) == 0) {
			bool childBeforeParent = child->HasType<bhkRefObject>() && !child->HasType<bhkConstraint>()
									 && !child->HasType<bhkBallSocketConstraintChain>();
			if (childBeforeParent)
				SortCollision(child, id, sortState);
		}
	}

	// Assign new sort index
	if (sortState.visitedIndices.count(parentIndex) == 0) {
		sortState.newIndices[parentIndex] = sortState.newIndex++;
		sortState.visitedIndices.insert(parentIndex);
	}

	for (auto& id : childIndices) {
		auto child = hdr.GetBlock<NiObject>(id);
		if (child && sortState.visitedIndices.count(id) == 0) {
			bool childBeforeParent = child->HasType<bhkRefObject>() && !child->HasType<bhkConstraint>()
									 && !child->HasType<bhkBallSocketConstraintChain>();
			if (!childBeforeParent)
				SortCollision(child, id, sortState);
		}
	}
}

void NifFile::SortShape(NiShape* shape, SortState& sortState) {
	SortAVObject(shape, sortState);

	SetSortIndices(shape->DataRef(), sortState);
	SetSortIndices(shape->SkinInstanceRef(), sortState);

	auto niSkinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (niSkinInst) {
		SetSortIndices(niSkinInst->dataRef, sortState);
		SetSortIndices(niSkinInst->skinPartitionRef, sortState);
	}

	auto bsSkinInst = hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef());
	if (bsSkinInst)
		SetSortIndices(bsSkinInst->dataRef, sortState);

	SetSortIndices(shape->ShaderPropertyRef(), sortState);
	SetSortIndices(shape->AlphaPropertyRef(), sortState);

	std::vector<uint32_t> remainingChildIndices;
	shape->GetChildIndices(remainingChildIndices);

	// Sort remaining children
	for (auto& child : remainingChildIndices)
		SetSortIndices(child, sortState);
}

void NifFile::SortGraph(NiNode* root, SortState& sortState) {
	bool isRootNode = GetBlockID(root) == 0;
	SortAVObject(root, sortState);

	std::vector<uint32_t> childIndices;
	root->childRefs.GetIndices(childIndices);

	if (childIndices.empty())
		return;

	bool reorderChildRefs = !root->HasType<BSOrderedNode>();
	if (reorderChildRefs) {
		std::vector<uint32_t> newChildIndices;
		newChildIndices.reserve(childIndices.size());

		NiBlockRefArray<NiAVObject> newChildRefs;

		if (hdr.GetVersion().IsOB() || hdr.GetVersion().IsFO3()) {
			// Order for OB/FO3:
			// 1. Nodes with children
			// 2. Shapes
			// 3. other

			// Add nodes with children
			for (auto& index : childIndices) {
				auto node = hdr.GetBlock<NiNode>(index);
				if (node && node->childRefs.GetSize() > 0) {
					newChildIndices.push_back(index);
					newChildRefs.AddBlockRef(index);
				}
			}

			// Add shapes
			std::vector<uint32_t> shapeIndices;
			for (auto& index : childIndices) {
				auto shape = hdr.GetBlock<NiShape>(index);
				if (shape)
					shapeIndices.push_back(index);
			}

			if (isRootNode) {
				// Reorder shapes on root node if order is provided
				if (sortState.rootShapeOrder.size() == shapeIndices.size()) {
					std::vector<uint32_t> newShapeIndices(shapeIndices.size());
					for (size_t si = 0; si < sortState.rootShapeOrder.size(); si++) {
						auto it = find(shapeIndices, sortState.rootShapeOrder[si]);
						if (it != shapeIndices.end())
							newShapeIndices[si] = shapeIndices[std::distance(shapeIndices.begin(), it)];
					}
					shapeIndices = newShapeIndices;
				}
			}

			for (auto& index : shapeIndices) {
				newChildIndices.push_back(index);
				newChildRefs.AddBlockRef(index);
			}
		}
		else {
			// Order:
			// 1. Nodes
			// 2. Shapes
			// 3. other

			// Add nodes
			for (auto& index : childIndices) {
				auto node = hdr.GetBlock<NiNode>(index);
				if (node) {
					newChildIndices.push_back(index);
					newChildRefs.AddBlockRef(index);
				}
			}

			// Add shapes
			std::vector<uint32_t> shapeIndices;
			for (auto& index : childIndices) {
				auto shape = hdr.GetBlock<NiShape>(index);
				if (shape)
					shapeIndices.push_back(index);
			}

			if (isRootNode) {
				// Reorder shapes on root node if order is provided
				if (sortState.rootShapeOrder.size() == shapeIndices.size()) {
					std::vector<uint32_t> newShapeIndices(shapeIndices.size());
					for (size_t si = 0; si < sortState.rootShapeOrder.size(); si++) {
						auto it = find(shapeIndices, sortState.rootShapeOrder[si]);
						if (it != shapeIndices.end())
							newShapeIndices[si] = shapeIndices[std::distance(shapeIndices.begin(), it)];
					}
					shapeIndices = newShapeIndices;
				}
			}

			for (auto& index : shapeIndices) {
				newChildIndices.push_back(index);
				newChildRefs.AddBlockRef(index);
			}
		}

		// Add missing others
		for (auto& index : childIndices) {
			if (!contains(newChildIndices, index)) {
				auto obj = hdr.GetBlock<NiObject>(index);
				if (obj) {
					newChildIndices.push_back(index);
					newChildRefs.AddBlockRef(index);
				}
			}
		}

		// Add empty refs
		for (auto& index : childIndices) {
			if (index == NIF_NPOS) {
				newChildIndices.push_back(index);
				newChildRefs.AddBlockRef(index);
			}
		}

		// Assign child ref array with new order
		root->childRefs = newChildRefs;
	}

	std::vector<uint32_t> remainingChildIndices;
	root->GetChildIndices(remainingChildIndices);

	// Sort remaining children
	for (auto& child : remainingChildIndices)
		SetSortIndices(child, sortState);
}

void NifFile::PrettySortBlocks() {
	if (hasUnknown)
		return;

	SortState sortState{};
	sortState.newIndices.resize(hdr.GetNumBlocks());
	for (size_t i = 0; i < sortState.newIndices.size(); i++)
		sortState.newIndices[i] = static_cast<uint32_t>(i);

	if (sortState.newIndices.empty())
		return;

	for (auto& node : GetNodes()) {
		auto parentNode = GetParentNode(node);
		if (!parentNode) {
			// No parent, node is at the root level
			SetSortIndices(GetBlockID(node), sortState);
		}
	}

	for (size_t i = 0; i < sortState.newIndices.size(); i++) {
		uint32_t index = static_cast<uint32_t>(i);
		if (sortState.visitedIndices.count(index) == 0) {
			sortState.newIndices[i] = sortState.newIndex++;
			sortState.visitedIndices.insert(index);
		}
	}

	hdr.SetBlockOrder(sortState.newIndices);
}

void NifFile::FixBSXFlags() {
	auto bsx = FindBlockByName<BSXFlags>("BSX");
	if (bsx) {
		if (bsx->integerData & BSX_EXTERNAL_EMITTANCE) {
			// BSXFlags external emittance = on. Check if any shaders require that.
			bool flagUnnecessary = true;

			for (auto& block : blocks) {
				auto bssp = dynamic_cast<BSShaderProperty*>(block.get());
				if (bssp) {
					if (bssp->shaderFlags1 & SLSF1_EXTERNAL_EMITTANCE) { // Same flag in SK and FO4
						flagUnnecessary = false;
						break;
					}
				}
			}

			if (flagUnnecessary)
			{
				// Unset unnecessary external emittance flag on BSXFlags
				bsx->integerData &= (~BSX_EXTERNAL_EMITTANCE);
			}
		}
		else {
			// BSXFlags external emittance = off. Check if any shaders have it set regardless.
			bool flagMissing = false;

			for (auto& block : blocks) {
				auto bssp = dynamic_cast<BSShaderProperty*>(block.get());
				if (bssp) {
					if (bssp->shaderFlags1 & SLSF1_EXTERNAL_EMITTANCE) { // Same flag in SK and FO4
						flagMissing = true;
						break;
					}
				}
			}

			if (flagMissing)
			{
				// Set missing external emittance flag on BSXFlags
				bsx->integerData |= BSX_EXTERNAL_EMITTANCE;
			}
		}
	}
}

void NifFile::FixShaderFlags() {
	for (auto& block : blocks) {
		auto bslsp = dynamic_cast<BSLightingShaderProperty*>(block.get());
		if (bslsp) {
			if (bslsp->bslspShaderType != BSLSP_ENVMAP && (bslsp->shaderFlags1 & SLSF1_ENVIRONMENT_MAPPING)) { // Same flag in SK and FO4
				// Shader is no environment shader, remove unused shader flag
				bslsp->shaderFlags1 &= (~SLSF1_ENVIRONMENT_MAPPING);
			}
			else if (bslsp->bslspShaderType == BSLSP_ENVMAP && !(bslsp->shaderFlags1 & SLSF1_ENVIRONMENT_MAPPING)) { // Same flag in SK and FO4
				// Shader is environment shader, add missing shader flag
				bslsp->shaderFlags1 |= SLSF1_ENVIRONMENT_MAPPING;
			}
		}
	}
}

bool NifFile::DeleteUnreferencedNodes(int* deletionCount) {
	if (hasUnknown)
		return false;

	auto root = GetRootNode();
	if (!root)
		return false;

	for (auto& node : GetNodes()) {
		if (node == root)
			continue;

		uint32_t blockId = GetBlockID(node);
		if (blockId == NIF_NPOS)
			continue;

		if (!CanDeleteNode(node))
			continue;

		if (hdr.GetBlockRefCount(blockId) < 2) {
			hdr.DeleteBlock(blockId);

			if (deletionCount)
				(*deletionCount)++;

			// Deleting a block can cause others to become unreferenced
			return DeleteUnreferencedNodes(deletionCount);
		}
	}

	return true;
}

NiNode* NifFile::AddNode(const std::string& nodeName, const MatTransform& xformToParent, NiNode* parent) {
	if (!parent)
		parent = GetRootNode();
	if (!parent)
		return nullptr;

	auto newNode = std::make_unique<NiNode>();
	newNode->name.get() = nodeName;
	newNode->SetTransformToParent(xformToParent);

	uint32_t newNodeId = hdr.AddBlock(std::move(newNode));
	if (newNodeId != NIF_NPOS)
		parent->childRefs.AddBlockRef(newNodeId);

	return hdr.GetBlockUnsafe<NiNode>(newNodeId);
}

void NifFile::DeleteNode(const std::string& nodeName) {
	hdr.DeleteBlock(GetBlockID(FindBlockByName<NiNode>(nodeName)));
}

bool NifFile::CanDeleteNode(NiNode* node) {
	if (!node)
		return false;

	std::set<NiRef*> refs;
	node->GetChildRefs(refs);

	// Only delete if the node has no child refs
	return std::all_of(refs.cbegin(), refs.cend(), [](auto&& ref) { return ref->IsEmpty(); });
}

bool NifFile::CanDeleteNode(const std::string& nodeName) const {
	auto node = FindBlockByName<NiNode>(nodeName);
	return CanDeleteNode(node);
}

std::string NifFile::GetNodeName(const uint32_t blockID) const {
	std::string name;

	auto n = hdr.GetBlock<NiNode>(blockID);
	if (n) {
		name = n->name.get();
		if (name.empty())
			name = "_unnamed_";
	}

	return name;
}

void NifFile::SetNodeName(const uint32_t blockID, const std::string& newName) {
	auto node = hdr.GetBlock<NiNode>(blockID);
	if (!node)
		return;

	node->name.get() = newName;
}

uint32_t NifFile::AssignExtraData(NiAVObject* target, std::unique_ptr<NiExtraData> extraData) {
	uint32_t extraDataId = hdr.AddBlock(std::move(extraData));
	target->extraDataRefs.AddBlockRef(extraDataId);
	return extraDataId;
}

NiShader* NifFile::GetShader(NiShape* shape) const {
	auto shader = hdr.GetBlock<NiShader>(shape->ShaderPropertyRef());
	if (shader)
		return shader;

	for (auto& prop : shape->propertyRefs) {
		auto shaderProp = hdr.GetBlock<NiShader>(prop);
		if (shaderProp) {
			shader = shaderProp;

			// Only return NiMaterialProperty if no other shader blocks are found
			if (!shaderProp->HasType<NiMaterialProperty>())
				return shaderProp;
		}
	}

	return shader;
}

NiMaterialProperty* NifFile::GetMaterialProperty(NiShape* shape) const {
	for (auto& prop : shape->propertyRefs) {
		auto material = hdr.GetBlock<NiMaterialProperty>(prop);
		if (material)
			return material;
	}

	return nullptr;
}

NiStencilProperty* NifFile::GetStencilProperty(NiShape* shape) const {
	for (auto& prop : shape->propertyRefs) {
		auto stencil = hdr.GetBlock<NiStencilProperty>(prop);
		if (stencil)
			return stencil;
	}

	return nullptr;
}

NiTexturingProperty* NifFile::GetTexturingProperty(NiShape* shape) const {
	for (auto& prop : shape->propertyRefs) {
		auto texturingProp = hdr.GetBlock<NiTexturingProperty>(prop);
		if (texturingProp)
			return texturingProp;
	}

	return nullptr;
}


NiGeometryData* NifFile::GetGeometryData(NiShape* shape) const {
	if (shape->HasType<NiTriBasedGeom>()) {
		return hdr.GetBlock<NiGeometryData>(shape->DataRef());
	}
	else if (shape->HasType<BSGeometry>()) {
		return static_cast<BSGeometry*>(shape)->GetGeomData();
	}
	return nullptr;
}

std::vector<std::reference_wrapper<std::string>> NifFile::GetExternalGeometryPathRefs(NiShape* shape) const {
	std::vector<std::reference_wrapper<std::string>> meshPaths;
	auto bsgeo = dynamic_cast<BSGeometry*>(shape);
	if (bsgeo) {
		for (uint8_t i = 0; i < bsgeo->MeshCount(); i++) {
			auto mesh = bsgeo->SelectMesh(i);
			meshPaths.push_back(mesh->meshName.get());
			bsgeo->ReleaseMesh();
		}
	}
	return meshPaths;
}

bool NifFile::LoadExternalShapeData(NiShape* shape, std::istream& infile, uint8_t shapeIndex) {
	auto bsgeo = dynamic_cast<BSGeometry*>(shape);
	if (bsgeo && (shapeIndex < bsgeo->MeshCount())) {
		NiIStream meshStream(&infile, nullptr);
		NiStreamReversible s(&meshStream, nullptr, NiStreamReversible::Mode::Reading);
		auto mesh = bsgeo->SelectMesh(shapeIndex);
		mesh->meshData.Sync(s);
		bsgeo->ReleaseMesh();
	}
	return true;
}

bool NifFile::SaveExternalShapeData(NiShape* shape, std::ostream& outfile, uint8_t shapeIndex) {
	auto bsgeo = dynamic_cast<BSGeometry*>(shape);
	if (bsgeo && (shapeIndex < bsgeo->MeshCount())) {
		NiOStream meshStream(&outfile, nullptr);
		NiStreamReversible s(nullptr, &meshStream,NiStreamReversible::Mode::Reading);
		auto mesh = bsgeo->SelectMesh(shapeIndex);
		mesh->Sync(s);
		bsgeo->ReleaseMesh();
	}
	return true;
}


std::vector<std::reference_wrapper<std::string>> NifFile::GetTexturePathRefs(NiShape* shape) const {
	std::vector<std::reference_wrapper<std::string>> texturePaths;

	auto shader = GetShader(shape);
	if (shader) {
		auto textureSet = hdr.GetBlock(shader->TextureSetRef());
		if (textureSet) {
			for (auto& t : textureSet->textures)
				texturePaths.push_back(t.get());
		}

		auto effectShader = dynamic_cast<BSEffectShaderProperty*>(shader);
		if (effectShader) {
			texturePaths.push_back(effectShader->sourceTexture.get());
			texturePaths.push_back(effectShader->normalTexture.get());
			texturePaths.push_back(effectShader->greyscaleTexture.get());
			texturePaths.push_back(effectShader->envMapTexture.get());
			texturePaths.push_back(effectShader->envMaskTexture.get());
		}
	}

	// Get texture path from referenced NiSourceTexture block
	auto pushSourceTexturePath = [&hdr = hdr, &texturePaths](const NiBlockRef<NiSourceTexture>& sourceRef) {
		auto sourceTexture = hdr.GetBlock(sourceRef);
		if (sourceTexture)
			texturePaths.push_back(sourceTexture->fileName.get());
	};

	// NiTexturingProperty and NiSourceTexture for OB
	auto texturingProp = GetTexturingProperty(shape);
	if (texturingProp) {
		if (texturingProp->hasBaseTex)
			pushSourceTexturePath(texturingProp->baseTex.sourceRef);

		if (texturingProp->hasDarkTex)
			pushSourceTexturePath(texturingProp->darkTex.sourceRef);

		if (texturingProp->hasDetailTex)
			pushSourceTexturePath(texturingProp->detailTex.sourceRef);

		if (texturingProp->hasGlossTex)
			pushSourceTexturePath(texturingProp->glossTex.sourceRef);

		if (texturingProp->hasGlowTex)
			pushSourceTexturePath(texturingProp->glowTex.sourceRef);

		if (texturingProp->hasBumpTex)
			pushSourceTexturePath(texturingProp->bumpTex.sourceRef);

		if (texturingProp->hasDecalTex0)
			pushSourceTexturePath(texturingProp->decalTex0.sourceRef);

		if (texturingProp->hasDecalTex1)
			pushSourceTexturePath(texturingProp->decalTex1.sourceRef);

		if (texturingProp->hasDecalTex2)
			pushSourceTexturePath(texturingProp->decalTex2.sourceRef);

		if (texturingProp->hasDecalTex3)
			pushSourceTexturePath(texturingProp->decalTex3.sourceRef);
	}

	return texturePaths;
}

uint32_t NifFile::GetTextureSlot(NiShape* shape, std::string& outTexFile, uint32_t texIndex) const {
	outTexFile.clear();

	auto shader = GetShader(shape);
	if (shader) {
		auto textureSet = hdr.GetBlock(shader->TextureSetRef());
		if (textureSet && texIndex + 1 <= textureSet->textures.size()) {
			outTexFile = textureSet->textures[texIndex].get();
			return 1;
		}

		if (!textureSet) {
			auto effectShader = dynamic_cast<BSEffectShaderProperty*>(shader);
			if (effectShader) {
				switch (texIndex) {
					case 0: outTexFile = effectShader->sourceTexture.get(); break;
					case 1: outTexFile = effectShader->normalTexture.get(); break;
					case 3: outTexFile = effectShader->greyscaleTexture.get(); break;
					case 4: outTexFile = effectShader->envMapTexture.get(); break;
					case 5: outTexFile = effectShader->envMaskTexture.get(); break;
				}

				return 2;
			}
		}
	}

	// Get texture path from referenced NiSourceTexture block
	auto getSourceTexturePath = [&hdr = hdr](const NiBlockRef<NiSourceTexture>& sourceRef) -> std::string {
		auto sourceTexture = hdr.GetBlock(sourceRef);
		if (sourceTexture)
			return sourceTexture->fileName.get();

		return std::string();
	};

	// NiTexturingProperty and NiSourceTexture for OB
	auto texturingProp = GetTexturingProperty(shape);
	if (texturingProp && texturingProp->textureCount > texIndex) {
		switch (texIndex) {
			case 0:
				if (texturingProp->hasBaseTex)
					outTexFile = getSourceTexturePath(texturingProp->baseTex.sourceRef);
				break;
			case 1:
				if (texturingProp->hasDarkTex)
					outTexFile = getSourceTexturePath(texturingProp->darkTex.sourceRef);
				break;
			case 2:
				if (texturingProp->hasDetailTex)
					outTexFile = getSourceTexturePath(texturingProp->detailTex.sourceRef);
				break;
			case 3:
				if (texturingProp->hasGlossTex)
					outTexFile = getSourceTexturePath(texturingProp->glossTex.sourceRef);
				break;
			case 4:
				if (texturingProp->hasGlowTex)
					outTexFile = getSourceTexturePath(texturingProp->glowTex.sourceRef);
				break;
			case 5:
				if (texturingProp->hasBumpTex)
					outTexFile = getSourceTexturePath(texturingProp->bumpTex.sourceRef);
				break;
			case 6:
				if (texturingProp->hasDecalTex0)
					outTexFile = getSourceTexturePath(texturingProp->decalTex0.sourceRef);
				break;
			case 7:
				if (texturingProp->hasDecalTex1)
					outTexFile = getSourceTexturePath(texturingProp->decalTex1.sourceRef);
				break;
			case 8:
				if (texturingProp->hasDecalTex2)
					outTexFile = getSourceTexturePath(texturingProp->decalTex2.sourceRef);
				break;
			case 9:
				if (texturingProp->hasDecalTex3)
					outTexFile = getSourceTexturePath(texturingProp->decalTex3.sourceRef);
				break;
		}

		if (!outTexFile.empty())
			return 3;
	}

	return 0;
}

void NifFile::SetTextureSlot(NiShape* shape, std::string& inTexFile, uint32_t texIndex) {
	auto shader = GetShader(shape);
	if (shader) {
		auto textureSet = hdr.GetBlock(shader->TextureSetRef());
		if (textureSet && texIndex + 1 <= textureSet->textures.size()) {
			textureSet->textures[texIndex].get() = inTexFile;
			return;
		}

		if (!textureSet) {
			auto effectShader = dynamic_cast<BSEffectShaderProperty*>(shader);
			if (effectShader) {
				switch (texIndex) {
					case 0: effectShader->sourceTexture.get() = inTexFile; break;
					case 1: effectShader->normalTexture.get() = inTexFile; break;
					case 3: effectShader->greyscaleTexture.get() = inTexFile; break;
					case 4: effectShader->envMapTexture.get() = inTexFile; break;
					case 5: effectShader->envMaskTexture.get() = inTexFile; break;
				}
				return;
			}
		}
	}

	// Set texture path in referenced NiSourceTexture block
	auto setSourceTexturePath = [&hdr = hdr](const NiBlockRef<NiSourceTexture>& sourceRef,
											 const std::string& texturePath) {
		auto sourceTexture = hdr.GetBlock(sourceRef);
		if (sourceTexture)
			sourceTexture->fileName.get() = texturePath;
	};

	// NiTexturingProperty and NiSourceTexture for OB
	auto texturingProp = GetTexturingProperty(shape);
	if (texturingProp) {
		texturingProp->textureCount = texIndex + 1;

		switch (texIndex) {
			case 0:
				texturingProp->hasBaseTex = true;
				setSourceTexturePath(texturingProp->baseTex.sourceRef, inTexFile);
				break;
			case 1:
				texturingProp->hasDarkTex = true;
				setSourceTexturePath(texturingProp->darkTex.sourceRef, inTexFile);
				break;
			case 2:
				texturingProp->hasDetailTex = true;
				setSourceTexturePath(texturingProp->detailTex.sourceRef, inTexFile);
				break;
			case 3:
				texturingProp->hasGlossTex = true;
				setSourceTexturePath(texturingProp->glossTex.sourceRef, inTexFile);
				break;
			case 4:
				texturingProp->hasGlowTex = true;
				setSourceTexturePath(texturingProp->glowTex.sourceRef, inTexFile);
				break;
			case 5:
				texturingProp->hasBumpTex = true;
				setSourceTexturePath(texturingProp->bumpTex.sourceRef, inTexFile);
				break;
			case 6:
				texturingProp->hasDecalTex0 = true;
				setSourceTexturePath(texturingProp->decalTex0.sourceRef, inTexFile);
				break;
			case 7:
				texturingProp->hasDecalTex1 = true;
				setSourceTexturePath(texturingProp->decalTex1.sourceRef, inTexFile);
				break;
			case 8:
				texturingProp->hasDecalTex2 = true;
				setSourceTexturePath(texturingProp->decalTex2.sourceRef, inTexFile);
				break;
			case 9:
				texturingProp->hasDecalTex3 = true;
				setSourceTexturePath(texturingProp->decalTex3.sourceRef, inTexFile);
				break;
		}
	}
}

void NifFile::TrimTexturePaths() {
	auto fTrimPath = [&hdr = hdr, &isTerrain = isTerrain](std::string& tex) -> std::string& {
		if (tex.empty())
			return tex;

		// Trim whitespace characters (including newlines)
		trim_whitespace(tex);

		if (tex.empty())
			return tex;

		// Replace multiple slashes or forward slashes with one backslash
		tex = std::regex_replace(tex, std::regex("/+|\\\\+"), "\\");

		// Search for the first occurrence of "\textures\" (only if "textures\" isn't at the start)
		std::smatch match;
		std::regex pattern(R"(^(?!textures\\).*?\\textures\\)", std::regex_constants::icase);
	
		if (std::regex_search(tex, match, pattern))
			tex = tex.substr(match[0].length()); // Remove matched string

		// Remove all backslashes from the front
		tex = std::regex_replace(tex, std::regex("^\\\\+"), "");

		if (!hdr.GetVersion().IsOB() && !hdr.GetVersion().IsSpecial() && is_relative_path(tex)) {
			// If the path doesn't start with "textures\", add it to the front
			tex = std::regex_replace(tex,
									 std::regex("^(?!^textures\\\\)", std::regex_constants::icase),
									 "textures\\");
		}

		// If the path doesn't start with "Data\", add it to the front
		if (isTerrain && is_relative_path(tex)) {
			tex = std::regex_replace(tex, std::regex("^(?!^Data\\\\)", std::regex_constants::icase), "Data\\");
		}
		return tex;
	};

	// Trim texture path in referenced NiSourceTexture block
	auto trimSourceTexturePath = [&hdr = hdr,
								  &fTrimPath = fTrimPath](const NiBlockRef<NiSourceTexture>& sourceRef) {
		auto sourceTexture = hdr.GetBlock(sourceRef);
		if (sourceTexture) {
			std::string tex = sourceTexture->fileName.get();
			sourceTexture->fileName.get() = fTrimPath(tex);
		}
	};

	for (auto& shape : GetShapes()) {
		auto shader = GetShader(shape);
		if (shader) {
			auto textureSet = hdr.GetBlock(shader->TextureSetRef());
			if (textureSet) {
				for (auto& i : textureSet->textures) {
					std::string tex = i.get();
					i.get() = fTrimPath(tex);
				}

				auto effectShader = dynamic_cast<BSEffectShaderProperty*>(shader);
				if (effectShader) {
					std::string tex = effectShader->sourceTexture.get();
					effectShader->sourceTexture.get() = fTrimPath(tex);

					tex = effectShader->normalTexture.get();
					effectShader->normalTexture.get() = fTrimPath(tex);

					tex = effectShader->greyscaleTexture.get();
					effectShader->greyscaleTexture.get() = fTrimPath(tex);

					tex = effectShader->envMapTexture.get();
					effectShader->envMapTexture.get() = fTrimPath(tex);

					tex = effectShader->envMaskTexture.get();
					effectShader->envMaskTexture.get() = fTrimPath(tex);
				}
			}
		}

		// NiTexturingProperty and NiSourceTexture for OB
		auto texturingProp = GetTexturingProperty(shape);
		if (texturingProp) {
			if (texturingProp->hasBaseTex)
				trimSourceTexturePath(texturingProp->baseTex.sourceRef);
			if (texturingProp->hasDarkTex)
				trimSourceTexturePath(texturingProp->darkTex.sourceRef);
			if (texturingProp->hasDetailTex)
				trimSourceTexturePath(texturingProp->detailTex.sourceRef);
			if (texturingProp->hasGlossTex)
				trimSourceTexturePath(texturingProp->glossTex.sourceRef);
			if (texturingProp->hasGlowTex)
				trimSourceTexturePath(texturingProp->glowTex.sourceRef);
			if (texturingProp->hasBumpTex)
				trimSourceTexturePath(texturingProp->bumpTex.sourceRef);
			if (texturingProp->hasDecalTex0)
				trimSourceTexturePath(texturingProp->decalTex0.sourceRef);
			if (texturingProp->hasDecalTex1)
				trimSourceTexturePath(texturingProp->decalTex1.sourceRef);
			if (texturingProp->hasDecalTex2)
				trimSourceTexturePath(texturingProp->decalTex2.sourceRef);
			if (texturingProp->hasDecalTex3)
				trimSourceTexturePath(texturingProp->decalTex3.sourceRef);
		}
	}
}

void NifFile::CloneChildren(NiObject* block, NifFile* srcNif) {
	if (!srcNif)
		srcNif = this;

	// Assign new refs and strings, rebind ptrs where possible
	std::function<void(NiObject*, uint32_t, uint32_t)> cloneBlock =
		[&](NiObject* b, uint32_t parentOldId, uint32_t parentNewId) -> void {
		std::set<NiRef*> refs;
		b->GetChildRefs(refs);

		for (auto& r : refs) {
			auto srcChild = srcNif->hdr.GetBlock<NiObject>(r);
			if (srcChild) {
				auto destChildS = srcChild->Clone();
				auto destChild = destChildS.get();
				uint32_t destId = hdr.AddBlock(std::move(destChildS));

				uint32_t oldId = r->index;
				r->index = destId;

				std::vector<NiStringRef*> strRefs;
				destChild->GetStringRefs(strRefs);

				for (auto& str : strRefs) {
					int strId = hdr.AddOrFindStringId(str->get());
					str->SetIndex(strId);
				}

				if (parentOldId != NIF_NPOS) {
					std::set<NiRef*> ptrs;
					destChild->GetPtrs(ptrs);

					for (auto& p : ptrs)
						if (p->index == parentOldId)
							p->index = parentNewId;

					cloneBlock(destChild, parentOldId, parentNewId);
				}
				else
					cloneBlock(destChild, oldId, destId);
			}
		}
	};

	cloneBlock(block, NIF_NPOS, NIF_NPOS);
}

NiShape* NifFile::CloneShape(NiShape* srcShape, const std::string& destShapeName, NifFile* srcNif) {
	if (!srcNif)
		srcNif = this;

	if (!srcShape)
		return nullptr;

	auto rootNode = GetRootNode();
	auto srcRootNode = srcNif->GetRootNode();

	// Geometry
	auto destShapeS = srcShape->Clone();
	auto destShape = destShapeS.get();
	destShape->name.get() = destShapeName;

	int destId = hdr.AddBlock(std::move(destShapeS));
	if (srcNif == this) {
		// Assign copied geometry to the same parent
		auto parentNode = GetParentNode(srcShape);
		if (parentNode)
			parentNode->childRefs.AddBlockRef(destId);
	}
	else if (rootNode)
		rootNode->childRefs.AddBlockRef(destId);

	// Children
	CloneChildren(destShape, srcNif);

	// Geometry Data
	auto destGeomData = hdr.GetBlock<NiTriBasedGeomData>(destShape->DataRef());
	if (destGeomData)
		destShape->SetGeomData(destGeomData);

	// Shader
	auto destShader = GetShader(destShape);
	if (destShader) {
		if (hdr.GetVersion().IsSK() || hdr.GetVersion().IsSSE()) {
			// Kill normals and tangents
			if (destShader->IsModelSpace()) {
				destShape->SetNormals(false);
				destShape->SetTangents(false);
			}
		}
	}

	// Bones
	std::vector<std::string> srcBoneList;
	srcNif->GetShapeBoneList(srcShape, srcBoneList);

	auto destBoneCont = hdr.GetBlock(destShape->SkinInstanceRef());
	if (destBoneCont)
		destBoneCont->boneRefs.Clear();

	if (rootNode && srcRootNode) {
		std::function<void(NiNode*)> cloneNodes = [&](NiNode* srcNode) -> void {
			std::string boneName = srcNode->name.get();

			// Insert as root child by default
			NiNode* nodeParent = rootNode;

			// Look for existing node to use as parent instead
			auto srcNodeParent = srcNif->GetParentNode(srcNode);
			if (srcNodeParent) {
				auto parent = FindBlockByName<NiNode>(srcNodeParent->name.get());
				if (parent)
					nodeParent = parent;
			}

			auto node = FindBlockByName<NiNode>(boneName);
			uint32_t boneID = GetBlockID(node);
			if (!node) {
				// Clone missing node into the right parent
				boneID = CloneNamedNode(boneName, srcNif);
				nodeParent->childRefs.AddBlockRef(boneID);
			}
			else {
				// Move existing node to non-root parent
				auto oldParent = GetParentNode(node);
				if (oldParent && oldParent != nodeParent && nodeParent != rootNode) {
					MatTransform xformToParent;
					srcNif->GetNodeTransformToParent(boneName, xformToParent);

					std::set<NiRef*> childRefs;
					oldParent->GetChildRefs(childRefs);
					for (auto& ref : childRefs)
						if (ref->index == boneID)
							ref->Clear();

					nodeParent->childRefs.AddBlockRef(boneID);
					SetNodeTransformToParent(boneName, xformToParent);
				}
			}

			// Recurse children
			for (auto& child : srcNode->childRefs) {
				auto childNode = srcNif->hdr.GetBlock<NiNode>(child);
				if (childNode)
					cloneNodes(childNode);
			}
		};

		for (auto& child : srcRootNode->childRefs) {
			auto srcChildNode = srcNif->hdr.GetBlock<NiNode>(child);
			if (srcChildNode)
				cloneNodes(srcChildNode);
		}
	}

	// Add bones to container if used in skin
	if (destBoneCont) {
		for (auto& boneName : srcBoneList) {
			auto node = FindBlockByName<NiNode>(boneName);
			int boneID = GetBlockID(node);
			if (node)
				destBoneCont->boneRefs.AddBlockRef(boneID);
		}
	}
	return destShape;
}

uint32_t NifFile::CloneNamedNode(const std::string& nodeName, NifFile* srcNif) {
	if (!srcNif)
		srcNif = this;

	auto srcNode = srcNif->FindBlockByName<NiNode>(nodeName);
	if (!srcNode)
		return NIF_NPOS;

	auto destNode = srcNode->Clone();
	destNode->name.get() = nodeName;
	destNode->collisionRef.Clear();
	destNode->controllerRef.Clear();
	destNode->childRefs.Clear();
	destNode->effectRefs.Clear();

	return hdr.AddBlock(std::move(destNode));
}

int NifFile::Save(const std::filesystem::path& fileName, const NifSaveOptions& options) {
	std::ofstream file(fileName, std::ios::out | std::ios::binary);
	return Save(file, options);
}

int NifFile::Save(std::ostream& file, const NifSaveOptions& options) {
	if (file) {
		NiOStream stream(&file, &hdr);
		FinalizeData();

		if (options.optimize)
			Optimize();

		if (options.sortBlocks)
			PrettySortBlocks();

		hdr.UpdateHeaderStrings(hasUnknown);

		hdr.Put(stream);
		stream.InitBlockSize();

		// Retrieve block sizes from NiStream while writing
		std::vector<std::streamsize> blockSizes(hdr.GetNumBlocks());
		for (uint32_t i = 0; i < hdr.GetNumBlocks(); i++) {
			blocks[i]->Put(stream);
			blockSizes[i] = stream.GetBlockSize();
			stream.InitBlockSize();
		}

		uint32_t endPad = 1;
		stream << endPad;
		endPad = 0;
		stream << endPad;

		// Get previous stream pos of block size array and overwrite
		std::streampos blockSizePos = hdr.GetBlockSizeStreamPos();
		if (blockSizePos != std::streampos()) {
			file.seekp(blockSizePos);

			for (uint32_t i = 0; i < hdr.GetNumBlocks(); i++)
				stream << static_cast<uint32_t>(blockSizes[i]);

			hdr.ResetBlockSizeStreamPos();
		}
	}
	else
		return 1;

	return 0;
}

void NifFile::Optimize() {
	for (auto& s : GetShapes())
		s->UpdateBounds();

	DeleteUnreferencedBlocks();
}

OptResult NifFile::OptimizeFor(OptOptions& options) {
	OptResult result;

	const bool toSSE = options.targetVersion.IsSSE() && hdr.GetVersion().IsSK();
	const bool toLE = options.targetVersion.IsSK() && hdr.GetVersion().IsSSE();

	if (!toSSE && !toLE) {
		result.versionMismatch = true;
		return result;
	}

	if (!isTerrain)
		result.dupesRenamed = RenameDuplicateShapes();

	hdr.SetVersion(options.targetVersion);

	auto shapes = GetShapes();
	if (toSSE) {
		for (auto* shape : shapes) {
			std::string shapeName = shape->name.get();

			auto geomData = hdr.GetBlock<NiGeometryData>(shape->DataRef());

			if (!geomData)
				continue;

			bool removeVertexColors = true;
			bool hasTangents = geomData->HasTangents();
			std::vector<Vector3>* vertices = &geomData->vertices;
			std::vector<Vector3>* normals = &geomData->normals;
			const std::vector<Color4>& colors = geomData->vertexColors;
			std::vector<Vector2>* uvs = nullptr;
			if (!geomData->uvSets.empty())
				uvs = &geomData->uvSets[0];

			std::vector<Triangle> triangles;
			geomData->GetTriangles(triangles);

			if (!options.removeParallax)
				removeVertexColors = false;

			// Only remove vertex colors if all are 0xFFFFFFFF
			if (removeVertexColors) {
				Color4 white(1.0f, 1.0f, 1.0f, 1.0f);
				for (auto& c : colors) {
					if (white != c) {
						removeVertexColors = false;
						break;
					}
				}
			}

			bool headPartEyes = false;
			NiShader* shader = GetShader(shape);
			if (shader) {
				auto bslsp = dynamic_cast<BSLightingShaderProperty*>(shader);
				if (bslsp) {
					// Remember eyes flag for later
					if ((bslsp->shaderFlags1 & (1 << 17)) != 0)
						headPartEyes = true;

					// No normals and tangents with model space maps
					if (bslsp->IsModelSpace()) {
						if (!normals->empty())
							result.shapesNormalsRemoved.push_back(shapeName);

						normals = nullptr;
					}

					// Check tree anim flag
					if ((bslsp->shaderFlags2 & (1 << 29)) != 0)
						removeVertexColors = false;

					// Disable flags if vertex colors were removed
					if (removeVertexColors) {
						bslsp->SetVertexColors(false);
						bslsp->SetVertexAlpha(false);
					}

					if (options.removeParallax) {
						if (bslsp->GetShaderType() == BSLSP_PARALLAX) {
							// Change type from parallax to default
							bslsp->SetShaderType(BSLSP_DEFAULT);

							// Remove parallax flag
							bslsp->shaderFlags1 &= ~(1 << 11);

							// Remove parallax texture from set
							auto textureSet = hdr.GetBlock(shader->TextureSetRef());
							if (textureSet && textureSet->textures.size() >= 4)
								textureSet->textures[3].clear();

							result.shapesParallaxRemoved.push_back(shapeName);
						}
					}
				}

				auto bsesp = dynamic_cast<BSEffectShaderProperty*>(shader);
				if (bsesp) {
					// Remember eyes flag for later
					if ((bsesp->shaderFlags1 & (1 << 17)) != 0)
						headPartEyes = true;

					// Check tree anim flag
					if ((bsesp->shaderFlags2 & (1 << 29)) != 0)
						removeVertexColors = false;

					// Disable flags if vertex colors were removed
					if (removeVertexColors) {
						bsesp->SetVertexColors(false);
						bsesp->SetVertexAlpha(false);
					}
				}
			}

			if (!colors.empty() && removeVertexColors)
				result.shapesVColorsRemoved.push_back(shapeName);

			std::unique_ptr<BSTriShape> bsOptShape = nullptr;

			auto bsSegmentShape = dynamic_cast<BSSegmentedTriShape*>(shape);
			if (bsSegmentShape) {
				bsOptShape = std::make_unique<BSSubIndexTriShape>();
			}
			else {
				if (options.headParts)
					bsOptShape = std::make_unique<BSDynamicTriShape>();
				else
					bsOptShape = std::make_unique<BSTriShape>();
			}

			bsOptShape->name.get() = shape->name.get();
			bsOptShape->controllerRef = shape->controllerRef;

			if (shape->HasSkinInstance())
				bsOptShape->SkinInstanceRef()->index = shape->SkinInstanceRef()->index;

			if (shape->HasShaderProperty())
				bsOptShape->ShaderPropertyRef()->index = shape->ShaderPropertyRef()->index;

			if (shape->HasAlphaProperty())
				bsOptShape->AlphaPropertyRef()->index = shape->AlphaPropertyRef()->index;

			bsOptShape->collisionRef = shape->collisionRef;
			bsOptShape->propertyRefs = shape->propertyRefs;
			bsOptShape->extraDataRefs = shape->extraDataRefs;

			bsOptShape->SetTransformToParent(shape->GetTransformToParent());

			bsOptShape->Create(hdr.GetVersion(), vertices, &triangles, uvs, normals);
			bsOptShape->flags = shape->flags;

			// Move segments to new shape
			if (bsSegmentShape) {
				auto bsSITS = static_cast<BSSubIndexTriShape*>(bsOptShape.get());
				bsSITS->SetSegments(bsSegmentShape->GetSegments());
			}

			// Restore old bounds for static meshes or when calc bounds is off
			if (!shape->IsSkinned() || !options.calcBounds)
				bsOptShape->SetBounds(geomData->GetBounds());

			// Vertex Colors
			if (bsOptShape->GetNumVertices() > 0) {
				if (!removeVertexColors && !colors.empty()) {
					bsOptShape->SetVertexColors(true);
					for (uint16_t i = 0; i < bsOptShape->GetNumVertices(); i++) {
						auto& vertex = bsOptShape->vertData[i];

						float f = std::max(0.0f, std::min(1.0f, colors[i].r));
						vertex.colorData[0] = static_cast<uint8_t>(std::floor(f == 1.0f ? 255 : f * 256.0));

						f = std::max(0.0f, std::min(1.0f, colors[i].g));
						vertex.colorData[1] = static_cast<uint8_t>(std::floor(f == 1.0f ? 255 : f * 256.0));

						f = std::max(0.0f, std::min(1.0f, colors[i].b));
						vertex.colorData[2] = static_cast<uint8_t>(std::floor(f == 1.0f ? 255 : f * 256.0));

						f = std::max(0.0f, std::min(1.0f, colors[i].a));
						vertex.colorData[3] = static_cast<uint8_t>(std::floor(f == 1.0f ? 255 : f * 256.0));
					}
				}

				// Find NiOptimizeKeep string
				for (auto& extraData : bsOptShape->extraDataRefs) {
					auto stringData = hdr.GetBlock<NiStringExtraData>(extraData);
					if (stringData) {
						if (stringData->stringData.get().find("NiOptimizeKeep") != std::string::npos) {
							bsOptShape->particleDataSize = bsOptShape->GetNumVertices() * 6
														   + static_cast<uint32_t>(triangles.size()) * 3;
							bsOptShape->particleVerts = *vertices;

							bsOptShape->particleNorms.resize(vertices->size(), Vector3(1.0f, 0.0f, 0.0f));
							if (normals && normals->size() == vertices->size())
								bsOptShape->particleNorms = *normals;

							bsOptShape->particleTris = triangles;
						}
					}
				}

				// Skinning and partitions
				if (shape->IsSkinned()) {
					bsOptShape->SetSkinned(true);

					auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
					if (skinInst) {
						auto skinPart = hdr.GetBlock(skinInst->skinPartitionRef);
						if (skinPart) {
							bool triangulated = skinPart->ConvertStripsToTriangles();
							if (triangulated)
								result.shapesPartTriangulated.push_back(shapeName);

							for (uint32_t partID = 0; partID < skinPart->numPartitions; partID++) {
								NiSkinPartition::PartitionBlock& part = skinPart->partitions[partID];

								for (uint32_t i = 0; i < part.numVertices; i++) {
									const uint16_t v = part.vertexMap[i];

									if (bsOptShape->vertData.size() > v) {
										auto& vertex = bsOptShape->vertData[v];

										if (part.hasVertexWeights) {
											auto& weights = part.vertexWeights[i];
											vertex.weights[0] = weights.w1;
											vertex.weights[1] = weights.w2;
											vertex.weights[2] = weights.w3;
											vertex.weights[3] = weights.w4;
										}

										if (part.hasBoneIndices) {
											auto& boneIndices = part.boneIndices[i];
											vertex.weightBones[0] = static_cast<uint8_t>(
												part.bones[boneIndices.i1]);
											vertex.weightBones[1] = static_cast<uint8_t>(
												part.bones[boneIndices.i2]);
											vertex.weightBones[2] = static_cast<uint8_t>(
												part.bones[boneIndices.i3]);
											vertex.weightBones[3] = static_cast<uint8_t>(
												part.bones[boneIndices.i4]);
										}
									}
								}

								part.GenerateTrueTrianglesFromMappedTriangles();
								part.triangles = part.trueTriangles;
							}
							skinPart->bMappedIndices = false;
						}
					}
				}
				else
					bsOptShape->SetSkinned(false);
			}
			else
				bsOptShape->SetVertices(false);

			// Check if tangents were added
			if (!hasTangents && bsOptShape->HasTangents())
				result.shapesTangentsAdded.push_back(shapeName);

			// Enable eye data flag
			if (!bsSegmentShape) {
				if (options.headParts) {
					if (headPartEyes)
						bsOptShape->SetEyeData(true);
				}
			}

			auto bsOptShapeObserver = bsOptShape.get();
			hdr.ReplaceBlock(GetBlockID(shape), std::move(bsOptShape));
			UpdateSkinPartitions(bsOptShapeObserver);
		}

		DeleteUnreferencedBlocks();

		// For files without a root node, remove the leftover data blocks anyway
		hdr.DeleteBlockByType("NiTriStripsData", true);
		hdr.DeleteBlockByType("NiTriShapeData", true);
	}
	else {
		for (auto* shape : shapes) {
			std::string shapeName = shape->name.get();

			auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
			if (!bsTriShape)
				continue;

			bool removeVertexColors = true;
			bool removeNormals = false;
			bool hasTangents = bsTriShape->HasTangents();
			const std::vector<Vector3>& vertices = bsTriShape->UpdateRawVertices();
			const std::vector<Vector3>& normals = bsTriShape->UpdateRawNormals();
			const std::vector<Color4>& colors = bsTriShape->UpdateRawColors();
			const std::vector<Vector2>& uvs = bsTriShape->UpdateRawUvs();

			std::vector<Triangle> triangles;
			bsTriShape->GetTriangles(triangles);

			if (!options.removeParallax)
				removeVertexColors = false;

			// Only remove vertex colors if all are 0xFFFFFFFF
			if (bsTriShape->HasVertexColors() && removeVertexColors) {
				Color4 white(1.0f, 1.0f, 1.0f, 1.0f);
				for (auto& c : colors) {
					if (white != c) {
						removeVertexColors = false;
						break;
					}
				}
			}

			NiShader* shader = GetShader(shape);
			if (shader) {
				auto bslsp = dynamic_cast<BSLightingShaderProperty*>(shader);
				if (bslsp) {
					// No normals and tangents with model space maps
					if (bslsp->IsModelSpace()) {
						if (!normals.empty())
							result.shapesNormalsRemoved.push_back(shapeName);

						removeNormals = true;
					}

					// Check tree anim flag
					if ((bslsp->shaderFlags2 & (1 << 29)) != 0)
						removeVertexColors = false;

					// Disable flags if vertex colors were removed
					if (removeVertexColors) {
						bslsp->SetVertexColors(false);
						bslsp->SetVertexAlpha(false);
					}

					// this flag breaks LE headparts
					if (options.headParts) {
						bslsp->shaderFlags2 &= ~SLSF2_PACKED_TANGENT;
					}

					if (options.removeParallax) {
						if (bslsp->GetShaderType() == BSLSP_PARALLAX) {
							// Change type from parallax to default
							bslsp->SetShaderType(BSLSP_DEFAULT);

							// Remove parallax flag
							bslsp->shaderFlags1 &= ~(1 << 11);

							// Remove parallax texture from set
							auto textureSet = hdr.GetBlock(shader->TextureSetRef());
							if (textureSet && textureSet->textures.size() >= 4)
								textureSet->textures[3].clear();

							result.shapesParallaxRemoved.push_back(shapeName);
						}
					}
				}

				auto bsesp = dynamic_cast<BSEffectShaderProperty*>(shader);
				if (bsesp) {
					// Check tree anim flag
					if ((bsesp->shaderFlags2 & (1 << 29)) != 0)
						removeVertexColors = false;

					// Disable flags if vertex colors were removed
					if (removeVertexColors) {
						bsesp->SetVertexColors(false);
						bsesp->SetVertexAlpha(false);
					}
				}
			}

			if (!colors.empty() && removeVertexColors)
				result.shapesVColorsRemoved.push_back(shapeName);

			std::unique_ptr<NiTriShape> bsOptShape = nullptr;
			auto [bsOptShapeDataS, bsOptShapeData] = make_unique<NiTriShapeData>();
			auto bsSITS = dynamic_cast<BSSubIndexTriShape*>(shape);
			if (bsSITS)
				bsOptShape = std::make_unique<BSSegmentedTriShape>();
			else
				bsOptShape = std::make_unique<NiTriShape>();

			int dataId = hdr.AddBlock(std::move(bsOptShapeDataS));
			bsOptShape->DataRef()->index = dataId;
			bsOptShape->SetGeomData(bsOptShapeData);
			bsOptShapeData->Create(hdr.GetVersion(),
								   &vertices,
								   &triangles,
								   &uvs,
								   !removeNormals ? &normals : nullptr);

			bsOptShape->name.get() = shape->name.get();

			if (shape->HasSkinInstance())
				bsOptShape->SkinInstanceRef()->index = shape->SkinInstanceRef()->index;

			if (shape->HasShaderProperty())
				bsOptShape->ShaderPropertyRef()->index = shape->ShaderPropertyRef()->index;

			if (shape->HasAlphaProperty())
				bsOptShape->AlphaPropertyRef()->index = shape->AlphaPropertyRef()->index;

			bsOptShape->controllerRef = shape->controllerRef;
			bsOptShape->collisionRef = shape->collisionRef;
			bsOptShape->propertyRefs = shape->propertyRefs;
			bsOptShape->extraDataRefs = shape->extraDataRefs;

			bsOptShape->SetTransformToParent(shape->GetTransformToParent());
			bsOptShape->flags = shape->flags;

			// Move segments to new shape
			if (bsSITS) {
				auto bsSegmentShape = static_cast<BSSegmentedTriShape*>(bsOptShape.get());
				bsSegmentShape->SetSegments(bsSITS->GetSegments());
			}

			// Restore old bounds for static meshes or when calc bounds is off
			if (!shape->IsSkinned() || !options.calcBounds)
				bsOptShape->SetBounds(bsTriShape->GetBounds());

			// Vertex Colors
			if (bsOptShape->GetNumVertices() > 0) {
				if (!removeVertexColors && !colors.empty()) {
					bsOptShape->SetVertexColors(true);
					for (uint16_t i = 0; i < bsOptShape->GetNumVertices(); i++)
						bsOptShapeData->vertexColors[i] = colors[i];
				}

				// Skinning and partitions
				if (shape->IsSkinned()) {
					auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
					if (skinInst) {
						auto skinPart = hdr.GetBlock(skinInst->skinPartitionRef);
						if (skinPart) {
							bool triangulated = skinPart->ConvertStripsToTriangles();
							if (triangulated)
								result.shapesPartTriangulated.push_back(shapeName);

							for (uint32_t partID = 0; partID < skinPart->numPartitions; partID++) {
								NiSkinPartition::PartitionBlock& part = skinPart->partitions[partID];

								part.GenerateMappedTrianglesFromTrueTrianglesAndVertexMap();
							}
							skinPart->bMappedIndices = true;
						}
					}
				}
			}
			else
				bsOptShape->SetVertices(false);

			// Check if tangents were added
			if (!hasTangents && bsOptShape->HasTangents())
				result.shapesTangentsAdded.push_back(shapeName);

			auto bsOptShapeObserver = bsOptShape.get();
			hdr.ReplaceBlock(GetBlockID(shape), std::move(bsOptShape));
			UpdateSkinPartitions(bsOptShapeObserver);
		}

		DeleteUnreferencedBlocks();
		PrettySortBlocks();
	}

	if (options.fixBSXFlags)
		FixBSXFlags();

	if (options.fixShaderFlags)
		FixShaderFlags();

	return result;
}

void NifFile::PrepareData() {
	hdr.FillStringRefs();
	LinkGeomData();
	TrimTexturePaths();

	for (auto& shape : GetShapes()) {
		// Move triangle and vertex data from partition to shape
		if (hdr.GetVersion().IsSSE()) {
			auto* bsTriShape = dynamic_cast<BSTriShape*>(shape);
			if (!bsTriShape)
				continue;

			auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
			if (!skinInst)
				continue;

			auto skinPart = hdr.GetBlock(skinInst->skinPartitionRef);
			if (!skinPart)
				continue;

			bsTriShape->SetVertexData(skinPart->vertData);

			std::vector<Triangle> tris;
			for (int pi = 0; pi < static_cast<int>(skinPart->partitions.size()); ++pi)
				for (auto& tri : skinPart->partitions[pi].trueTriangles) {
					tris.push_back(tri);
					skinPart->triParts.push_back(pi);
				}

			bsTriShape->SetTriangles(tris);

			auto dynamicShape = dynamic_cast<BSDynamicTriShape*>(bsTriShape);
			if (dynamicShape) {
				for (uint16_t i = 0; i < dynamicShape->GetNumVertices(); i++) {
					dynamicShape->vertData[i].vert.x = dynamicShape->dynamicData[i].x;
					dynamicShape->vertData[i].vert.y = dynamicShape->dynamicData[i].y;
					dynamicShape->vertData[i].vert.z = dynamicShape->dynamicData[i].z;
					dynamicShape->vertData[i].bitangentX = dynamicShape->dynamicData[i].w;
				}
			}
		}

		// Move tangents and bitangents from binary extra data to shape
		if (hdr.GetVersion().IsOB()) {
			std::vector<Vector3> tangents;
			std::vector<Vector3> bitangents;
			if (GetBinaryTangentData(shape, &tangents, &bitangents)) {
				SetTangentsForShape(shape, tangents);
				SetBitangentsForShape(shape, bitangents);
			}
		}
	}

	RemoveInvalidTris();
}

void NifFile::FinalizeData() {
	for (auto& shape : GetShapes()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape) {
			auto bsDynTriShape = dynamic_cast<BSDynamicTriShape*>(shape);
			if (bsDynTriShape)
				bsDynTriShape->CalcDynamicData();

			bsTriShape->CalcDataSizes(hdr.GetVersion());

			if (hdr.GetVersion().IsSSE()) {
				// Move triangle and vertex data from shape to partition
				auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
				if (skinInst) {
					auto skinPart = hdr.GetBlock(skinInst->skinPartitionRef);
					if (skinPart) {
						skinPart->numVertices = bsTriShape->GetNumVertices();
						skinPart->dataSize = bsTriShape->dataSize;
						skinPart->vertexSize = bsTriShape->vertexSize;
						skinPart->vertData = bsTriShape->vertData;
						skinPart->vertexDesc = bsTriShape->vertexDesc;

						for (uint32_t partInd = 0; partInd < skinPart->numPartitions; ++partInd) {
							NiSkinPartition::PartitionBlock& part = skinPart->partitions[partInd];

							// Copy relevant data from shape to each partition
							part.vertexDesc = bsTriShape->vertexDesc;
						}
					}
				}
			}
		}

		if (hdr.GetVersion().IsOB()) {
			// Move tangents and bitangents from shape back to binary extra data
			if (shape->HasTangents()) {
				auto tangents = GetTangentsForShape(shape);
				auto bitangents = GetBitangentsForShape(shape);
				SetBinaryTangentData(shape, tangents, bitangents);
			}
			else
				DeleteBinaryTangentData(shape);
		}
	}
}

bool NifFile::IsSSECompatible() const {
	auto shapes = GetShapes();
	return std::all_of(shapes.cbegin(), shapes.cend(), [this](auto&& shape) {
		return IsSSECompatible(shape);
	});
}

bool NifFile::IsSSECompatible(NiShape* shape) const {
	// Check if shape has strips in the geometry or skin partition
	if (shape->HasType<NiTriStrips>())
		return false;

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (skinInst) {
		auto skinPart = hdr.GetBlock(skinInst->skinPartitionRef);
		if (skinPart) {
			for (auto& partition : skinPart->partitions) {
				if (partition.numStrips > 0)
					return false;
			}
		}
	}

	return true;
}

NiShape* NifFile::CreateShapeFromData(const std::string& shapeName,
									  const std::vector<Vector3>* v,
									  const std::vector<Triangle>* t,
									  const std::vector<Vector2>* uv,
									  const std::vector<Vector3>* norms) {
	auto rootNode = GetRootNode();
	if (!rootNode)
		return nullptr;

	const NiVersion& version = hdr.GetVersion();

	NiShape* shapeResult = nullptr;
	if (version.IsSSE()) {
		auto triShape = std::make_unique<BSTriShape>();
		triShape->Create(hdr.GetVersion(), v, t, uv, norms);
		triShape->SetSkinned(false);

		auto nifTexset = std::make_unique<BSShaderTextureSet>(hdr.GetVersion());

		auto nifShader = std::make_unique<BSLightingShaderProperty>(hdr.GetVersion());
		nifShader->TextureSetRef()->index = hdr.AddBlock(std::move(nifTexset));
		nifShader->SetSkinned(false);

		triShape->name.get() = shapeName;

		int shaderID = hdr.AddBlock(std::move(nifShader));
		triShape->ShaderPropertyRef()->index = shaderID;

		shapeResult = triShape.get();

		int shapeID = hdr.AddBlock(std::move(triShape));
		rootNode->childRefs.AddBlockRef(shapeID);
	}
	else if (version.IsFO4() || version.IsFO76()) {
		auto nifBSTriShape = std::make_unique<BSSubIndexTriShape>();
		nifBSTriShape->Create(hdr.GetVersion(), v, t, uv, norms);
		nifBSTriShape->SetSkinned(false);

		auto nifTexset = std::make_unique<BSShaderTextureSet>(hdr.GetVersion());

		auto nifShader = std::make_unique<BSLightingShaderProperty>(hdr.GetVersion());
		nifShader->TextureSetRef()->index = hdr.AddBlock(std::move(nifTexset));

		std::string wetShaderName = "template/OutfitTemplate_Wet.bgsm";
		nifShader->SetWetMaterialName(wetShaderName);
		nifShader->SetSkinned(false);

		nifBSTriShape->name.get() = shapeName;

		int shaderID = hdr.AddBlock(std::move(nifShader));
		nifBSTriShape->ShaderPropertyRef()->index = shaderID;

		shapeResult = nifBSTriShape.get();

		int shapeID = hdr.AddBlock(std::move(nifBSTriShape));
		rootNode->childRefs.AddBlockRef(shapeID);
	}
	else {
		auto nifTexset = std::make_unique<BSShaderTextureSet>(hdr.GetVersion());

		int shaderID{};
		std::unique_ptr<BSLightingShaderProperty> nifShader = nullptr;
		std::unique_ptr<BSShaderPPLightingProperty> nifShaderPP = nullptr;

		if (version.IsSK()) {
			nifShader = std::make_unique<BSLightingShaderProperty>(hdr.GetVersion());
			nifShader->TextureSetRef()->index = hdr.AddBlock(std::move(nifTexset));
			nifShader->SetSkinned(false);
			shaderID = hdr.AddBlock(std::move(nifShader));
		}
		else {
			nifShaderPP = std::make_unique<BSShaderPPLightingProperty>();
			nifShaderPP->TextureSetRef()->index = hdr.AddBlock(std::move(nifTexset));
			nifShaderPP->SetSkinned(false);
			shaderID = hdr.AddBlock(std::move(nifShaderPP));
		}

		auto nifTriShape = std::make_unique<NiTriShape>();
		if (version.IsSK())
			nifTriShape->ShaderPropertyRef()->index = shaderID;
		else
			nifTriShape->propertyRefs.AddBlockRef(shaderID);

		nifTriShape->name.get() = shapeName;

		auto nifShapeData = std::make_unique<NiTriShapeData>();
		nifShapeData->Create(hdr.GetVersion(), v, t, uv, norms);
		nifTriShape->SetGeomData(nifShapeData.get());

		int dataID = hdr.AddBlock(std::move(nifShapeData));
		nifTriShape->DataRef()->index = dataID;
		nifTriShape->SetSkinned(false);

		shapeResult = nifTriShape.get();

		int shapeID = hdr.AddBlock(std::move(nifTriShape));
		rootNode->childRefs.AddBlockRef(shapeID);
	}

	return shapeResult;
}

std::vector<std::string> NifFile::GetShapeNames() const {
	std::vector<std::string> outList;
	for (auto& block : blocks) {
		auto shape = dynamic_cast<NiShape*>(block.get());
		if (shape)
			outList.push_back(shape->name.get());
	}
	return outList;
}

std::vector<NiShape*> NifFile::GetShapes() const {
	std::vector<NiShape*> outList;
	for (auto& block : blocks) {
		auto shape = dynamic_cast<NiShape*>(block.get());
		if (shape)
			outList.push_back(shape);
	}
	return outList;
}

bool NifFile::RenameShape(NiShape* shape, const std::string& newName) {
	if (shape) {
		shape->name.get() = newName;
		return true;
	}

	return false;
}

bool NifFile::RenameDuplicateShapes() {
	auto countDupes = [this](NiNode* parent, const std::string& name) {
		if (name.empty())
			return ptrdiff_t(0);

		std::vector<std::string> names;
		std::set<int> uniqueRefs;
		for (auto& child : parent->childRefs) {
			auto obj = hdr.GetBlock<NiAVObject>(child);
			if (obj) {
				if (uniqueRefs.find(child.index) == uniqueRefs.end()) {
					names.push_back(obj->name.get());
					uniqueRefs.insert(child.index);
				}
			}
		}

		return std::count(names.begin(), names.end(), name);
	};

	bool renamed = false;
	auto nodes = GetChildren<NiNode>();

	auto root = GetRootNode();
	if (root)
		nodes.push_back(root);

	for (auto& node : nodes) {
		int dupCount = 0;

		for (auto& child : node->childRefs) {
			auto shape = hdr.GetBlock<NiShape>(child);
			if (shape) {
				// Skip first child
				if (dupCount == 0) {
					dupCount++;
					continue;
				}

				std::string shapeName = shape->name.get();

				bool duped = countDupes(node, shapeName) > 1;
				if (duped) {
					std::string dup = "_" + std::to_string(dupCount);

					while (countDupes(node, shapeName + dup) > 1) {
						dupCount++;
						dup = "_" + std::to_string(dupCount);
					}

					shape->name.get() = shapeName + dup;
					dupCount++;
					renamed = true;
				}
			}
		}
	}

	return renamed;
}

void NifFile::TriangulateShape(NiShape* shape) {
	if (shape->HasType<NiTriStrips>()) {
		auto stripsData = hdr.GetBlock<NiTriStripsData>(shape->DataRef());
		if (stripsData) {
			std::vector<Triangle> tris = stripsData->StripsToTris();

			if (!tris.empty()) {
				auto [triShapeS, triShape] = make_unique<NiTriShape>();
				*static_cast<NiTriBasedGeom*>(triShape) = *static_cast<NiTriBasedGeom*>(shape);
				hdr.ReplaceBlock(GetBlockID(shape), std::move(triShapeS));

				auto [triShapeDataS, triShapeData] = make_unique<NiTriShapeData>();
				*static_cast<NiTriBasedGeomData*>(triShapeData) = *static_cast<NiTriBasedGeomData*>(
					stripsData);
				triShapeData->SetTriangles(tris);
				hdr.ReplaceBlock(GetBlockID(stripsData), std::move(triShapeDataS));
				triShape->SetGeomData(triShapeData);
			}
		}
	}
}

NiNode* NifFile::GetRootNode() const {
	// Check if block at index 0 is a node
	auto root = hdr.GetBlock<NiNode>(0u);
	if (!root) {
		// Not a node, look for first node block
		for (auto& block : blocks) {
			auto node = dynamic_cast<NiNode*>(block.get());
			if (node) {
				root = node;
				break;
			}
		}
	}
	return root;
}

void NifFile::GetTree(std::vector<NiObject*>& result, NiObject* parent) const {
	if (parent == nullptr) {
		parent = GetRootNode();
		if (parent == nullptr)
			return;
	}

	result.push_back(parent);

	std::vector<uint32_t> indices;
	parent->GetChildIndices(indices);

	for (auto& i : indices) {
		auto child = hdr.GetBlock<NiObject>(i);
		if (child && !contains(result, child))
			GetTree(result, child);
	}
}

bool NifFile::GetNodeTransformToParent(const std::string& nodeName, MatTransform& outTransform) const {
	for (auto& block : blocks) {
		auto node = dynamic_cast<NiNode*>(block.get());
		if (node && node->name == nodeName) {
			outTransform = node->GetTransformToParent();
			return true;
		}
	}
	return false;
}

bool NifFile::GetNodeTransformToGlobal(const std::string& nodeName, MatTransform& outTransform) const {
	for (auto& block : blocks) {
		auto* node = dynamic_cast<NiNode*>(block.get());
		if (!node || node->name != nodeName)
			continue;

		MatTransform xform = node->GetTransformToParent();
		NiNode* parent = GetParentNode(node);
		while (parent) {
			xform = parent->GetTransformToParent().ComposeTransforms(xform);
			parent = GetParentNode(parent);
		}
		outTransform = xform;
		return true;
	}

	return false;
}

bool NifFile::SetNodeTransformToParent(const std::string& nodeName,
									   const MatTransform& inTransform,
									   const bool rootChildrenOnly) {
	if (rootChildrenOnly) {
		auto root = GetRootNode();
		if (root) {
			for (auto& child : root->childRefs) {
				auto node = hdr.GetBlock<NiNode>(child);
				if (node) {
					if (node->name == nodeName) {
						node->SetTransformToParent(inTransform);
						return true;
					}
				}
			}
		}
	}
	else {
		for (auto& block : blocks) {
			auto node = dynamic_cast<NiNode*>(block.get());
			if (node && node->name == nodeName) {
				node->SetTransformToParent(inTransform);
				return true;
			}
		}
	}

	return false;
}

uint32_t NifFile::GetShapeBoneList(NiShape* shape, std::vector<std::string>& outList) const {
	outList.clear();

	if (!shape)
		return 0;

	auto skinInst = hdr.GetBlock<NiBoneContainer>(shape->SkinInstanceRef());
	if (!skinInst)
		return 0;

	for (auto& bone : skinInst->boneRefs) {
		auto node = hdr.GetBlock(bone);
		if (node)
			outList.push_back(node->name.get());
	}

	return static_cast<uint32_t>(outList.size());
}

uint32_t NifFile::GetShapeBoneIDList(NiShape* shape, std::vector<int>& outList) const {
	outList.clear();

	if (!shape)
		return 0;

	auto skinInst = hdr.GetBlock<NiBoneContainer>(shape->SkinInstanceRef());
	if (!skinInst)
		return 0;

	for (auto& bone : skinInst->boneRefs)
		if (!bone.IsEmpty())
			outList.push_back(bone.index);

	return static_cast<uint32_t>(outList.size());
}

void NifFile::SetShapeBoneIDList(NiShape* shape, std::vector<int>& inList) {
	if (!shape)
		return;

	BSSkinBoneData* boneData = nullptr;
	if (shape->HasType<BSTriShape>()) {
		auto skinForBoneRef = hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef());
		if (skinForBoneRef)
			boneData = hdr.GetBlock(skinForBoneRef->dataRef);
	}

	auto boneCont = hdr.GetBlock<NiBoneContainer>(shape->SkinInstanceRef());
	if (!boneCont)
		return;

	boneCont->boneRefs.Clear();

	bool feedBoneData = false;
	if (boneData && boneData->nBones != inList.size()) {
		// Clear if size doesn't match
		boneData->nBones = 0;
		boneData->boneXforms.clear();
		feedBoneData = true;
	}

	for (auto& i : inList) {
		boneCont->boneRefs.AddBlockRef(i);
		if (boneData && feedBoneData) {
			boneData->boneXforms.emplace_back();
			boneData->nBones++;
		}
	}

	auto skinInst = dynamic_cast<NiSkinInstance*>(boneCont);
	if (skinInst) {
		auto skinData = hdr.GetBlock(skinInst->dataRef);
		if (skinData) {
			feedBoneData = false;

			if (skinData->numBones != inList.size()) {
				// Clear if size doesn't match
				skinData->numBones = 0;
				skinData->bones.clear();
				feedBoneData = true;
			}

			if (feedBoneData) {
				skinData->bones.resize(inList.size());
				skinData->numBones = static_cast<uint32_t>(skinData->bones.size());
			}
		}
	}
}

uint32_t NifFile::GetShapeBoneWeights(NiShape* shape,
									  const uint32_t boneIndex,
									  std::unordered_map<uint16_t, float>& outWeights) const {
	outWeights.clear();

	if (!shape)
		return 0;

	auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
	if (bsTriShape) {
		outWeights.reserve(bsTriShape->GetNumVertices());
		for (uint16_t vid = 0; vid < bsTriShape->GetNumVertices(); vid++) {
			auto& vertex = bsTriShape->vertData[vid];
			for (size_t i = 0; i < 4; i++) {
				if (vertex.weightBones[i] == boneIndex && vertex.weights[i] != 0.0f)
					outWeights.emplace(vid, vertex.weights[i]);
			}
		}

		return static_cast<uint32_t>(outWeights.size());
	}

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return 0;

	auto skinData = hdr.GetBlock(skinInst->dataRef);
	if (!skinData || boneIndex >= skinData->numBones)
		return 0;

	NiSkinData::BoneData* bone = &skinData->bones[boneIndex];
	for (auto& sw : bone->vertexWeights)
		if (sw.weight >= EPSILON)
			outWeights.emplace(sw.index, sw.weight);

	return static_cast<uint32_t>(outWeights.size());
}

bool NifFile::CalcShapeTransformGlobalToSkin(NiShape* shape, MatTransform& outTransform) const {
	if (!shape)
		return false;
	if (GetShapeTransformGlobalToSkin(shape, outTransform))
		return true;

	// Now the nif doesn't have this transform, probably because it's
	// a FO4 nif, so we will try to calculate it, since FO4 shapes almost
	// always have a non-identity global-to-skin transform.
	// Ideally, we'd use bone transforms from the skeleton file, but we
	// don't have access to that here.
	std::vector<std::string> bones;
	GetShapeBoneList(shape, bones);
	for (const std::string& bone : bones) {
		MatTransform xformBoneToGlobal;
		if (!GetNodeTransformToGlobal(bone, xformBoneToGlobal))
			continue;
		MatTransform xformSkinToBone;
		if (!GetShapeTransformSkinToBone(shape, bone, xformSkinToBone))
			continue;
		// compose: skin -> bone -> global and invert
		outTransform = xformBoneToGlobal.ComposeTransforms(xformSkinToBone).InverseTransform();
		return true;
	}
	return false;
}

bool NifFile::GetShapeTransformGlobalToSkin(NiShape* shape, MatTransform& outTransform) const {
	if (!shape)
		return false;

	// For FO4 meshes, the skin instance is a BSSkinInstance instead of
	// an NiSkinInstance, so skinInst will be nullptr.  FO4 meshes do not
	// have this transform.
	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return false;

	auto skinData = hdr.GetBlock(skinInst->dataRef);
	if (!skinData)
		return false;

	outTransform = skinData->skinTransform;
	return true;
}

void NifFile::SetShapeTransformGlobalToSkin(NiShape* shape, const MatTransform& inTransform) {
	if (!shape)
		return;

	// For FO4 meshes, the skin instance is a BSSkinInstance instead of
	// an NiSkinInstance, so skinInst will be nullptr.  FO4 meshes do not
	// have this transform.
	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return;

	auto skinData = hdr.GetBlock(skinInst->dataRef);
	if (!skinData)
		return;

	// Set the overall skin transform
	skinData->skinTransform = inTransform;
}

bool NifFile::GetShapeTransformSkinToBone(NiShape* shape,
										  const std::string& boneName,
										  MatTransform& outTransform) const {
	if (!shape)
		return false;

	return GetShapeTransformSkinToBone(shape, shape->GetBoneID(hdr, boneName), outTransform);
}

bool NifFile::GetShapeTransformSkinToBone(NiShape* shape,
										  const uint32_t boneIndex,
										  MatTransform& outTransform) const {
	if (!shape)
		return false;

	auto skinForBoneRef = hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef());
	if (skinForBoneRef) {
		auto boneData = hdr.GetBlock(skinForBoneRef->dataRef);
		if (boneData) {
			if (boneIndex >= boneData->nBones)
				return false;

			outTransform = boneData->boneXforms[boneIndex].boneTransform;
			return true;
		}
	}

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return false;

	auto skinData = hdr.GetBlock(skinInst->dataRef);
	if (!skinData)
		return false;

	if (boneIndex >= skinData->numBones)
		return false;

	NiSkinData::BoneData* bone = &skinData->bones[boneIndex];
	outTransform = bone->boneTransform;
	return true;
}

void NifFile::SetShapeTransformSkinToBone(NiShape* shape,
										  const uint32_t boneIndex,
										  const MatTransform& inTransform) {
	if (!shape)
		return;

	auto skinForBoneRef = hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef());
	if (skinForBoneRef) {
		auto bsSkin = hdr.GetBlock(skinForBoneRef->dataRef);
		if (!bsSkin)
			return;

		if (boneIndex >= bsSkin->nBones)
			return;
		bsSkin->boneXforms[boneIndex].boneTransform = inTransform;
		return;
	}

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return;

	auto skinData = hdr.GetBlock(skinInst->dataRef);
	if (!skinData)
		return;

	if (boneIndex >= skinData->numBones)
		return;

	NiSkinData::BoneData* bone = &skinData->bones[boneIndex];
	bone->boneTransform = inTransform;
}

bool NifFile::GetShapeBoneTransform(NiShape* shape,
									const std::string& boneName,
									MatTransform& outTransform) const {
	if (boneName.empty())
		return GetShapeTransformGlobalToSkin(shape, outTransform);
	return GetShapeTransformSkinToBone(shape, boneName, outTransform);
}

bool NifFile::GetShapeBoneTransform(NiShape* shape,
									const uint32_t boneIndex,
									MatTransform& outTransform) const {
	if (boneIndex == 0xFFFFFFFF)
		return GetShapeTransformGlobalToSkin(shape, outTransform);

	return GetShapeTransformSkinToBone(shape, boneIndex, outTransform);
}

bool NifFile::SetShapeBoneTransform(NiShape* shape, const uint32_t boneIndex, MatTransform& inTransform) {
	if (boneIndex == 0xFFFFFFFF)
		SetShapeTransformGlobalToSkin(shape, inTransform);
	else
		SetShapeTransformSkinToBone(shape, boneIndex, inTransform);
	return true;
}

bool NifFile::SetShapeBoneBounds(const std::string& shapeName,
								 const uint32_t boneIndex,
								 BoundingSphere& inBounds) {
	auto shape = FindBlockByName<NiShape>(shapeName);
	if (!shape)
		return false;

	auto skinForBoneRef = hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef());
	if (skinForBoneRef && boneIndex != 0xFFFFFFFF) {
		auto bsSkin = hdr.GetBlock(skinForBoneRef->dataRef);
		if (!bsSkin)
			return false;

		bsSkin->boneXforms[boneIndex].bounds = inBounds;
		return true;
	}

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return false;

	auto skinData = hdr.GetBlock(skinInst->dataRef);
	if (!skinData)
		return false;

	if (boneIndex >= skinData->numBones)
		return false;

	NiSkinData::BoneData* bone = &skinData->bones[boneIndex];
	bone->bounds = inBounds;
	return true;
}

bool NifFile::GetShapeBoneBounds(NiShape* shape, const uint32_t boneIndex, BoundingSphere& outBounds) const {
	if (!shape)
		return false;

	auto skinForBoneRef = hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef());
	if (skinForBoneRef) {
		auto boneData = hdr.GetBlock(skinForBoneRef->dataRef);
		if (boneData) {
			outBounds = boneData->boneXforms[boneIndex].bounds;
			return true;
		}
	}

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return false;

	auto skinData = hdr.GetBlock(skinInst->dataRef);
	if (!skinData)
		return false;

	if (boneIndex >= skinData->numBones)
		return false;

	NiSkinData::BoneData* bone = &skinData->bones[boneIndex];
	outBounds = bone->bounds;
	return true;
}

void NifFile::UpdateShapeBoneID(const std::string& shapeName, const uint32_t oldID, const uint32_t newID) {
	auto shape = FindBlockByName<NiShape>(shapeName);
	if (!shape)
		return;

	auto boneCont = hdr.GetBlock<NiBoneContainer>(shape->SkinInstanceRef());
	if (!boneCont)
		return;

	for (auto& bp : boneCont->boneRefs) {
		if (!bp.IsEmpty() && bp.index == oldID) {
			bp.index = newID;
			return;
		}
	}
}

void NifFile::SetShapeBoneWeights(const std::string& shapeName,
								  const uint32_t boneIndex,
								  std::unordered_map<uint16_t, float>& inWeights) {
	auto shape = FindBlockByName<NiShape>(shapeName);
	if (!shape)
		return;

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return;

	auto skinData = hdr.GetBlock(skinInst->dataRef);
	if (!skinData)
		return;

	if (boneIndex >= skinData->numBones)
		return;

	skinData->hasVertWeights = true;

	NiSkinData::BoneData* bone = &skinData->bones[boneIndex];
	bone->vertexWeights.clear();
	for (auto& sw : inWeights)
		if (sw.second >= 0.0001f)
			bone->vertexWeights.emplace_back(SkinWeight(sw.first, sw.second));

	bone->numVertices = static_cast<uint16_t>(bone->vertexWeights.size());
}

void NifFile::SetShapeVertWeights(const std::string& shapeName,
								  const uint16_t vertIndex,
								  std::vector<uint8_t>& boneids,
								  std::vector<float>& weights) const {
	auto shape = FindBlockByName<NiShape>(shapeName);
	if (!shape)
		return;

	auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
	if (!bsTriShape)
		return;

	if (vertIndex < 0 || vertIndex >= bsTriShape->vertData.size())
		return;

	auto& vertex = bsTriShape->vertData[vertIndex];
	std::memset(vertex.weights, 0, sizeof(float) * 4);
	std::memset(vertex.weightBones, 0, sizeof(uint8_t) * 4);

	// Sum weights to normalize values
	float sum = 0.0f;
	for (auto weight : weights)
		sum += weight;

	uint32_t num = (weights.size() < 4 ? static_cast<uint32_t>(weights.size()) : 4);

	for (uint32_t i = 0; i < num; i++) {
		vertex.weightBones[i] = boneids[i];
		vertex.weights[i] = weights[i] / sum;
	}
}

void NifFile::ClearShapeVertWeights(const std::string& shapeName) const {
	auto shape = FindBlockByName<NiShape>(shapeName);
	if (!shape)
		return;

	auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
	if (!bsTriShape)
		return;

	for (auto& vertex : bsTriShape->vertData) {
		std::memset(vertex.weights, 0, sizeof(float) * 4);
		std::memset(vertex.weightBones, 0, sizeof(uint8_t) * 4);
	}
}

bool NifFile::GetShapeSegments(NiShape* shape, NifSegmentationInfo& inf, std::vector<int>& triParts) {
	auto bssits = dynamic_cast<BSSubIndexTriShape*>(shape);
	if (!bssits)
		return false;

	bssits->GetSegmentation(inf, triParts);
	return true;
}

void NifFile::SetShapeSegments(NiShape* shape,
							   const NifSegmentationInfo& inf,
							   const std::vector<int>& triParts) {
	auto bssits = dynamic_cast<BSSubIndexTriShape*>(shape);
	if (!bssits)
		return;

	bssits->SetSegmentation(inf, triParts);
}

bool NifFile::GetShapePartitions(NiShape* shape,
								 NiVector<BSDismemberSkinInstance::PartitionInfo>& partitionInfo,
								 std::vector<int>& triParts) const {
	if (!shape)
		return false;

	auto bsdSkinInst = hdr.GetBlock<BSDismemberSkinInstance>(shape->SkinInstanceRef());
	if (bsdSkinInst)
		partitionInfo = bsdSkinInst->partitions;
	else
		partitionInfo.clear();

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return false;

	auto skinPart = hdr.GetBlock(skinInst->skinPartitionRef);
	if (!skinPart)
		return false;

	// Generate triParts
	std::vector<Triangle> shapeTris;
	shape->GetTriangles(shapeTris);
	skinPart->PrepareTriParts(shapeTris);
	triParts = skinPart->triParts;

	// Make sure every partition has a PartitionInfo
	while (partitionInfo.size() < skinPart->partitions.size()) {
		BSDismemberSkinInstance::PartitionInfo pi;
		pi.flags = PF_EDITOR_VISIBLE;
		pi.partID = hdr.GetVersion().User() >= 12 ? 32 : 0;
		partitionInfo.push_back(pi);
	}

	return true;
}

void NifFile::SetShapePartitions(NiShape* shape,
								 const NiVector<BSDismemberSkinInstance::PartitionInfo>& partitionInfo,
								 const std::vector<int>& triParts,
								 const bool convertSkinInstance) {
	if (!shape)
		return;

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return;

	auto skinPart = hdr.GetBlock(skinInst->skinPartitionRef);
	if (!skinPart)
		return;

	// Calculate new number of partitions.  This code assumes we might have
	// misassigned or unassigned triangles, though it's unclear whether
	// it's even possible to have misassigned or unassigned triangles.
	uint32_t numParts = static_cast<uint32_t>(partitionInfo.size());
	bool hasUnassignedTris = false;
	for (auto pi : triParts) {
		if (pi >= static_cast<int>(numParts))
			numParts = pi + 1;
		if (pi < 0)
			hasUnassignedTris = true;
	}
	if (hasUnassignedTris)
		++numParts;

	// Copy triParts and assign unassigned triangles to a partition
	skinPart->triParts = triParts;
	if (hasUnassignedTris) {
		for (int& pi : skinPart->triParts) {
			if (pi < 0)
				pi = static_cast<int>(numParts) - 1;
		}
	}

	// Resize NiSkinPartition partition list
	skinPart->numPartitions = numParts;
	skinPart->partitions.resize(numParts);
	for (uint32_t i = 0; i < numParts; i++)
		skinPart->partitions[i].hasVertexMap = true;

	// Regenerate trueTriangles
	std::vector<Triangle> shapeTris;
	shape->GetTriangles(shapeTris);
	skinPart->GenerateTrueTrianglesFromTriParts(shapeTris);

	// Set BSDismemberSkinInstance partition list
	auto bsdSkinInst = hdr.GetBlock<BSDismemberSkinInstance>(shape->SkinInstanceRef());
	if (!bsdSkinInst && convertSkinInstance && hdr.GetVersion().File() == NiFileVersion::V20_2_0_7) {
		auto newBsdSkinInst = std::make_unique<BSDismemberSkinInstance>();
		bsdSkinInst = newBsdSkinInst.get();

		*static_cast<NiSkinInstance*>(bsdSkinInst) = *static_cast<NiSkinInstance*>(skinInst);
		hdr.ReplaceBlock(GetBlockID(skinInst), std::move(newBsdSkinInst));
	}

	if (bsdSkinInst) {
		bsdSkinInst->partitions = partitionInfo;
		while (bsdSkinInst->partitions.size() < numParts) {
			BSDismemberSkinInstance::PartitionInfo pi;
			pi.flags = PF_EDITOR_VISIBLE;
			pi.partID = hdr.GetVersion().User() >= 12 ? 32 : 0;
			bsdSkinInst->partitions.push_back(pi);
		}
	}
}

void NifFile::SetDefaultPartition(NiShape* shape) {
	std::vector<Triangle> tris;
	shape->GetTriangles(tris);

	uint16_t numVertices = shape->GetNumVertices();
	bool bMappedIndices = !shape->HasType<BSTriShape>();

	auto bsdSkinInst = hdr.GetBlock<BSDismemberSkinInstance>(shape->SkinInstanceRef());
	if (bsdSkinInst) {
		BSDismemberSkinInstance::PartitionInfo partInfo;
		partInfo.flags = PF_EDITOR_VISIBLE;
		partInfo.partID = hdr.GetVersion().User() >= 12 ? 32 : 0;

		bsdSkinInst->partitions.clear();
		bsdSkinInst->partitions.push_back(partInfo);
	}

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return;

	auto skinPart = hdr.GetBlock(skinInst->skinPartitionRef);
	if (skinPart) {
		NiSkinPartition::PartitionBlock part;
		if (numVertices > 0) {
			part.hasVertexMap = true;
			part.numVertices = numVertices;

			std::vector<uint16_t> vertIndices(part.numVertices);
			for (uint16_t i = 0; i < static_cast<uint16_t>(vertIndices.size()); i++)
				vertIndices[i] = i;

			part.vertexMap = vertIndices;
		}

		if (!tris.empty()) {
			part.numTriangles = static_cast<uint16_t>(tris.size());
			part.trueTriangles = tris;
			if (!bMappedIndices)
				part.triangles = part.trueTriangles;
		}

		skinPart->bMappedIndices = bMappedIndices;
		skinPart->partitions.clear();
		skinPart->partitions.push_back(part);
		skinPart->numPartitions = 1;
		skinPart->triParts.clear();
	}
}

void NifFile::DeletePartitions(NiShape* shape, std::vector<uint32_t>& partInds) {
	if (!shape)
		return;

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (!skinInst)
		return;

	auto skinPart = hdr.GetBlock(skinInst->skinPartitionRef);
	if (!skinPart)
		return;

	skinPart->DeletePartitions(partInds);

	auto bsdSkinInst = dynamic_cast<BSDismemberSkinInstance*>(skinInst);
	if (bsdSkinInst) {
		bsdSkinInst->DeletePartitions(partInds);
		UpdatePartitionFlags(shape);
	}
}

bool NifFile::ReorderTriangles(NiShape* shape, const std::vector<uint32_t>& triangleIndices) {
	if (!shape)
		return false;

	if (shape->HasType<NiTriStrips>())
		return false;

	return shape->ReorderTriangles(triangleIndices);
}

const std::vector<Vector3>* NifFile::GetVertsForShape(NiShape* shape) {
	if (!shape)
		return nullptr;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData)
			return &geomData->vertices;
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape)
			return &bsTriShape->UpdateRawVertices();
	}
	return nullptr;
}

const std::vector<Vector3>* NifFile::GetNormalsForShape(NiShape* shape) {
	if (!shape || !shape->HasNormals())
		return nullptr;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData)
			return &geomData->normals;
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape)
			return &bsTriShape->UpdateRawNormals();
	}

	return nullptr;
}

const std::vector<Vector2>* NifFile::GetUvsForShape(NiShape* shape) {
	if (!shape)
		return nullptr;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && !geomData->uvSets.empty())
			return &geomData->uvSets[0];
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape)
			return &bsTriShape->UpdateRawUvs();
	}

	return nullptr;
}

const std::vector<Color4>* NifFile::GetColorsForShape(const std::string& shapeName) {
	auto shape = FindBlockByName<NiShape>(shapeName);
	return GetColorsForShape(shape);
}

const std::vector<Color4>* NifFile::GetColorsForShape(NiShape* shape) {
	if (!shape)
		return nullptr;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData)
			return &geomData->vertexColors;
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape)
			return &bsTriShape->UpdateRawColors();
	}

	return nullptr;
}

const std::vector<Vector3>* NifFile::GetTangentsForShape(NiShape* shape) {
	if (!shape || !shape->HasTangents())
		return nullptr;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData)
			return &geomData->tangents;
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape)
			return &bsTriShape->UpdateRawTangents();
	}

	return nullptr;
}

const std::vector<Vector3>* NifFile::GetBitangentsForShape(NiShape* shape) {
	if (!shape || !shape->HasTangents())
		return nullptr;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData)
			return &geomData->bitangents;
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape)
			return &bsTriShape->UpdateRawBitangents();
	}

	return nullptr;
}

const std::vector<float>* NifFile::GetEyeDataForShape(NiShape* shape) {
	if (!shape)
		return nullptr;

	auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
	if (bsTriShape)
		return &bsTriShape->UpdateRawEyeData();

	return nullptr;
}

bool NifFile::GetVertsForShape(NiShape* shape, std::vector<Vector3>& outVerts) const {
	if (!shape) {
		outVerts.clear();
		return false;
	}

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && geomData->HasVertices()) {
			outVerts = geomData->vertices;
			return true;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape) {
			outVerts.resize(bsTriShape->GetNumVertices());

			for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++)
				outVerts[i] = bsTriShape->vertData[i].vert;

			return true;
		}
	}

	outVerts.clear();
	return false;
}

bool NifFile::GetUvsForShape(NiShape* shape, std::vector<Vector2>& outUvs) const {
	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && geomData->HasUVs() && !geomData->uvSets.empty()) {
			outUvs = geomData->uvSets[0];
			return true;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape && bsTriShape->HasUVs()) {
			outUvs.resize(bsTriShape->GetNumVertices());

			for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++)
				outUvs[i] = bsTriShape->vertData[i].uv;

			return true;
		}
	}

	return false;
}

bool NifFile::GetColorsForShape(NiShape* shape, std::vector<Color4>& outColors) const {
	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && geomData->HasVertexColors()) {
			outColors = geomData->vertexColors;
			return true;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape && bsTriShape->HasVertexColors()) {
			outColors.resize(bsTriShape->GetNumVertices());

			for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++) {
				outColors[i].r = bsTriShape->vertData[i].colorData[0] / 255.0f;
				outColors[i].g = bsTriShape->vertData[i].colorData[1] / 255.0f;
				outColors[i].b = bsTriShape->vertData[i].colorData[2] / 255.0f;
				outColors[i].a = bsTriShape->vertData[i].colorData[3] / 255.0f;
			}

			return true;
		}
	}

	return false;
}

bool NifFile::GetTangentsForShape(NiShape* shape, std::vector<Vector3>& outTang) const {
	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && geomData->HasTangents()) {
			outTang = geomData->tangents;
			return true;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape && bsTriShape->HasTangents()) {
			outTang.resize(bsTriShape->GetNumVertices());

			for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++) {
				outTang[i].x = ((static_cast<float>(bsTriShape->vertData[i].tangent[0])) / 255.0f) * 2.0f
							   - 1.0f;
				outTang[i].y = ((static_cast<float>(bsTriShape->vertData[i].tangent[1])) / 255.0f) * 2.0f
							   - 1.0f;
				outTang[i].z = ((static_cast<float>(bsTriShape->vertData[i].tangent[2])) / 255.0f) * 2.0f
							   - 1.0f;
			}

			return true;
		}
	}

	return false;
}

bool NifFile::GetBitangentsForShape(NiShape* shape, std::vector<Vector3>& outBitang) const {
	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && geomData->HasTangents()) {
			outBitang = geomData->bitangents;
			return true;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape && bsTriShape->HasTangents()) {
			outBitang.resize(bsTriShape->GetNumVertices());

			for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++) {
				outBitang[i].x = bsTriShape->vertData[i].bitangentX;
				outBitang[i].y = ((static_cast<float>(bsTriShape->vertData[i].bitangentY)) / 255.0f) * 2.0f
								 - 1.0f;
				outBitang[i].z = ((static_cast<float>(bsTriShape->vertData[i].bitangentZ)) / 255.0f) * 2.0f
								 - 1.0f;
			}

			return true;
		}
	}

	return false;
}

bool NifFile::GetEyeDataForShape(NiShape* shape, std::vector<float>& outEyeData) {
	auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
	if (bsTriShape && bsTriShape->HasEyeData()) {
		outEyeData.resize(bsTriShape->GetNumVertices());

		for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++)
			outEyeData[i] = bsTriShape->vertData[i].eyeData;

		return true;
	}

	return false;
}

void NifFile::SetVertsForShape(NiShape* shape, const std::vector<Vector3>& verts) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData) {
			if (verts.size() != geomData->GetNumVertices())
				geomData->Create(hdr.GetVersion(), &verts, nullptr, nullptr, nullptr);
			else
				geomData->vertices = verts;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape) {
			if (verts.size() != bsTriShape->GetNumVertices()) {
				bsTriShape->Create(hdr.GetVersion(), &verts, nullptr, nullptr, nullptr);
			}
			else {
				for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++)
					bsTriShape->vertData[i].vert = verts[i];
			}
		}
	}
}

void NifFile::SetUvsForShape(NiShape* shape, const std::vector<Vector2>& uvs) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && uvs.size() == geomData->GetNumVertices()) {
			geomData->SetUVs(true);
			geomData->uvSets[0] = uvs;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape && uvs.size() == bsTriShape->GetNumVertices()) {
			bsTriShape->SetUVs(true);

			for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++)
				bsTriShape->vertData[i].uv = uvs[i];
		}
	}
}

void NifFile::SetColorsForShape(NiShape* shape, const std::vector<Color4>& colors) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && colors.size() == geomData->GetNumVertices()) {
			geomData->SetVertexColors(true);
			geomData->vertexColors = colors;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape && colors.size() == bsTriShape->GetNumVertices()) {
			bsTriShape->SetVertexColors(true);

			for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++) {
				auto& vertex = bsTriShape->vertData[i];

				float f = std::max(0.0f, std::min(1.0f, colors[i].r));
				vertex.colorData[0] = static_cast<uint8_t>(std::floor(f == 1.0f ? 255 : f * 256.0));

				f = std::max(0.0f, std::min(1.0f, colors[i].g));
				vertex.colorData[1] = static_cast<uint8_t>(std::floor(f == 1.0f ? 255 : f * 256.0));

				f = std::max(0.0f, std::min(1.0f, colors[i].b));
				vertex.colorData[2] = static_cast<uint8_t>(std::floor(f == 1.0f ? 255 : f * 256.0));

				f = std::max(0.0f, std::min(1.0f, colors[i].a));
				vertex.colorData[3] = static_cast<uint8_t>(std::floor(f == 1.0f ? 255 : f * 256.0));
			}
		}
	}
}

void NifFile::SetColorsForShape(const std::string& shapeName, const std::vector<Color4>& colors) {
	auto shape = FindBlockByName<NiShape>(shapeName);
	if (!shape)
		return;

	SetColorsForShape(shape, colors);
}

void NifFile::SetTangentsForShape(NiShape* shape, const std::vector<Vector3>& tangents) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData) {
			geomData->SetTangents(true);
			geomData->tangents = tangents;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape && tangents.size() == bsTriShape->GetNumVertices())
			bsTriShape->SetTangentData(tangents);
	}
}

void NifFile::SetBitangentsForShape(NiShape* shape, const std::vector<Vector3>& bitangents) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData) {
			geomData->SetTangents(true);
			geomData->bitangents = bitangents;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape && bitangents.size() == bsTriShape->GetNumVertices())
			bsTriShape->SetBitangentData(bitangents);
	}
}

void NifFile::SetEyeDataForShape(NiShape* shape, const std::vector<float>& eyeData) {
	if (!shape)
		return;

	auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
	if (bsTriShape && eyeData.size() == bsTriShape->GetNumVertices())
		bsTriShape->SetEyeData(eyeData);
}

NiBinaryExtraData* NifFile::GetBinaryTangentData(NiShape* shape,
												 std::vector<nifly::Vector3>* outTangents,
												 std::vector<nifly::Vector3>* outBitangents) const {
	if (!shape)
		return nullptr;

	uint16_t numVerts = shape->GetNumVertices();

	for (auto& extraData : shape->extraDataRefs) {
		auto binaryData = hdr.GetBlock<NiBinaryExtraData>(extraData);
		if (binaryData && binaryData->name.get() == "Tangent space (binormal & tangent vectors)") {
			uint32_t dataSize = numVerts * 4 * 3 * 2;
			if (binaryData->data.size() == dataSize) {
				auto vecPtr = reinterpret_cast<Vector3*>(binaryData->data.data());

				if (outTangents) {
					outTangents->resize(numVerts);

					for (uint16_t i = 0; i < numVerts; i++) {
						outTangents->at(i) = (*vecPtr);
						++vecPtr;
					}
				}
				else
					vecPtr += numVerts;

				if (outBitangents) {
					outBitangents->resize(numVerts);

					for (uint16_t i = 0; i < numVerts; i++) {
						outBitangents->at(i) = (*vecPtr);
						++vecPtr;
					}
				}
				else
					vecPtr += numVerts;
			}

			return binaryData;
		}
	}

	return nullptr;
}

void NifFile::SetBinaryTangentData(NiShape* shape,
								   const std::vector<nifly::Vector3>* tangents,
								   const std::vector<nifly::Vector3>* bitangents) {
	if (!shape || !tangents || !bitangents)
		return;

	uint16_t numVerts = shape->GetNumVertices();
	if (tangents->size() != numVerts || bitangents->size() != numVerts)
		return;

	NiBinaryExtraData* binaryData = nullptr;

	for (auto& extraData : shape->extraDataRefs) {
		auto binaryExtraData = hdr.GetBlock<NiBinaryExtraData>(extraData);
		if (binaryExtraData && binaryExtraData->name.get() == "Tangent space (binormal & tangent vectors)") {
			binaryData = binaryExtraData;
			break;
		}
	}

	if (!binaryData) {
		// Add new NiBinaryExtraData block
		NiBinaryExtraData binaryExtraData;
		binaryExtraData.name.get() = "Tangent space (binormal & tangent vectors)";

		uint32_t extraDataId = AssignExtraData(shape, binaryExtraData.Clone());
		binaryData = hdr.GetBlock<NiBinaryExtraData>(extraDataId);
	}

	if (!binaryData)
		return;

	uint32_t dataSize = numVerts * 4 * 3 * 2;
	binaryData->data.resize(dataSize);

	auto vecPtr = reinterpret_cast<Vector3*>(binaryData->data.data());

	for (uint16_t i = 0; i < numVerts; i++) {
		(*vecPtr) = tangents->at(i);
		++vecPtr;
	}

	for (uint16_t i = 0; i < numVerts; i++) {
		(*vecPtr) = bitangents->at(i);
		++vecPtr;
	}
}

void NifFile::DeleteBinaryTangentData(NiShape* shape) {
	if (!shape)
		return;

	for (auto& extraData : shape->extraDataRefs) {
		auto binaryExtraData = hdr.GetBlock<NiBinaryExtraData>(extraData);
		if (binaryExtraData && binaryExtraData->name.get() == "Tangent space (binormal & tangent vectors)")
			hdr.DeleteBlock(extraData);
	}
}

void NifFile::InvertUVsForShape(NiShape* shape, bool invertX, bool invertY) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && !geomData->uvSets.empty()) {
			if (invertX)
				for (auto& i : geomData->uvSets[0])
					i.u = 1.0f - i.u;

			if (invertY)
				for (auto& i : geomData->uvSets[0])
					i.v = 1.0f - i.v;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape) {
			if (invertX)
				for (auto& i : bsTriShape->vertData)
					i.uv.u = 1.0f - i.uv.u;

			if (invertY)
				for (auto& i : bsTriShape->vertData)
					i.uv.v = 1.0f - i.uv.v;
		}
	}
}

void NifFile::MirrorShape(NiShape* shape, bool mirrorX, bool mirrorY, bool mirrorZ) {
	if (!shape)
		return;

	bool flipTris = false;
	Matrix4 mirrorMat;

	if (mirrorX) {
		mirrorMat.Scale(-1.0f, 1.0f, 1.0f);
		flipTris = !flipTris;
	}

	if (mirrorY) {
		mirrorMat.Scale(1.0f, -1.0f, 1.0f);
		flipTris = !flipTris;
	}

	if (mirrorZ) {
		mirrorMat.Scale(1.0f, 1.0f, -1.0f);
		flipTris = !flipTris;
	}

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && !geomData->vertices.empty()) {
			for (auto& vertice : geomData->vertices)
				vertice = mirrorMat * vertice;

			for (auto& normal : geomData->normals)
				normal = mirrorMat * normal;

			for (auto& tangent : geomData->tangents)
				tangent = mirrorMat * tangent;

			for (auto& bitangent : geomData->bitangents)
				bitangent = mirrorMat * bitangent;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape) {
			for (auto& i : bsTriShape->vertData)
				i.vert = mirrorMat * i.vert;

			if (bsTriShape->HasNormals()) {
				bsTriShape->UpdateRawNormals();

				for (auto& normal : bsTriShape->rawNormals)
					normal = mirrorMat * normal;

				bsTriShape->SetNormals(bsTriShape->rawNormals);

				if (bsTriShape->HasTangents())
					bsTriShape->CalcTangentSpace();
			}
		}
	}

	if (flipTris) {
		std::vector<Triangle> tris;
		shape->GetTriangles(tris);

		for (auto& tri : tris)
			std::swap(tri.p1, tri.p3);

		shape->SetTriangles(tris);
	}
}

void NifFile::SetNormalsForShape(NiShape* shape, const std::vector<Vector3>& norms) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData) {
			geomData->SetNormals(true);
			geomData->normals = norms;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape)
			bsTriShape->SetNormals(norms);
	}
}

void NifFile::CalcNormalsForShape(NiShape* shape,
								  const bool force,
								  const bool smooth,
								  const float smoothThresh) {
	if (!shape)
		return;

	if (hdr.GetVersion().IsSK() || hdr.GetVersion().IsSSE()) {
		NiShader* shader = GetShader(shape);
		if (shader && shader->IsModelSpace() && !force)
			return;
	}

	std::unordered_set<uint32_t> lockedIndices;

	for (auto& extraDataRef : shape->extraDataRefs) {
		auto integersExtraData = hdr.GetBlock<NiIntegersExtraData>(extraDataRef);
		if (integersExtraData && integersExtraData->name == "LOCKEDNORM")
			for (auto& i : integersExtraData->integersData)
				lockedIndices.insert(i);
	}

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData)
			geomData->RecalcNormals(smooth, smoothThresh, lockedIndices.empty() ? nullptr : &lockedIndices);
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape)
			bsTriShape->RecalcNormals(smooth, smoothThresh, lockedIndices.empty() ? nullptr : &lockedIndices);
	}
}

void NifFile::CalcTangentsForShape(NiShape* shape) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData)
			geomData->CalcTangentSpace();
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape)
			bsTriShape->CalcTangentSpace();
	}
}

int NifFile::ApplyNormalsFromFile(NifFile& srcNif, const std::string& shapeName) {
	auto shape = FindBlockByName<NiShape>(shapeName);
	if (!shape)
		return -1;

	auto srcShape = srcNif.FindBlockByName<NiShape>(shapeName);
	if (!srcShape)
		return -2;

	std::unordered_set<uint32_t> lockedNormalIndices;

	// Get LOCKEDNORM from source
	NiIntegersExtraData* integersExtraData = nullptr;

	for (auto& extraDataRef : srcShape->extraDataRefs) {
		integersExtraData = srcNif.GetHeader().GetBlock<NiIntegersExtraData>(extraDataRef);
		if (integersExtraData && integersExtraData->name == "LOCKEDNORM")
			for (auto& i : integersExtraData->integersData)
				lockedNormalIndices.insert(i);
	}

	if (lockedNormalIndices.empty())
		return -3;

	// Get normals of target
	auto norms = GetNormalsForShape(shape);
	if (!norms)
		return -4;

	// Get normals of source
	auto srcNorms = srcNif.GetNormalsForShape(srcShape);
	if (!srcNorms)
		return -5;

	// Vertex count needs to match up
	if (norms->size() != srcNorms->size())
		return -6;

	auto workNorms = (*norms);

	// Copy locked normals of the source into the target
	for (auto& i : lockedNormalIndices) {
		auto& sn = srcNorms->at(i);
		workNorms[i] = sn;
	}

	SetNormalsForShape(shape, workNorms);

	for (auto& extraDataRef : shape->extraDataRefs) {
		auto oldIntegersExtraData = hdr.GetBlock<NiIntegersExtraData>(extraDataRef);
		if (oldIntegersExtraData && oldIntegersExtraData->name == "LOCKEDNORM")
			hdr.DeleteBlock(extraDataRef);
	}

	AssignExtraData(shape, integersExtraData->Clone());
	return 0;
}

void NifFile::GetRootTranslation(Vector3& outVec) const {
	auto root = GetRootNode();
	if (root)
		outVec = root->GetTransformToParent().translation;
	else
		outVec.Zero();
}

void NifFile::MoveVertex(NiShape* shape, const Vector3& pos, const int id) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData && geomData->GetNumVertices() > id)
			geomData->vertices[id] = pos;
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape && bsTriShape->GetNumVertices() > id)
			bsTriShape->vertData[id].vert = pos;
	}
}

void NifFile::OffsetShape(NiShape* shape, const Vector3& offset, std::unordered_map<uint16_t, float>* mask) {
	if (!shape)
		return;

	if (auto geomData = GetGeometryData(shape)) {
		if (geomData) {
			for (uint16_t i = 0; i < geomData->GetNumVertices(); i++) {
				if (mask) {
					float maskFactor = 1.0f;
					Vector3 diff = offset;
					if (mask->find(i) != mask->end()) {
						maskFactor = 1.0f - (*mask)[i];
						diff *= maskFactor;
					}
					geomData->vertices[i] += diff;
				}
				else
					geomData->vertices[i] += offset;
			}
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (bsTriShape) {
			for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++) {
				if (mask) {
					float maskFactor = 1.0f;
					Vector3 diff = offset;
					if (mask->find(i) != mask->end()) {
						maskFactor = 1.0f - (*mask)[i];
						diff *= maskFactor;
					}
					bsTriShape->vertData[i].vert += diff;
				}
				else
					bsTriShape->vertData[i].vert += offset;
			}
		}
	}
}

void NifFile::ScaleShape(NiShape* shape, const Vector3& scale, std::unordered_map<uint16_t, float>* mask) {
	if (!shape)
		return;

	Vector3 root;
	GetRootTranslation(root);

	if (auto geomData = GetGeometryData(shape)) {
		if (!geomData)
			return;

		std::unordered_map<uint16_t, Vector3> diff;
		for (uint16_t i = 0; i < geomData->GetNumVertices(); i++) {
			Vector3 target = geomData->vertices[i] - root;
			target.x *= scale.x;
			target.y *= scale.y;
			target.z *= scale.z;
			diff[i] = geomData->vertices[i] - target;

			if (mask) {
				float maskFactor = 1.0f;
				if (mask->find(i) != mask->end()) {
					maskFactor = 1.0f - (*mask)[i];
					diff[i] *= maskFactor;
					target = geomData->vertices[i] - root + diff[i];
				}
			}
			geomData->vertices[i] = target;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (!bsTriShape)
			return;

		std::unordered_map<uint16_t, Vector3> diff;
		for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++) {
			Vector3 target = bsTriShape->vertData[i].vert - root;
			target.x *= scale.x;
			target.y *= scale.y;
			target.z *= scale.z;
			diff[i] = bsTriShape->vertData[i].vert - target;

			if (mask) {
				float maskFactor = 1.0f;
				if (mask->find(i) != mask->end()) {
					maskFactor = 1.0f - (*mask)[i];
					diff[i] *= maskFactor;
					target = bsTriShape->vertData[i].vert - root + diff[i];
				}
			}
			bsTriShape->vertData[i].vert = target;
		}
	}
}

void NifFile::RotateShape(NiShape* shape, const Vector3& angle, std::unordered_map<uint16_t, float>* mask) {
	if (!shape)
		return;

	Vector3 root;
	GetRootTranslation(root);

	if (auto geomData = GetGeometryData(shape)) {
		if (!geomData)
			return;

		std::unordered_map<uint16_t, Vector3> diff;
		for (uint16_t i = 0; i < geomData->GetNumVertices(); i++) {
			Vector3 target = geomData->vertices[i] - root;
			Matrix4 mat;
			mat.Rotate(angle.x * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			mat.Rotate(angle.y * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));
			mat.Rotate(angle.z * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));
			target = mat * target;
			diff[i] = geomData->vertices[i] - target;

			if (mask) {
				float maskFactor = 1.0f;
				if (mask->find(i) != mask->end()) {
					maskFactor = 1.0f - (*mask)[i];
					diff[i] *= maskFactor;
					target = geomData->vertices[i] - root + diff[i];
				}
			}
			geomData->vertices[i] = target;
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
		if (!bsTriShape)
			return;

		std::unordered_map<uint16_t, Vector3> diff;
		for (uint16_t i = 0; i < bsTriShape->GetNumVertices(); i++) {
			Vector3 target = bsTriShape->vertData[i].vert - root;
			Matrix4 mat;
			mat.Rotate(angle.x * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			mat.Rotate(angle.y * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));
			mat.Rotate(angle.z * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));
			target = mat * target;
			diff[i] = bsTriShape->vertData[i].vert - target;

			if (mask) {
				float maskFactor = 1.0f;
				if (mask->find(i) != mask->end()) {
					maskFactor = 1.0f - (*mask)[i];
					diff[i] *= maskFactor;
					target = bsTriShape->vertData[i].vert - root + diff[i];
				}
			}
			bsTriShape->vertData[i].vert = target;
		}
	}
}

NiAlphaProperty* NifFile::GetAlphaProperty(NiShape* shape) const {
	if (shape->HasAlphaProperty())
		return hdr.GetBlock(shape->AlphaPropertyRef());

	for (auto& prop : shape->propertyRefs) {
		auto alphaProp = hdr.GetBlock<NiAlphaProperty>(prop);
		if (alphaProp)
			return alphaProp;
	}

	return nullptr;
}

uint32_t NifFile::AssignAlphaProperty(NiShape* shape, std::unique_ptr<NiAlphaProperty> alphaProp) {
	RemoveAlphaProperty(shape);

	NiShader* shader = GetShader(shape);
	if (shader) {
		int alphaRef = hdr.AddBlock(std::move(alphaProp));
		if (shader->HasType<BSShaderPPLightingProperty>() || shader->HasType<NiMaterialProperty>())
			shape->propertyRefs.AddBlockRef(alphaRef);
		else if (shape->AlphaPropertyRef())
			shape->AlphaPropertyRef()->index = alphaRef;

		return alphaRef;
	}

	return NIF_NPOS;
}

void NifFile::RemoveAlphaProperty(NiShape* shape) {
	auto alpha = hdr.GetBlock(shape->AlphaPropertyRef());
	if (alpha) {
		hdr.DeleteBlock(*shape->AlphaPropertyRef());
		shape->AlphaPropertyRef()->Clear();
	}

	for (uint32_t i = 0; i < shape->propertyRefs.GetSize(); i++) {
		alpha = hdr.GetBlock<NiAlphaProperty>(shape->propertyRefs.GetBlockRef(i));
		if (alpha) {
			hdr.DeleteBlock(shape->propertyRefs.GetBlockRef(i));
			shape->propertyRefs.RemoveBlockRef(i);
			i--;
			continue;
		}
	}
}

void NifFile::DeleteShape(NiShape* shape) {
	if (!shape)
		return;

	if (shape->HasData())
		hdr.DeleteBlock(*shape->DataRef());

	if (shape->HasShaderProperty()) {
		if (hdr.GetBlockRefCount(shape->ShaderPropertyRef()->index, false) == 1)
			DeleteShader(shape);
	}
	else
		DeleteShader(shape); // Call anyway (for shaders in property refs)

	DeleteSkinning(shape);

	for (int i = shape->propertyRefs.GetSize() - 1; i >= 0; --i) {
		hdr.DeleteBlock(shape->propertyRefs.GetBlockRef(i));
		shape->propertyRefs.RemoveBlockRef(i);
	}

	for (int i = shape->extraDataRefs.GetSize() - 1; i >= 0; --i) {
		hdr.DeleteBlock(shape->extraDataRefs.GetBlockRef(i));
		shape->extraDataRefs.RemoveBlockRef(i);
	}

	int shapeID = GetBlockID(shape);
	hdr.DeleteBlock(shapeID);
}

void NifFile::DeleteShader(NiShape* shape) {
	auto shader = hdr.GetBlock(shape->ShaderPropertyRef());
	if (shader) {
		if (shader->HasTextureSet()) {
			if (hdr.GetBlockRefCount(shader->TextureSetRef()->index, false) == 1)
				hdr.DeleteBlock(*shader->TextureSetRef());
		}

		hdr.DeleteBlock(shader->controllerRef);
		hdr.DeleteBlock(*shape->ShaderPropertyRef());
		shape->ShaderPropertyRef()->Clear();
	}

	RemoveAlphaProperty(shape);

	for (uint32_t i = 0; i < shape->propertyRefs.GetSize(); i++) {
		shader = hdr.GetBlock<NiShader>(shape->propertyRefs.GetBlockRef(i));
		if (shader) {
			if (shader->HasType<BSShaderPPLightingProperty>() || shader->HasType<NiMaterialProperty>()) {
				if (shader->HasTextureSet()) {
					if (hdr.GetBlockRefCount(shader->TextureSetRef()->index, false) == 1)
						hdr.DeleteBlock(*shader->TextureSetRef());
				}

				hdr.DeleteBlock(shader->controllerRef);
				hdr.DeleteBlock(shape->propertyRefs.GetBlockRef(i));
				shape->propertyRefs.RemoveBlockRef(i);
				i--;
				continue;
			}
		}
	}
}

void NifFile::DeleteSkinning(NiShape* shape) {
	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (skinInst) {
		hdr.DeleteBlock(skinInst->dataRef);
		hdr.DeleteBlock(skinInst->skinPartitionRef);

		if (shape->HasSkinInstance()) {
			hdr.DeleteBlock(*shape->SkinInstanceRef());
			shape->SkinInstanceRef()->Clear();
		}
	}

	auto bsSkinInst = hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef());
	if (bsSkinInst) {
		hdr.DeleteBlock(bsSkinInst->dataRef);

		if (shape->HasSkinInstance()) {
			hdr.DeleteBlock(*shape->SkinInstanceRef());
			shape->SkinInstanceRef()->Clear();
		}
	}

	shape->SetSkinned(false);

	NiShader* shader = GetShader(shape);
	if (shader)
		shader->SetSkinned(false);
}

void NifFile::RemoveEmptyPartitions(NiShape* shape) {
	if (!shape)
		return;

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (skinInst) {
		auto skinPartition = hdr.GetBlock(skinInst->skinPartitionRef);
		if (skinPartition) {
			std::vector<uint32_t> emptyIndices;
			if (skinPartition->RemoveEmptyPartitions(emptyIndices)) {
				auto bsdSkinInst = dynamic_cast<BSDismemberSkinInstance*>(skinInst);
				if (bsdSkinInst) {
					bsdSkinInst->DeletePartitions(emptyIndices);
					UpdatePartitionFlags(shape);
				}
			}
		}
	}
}

bool NifFile::DeleteVertsForShape(NiShape* shape, const std::vector<uint16_t>& indices) {
	if (indices.empty())
		return false;

	if (!shape)
		return false;

	bool allVertsDeleted = false;

	auto geomData = hdr.GetBlock<NiTriBasedGeomData>(shape->DataRef());
	if (geomData) {
		geomData->notifyVerticesDelete(indices);
		if (geomData->GetNumVertices() == 0 || geomData->GetNumTriangles() == 0) {
			// Deleted all verts or tris
			allVertsDeleted = true;
		}
	}

	auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
	if (bsTriShape) {
		bsTriShape->notifyVerticesDelete(indices);
		if (bsTriShape->GetNumVertices() == 0 || bsTriShape->GetNumTriangles() == 0) {
			// Deleted all verts or tris
			allVertsDeleted = true;
		}
	}

	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (skinInst) {
		auto skinData = hdr.GetBlock(skinInst->dataRef);
		if (skinData)
			skinData->notifyVerticesDelete(indices);

		auto skinPartition = hdr.GetBlock(skinInst->skinPartitionRef);
		if (skinPartition) {
			skinPartition->notifyVerticesDelete(indices);

			std::vector<uint32_t> emptyIndices;
			if (skinPartition->RemoveEmptyPartitions(emptyIndices)) {
				auto bsdSkinInst = dynamic_cast<BSDismemberSkinInstance*>(skinInst);
				if (bsdSkinInst) {
					bsdSkinInst->DeletePartitions(emptyIndices);
					UpdatePartitionFlags(shape);
				}
			}
		}
	}

	for (auto& extraDataRef : shape->extraDataRefs) {
		auto integersExtraData = hdr.GetBlock<NiIntegersExtraData>(extraDataRef);
		if (integersExtraData && integersExtraData->name == "LOCKEDNORM") {
			auto integersData = integersExtraData->integersData;
			std::sort(integersData.begin(), integersData.end());

			uint16_t highestRemoved = indices.back();
			uint16_t mapSize = highestRemoved + 1;
			std::vector<int> indexCollapse = GenerateIndexCollapseMap(indices, mapSize);

			for (uint32_t i = integersData.size() - 1; i != NIF_NPOS; i--) {
				auto& val = integersData[i];
				if (val > highestRemoved) {
					val -= static_cast<uint32_t>(indices.size());
				}
				else if (indexCollapse[val] == -1) {
					integersData.erase(i);
				}
				else
					val = indexCollapse[val];
			}

			integersExtraData->integersData = std::move(integersData);
		}
	}

	return allVertsDeleted;
}

int NifFile::CalcShapeDiff(NiShape* shape,
						   const std::vector<Vector3>* targetData,
						   std::unordered_map<uint16_t, Vector3>& outDiffData,
						   float scale) {
	outDiffData.clear();

	const std::vector<Vector3>* myData = GetVertsForShape(shape);
	if (!myData)
		return 1;

	if (!targetData)
		return 2;

	if (myData->size() != targetData->size())
		return 3;

	for (uint16_t i = 0; i < static_cast<uint16_t>(myData->size()); i++) {
		auto& target = targetData->at(i);
		auto& src = myData->at(i);

		Vector3 v;
		v.x = (target.x * scale) - src.x;
		v.y = (target.y * scale) - src.y;
		v.z = (target.z * scale) - src.z;

		if (v.IsZero(true))
			continue;

		outDiffData[i] = v;
	}

	return 0;
}

int NifFile::CalcUVDiff(NiShape* shape,
						const std::vector<Vector2>* targetData,
						std::unordered_map<uint16_t, Vector3>& outDiffData,
						float scale) {
	outDiffData.clear();

	const std::vector<Vector2>* myData = GetUvsForShape(shape);
	if (!myData)
		return 1;

	if (!targetData)
		return 2;

	if (myData->size() != targetData->size())
		return 3;

	for (uint16_t i = 0; i < static_cast<uint16_t>(myData->size()); i++) {
		Vector3 v;
		v.x = (targetData->at(i).u - myData->at(i).u) * scale;
		v.y = (targetData->at(i).v - myData->at(i).v) * scale;

		if (v.IsZero(true))
			continue;

		outDiffData[i] = v;
	}

	return 0;
}

void NifFile::UpdateSkinPartitions(NiShape* shape) {
	NiSkinData* skinData = nullptr;
	NiSkinPartition* skinPart = nullptr;
	auto skinInst = hdr.GetBlock<NiSkinInstance>(shape->SkinInstanceRef());
	if (skinInst) {
		skinData = hdr.GetBlock(skinInst->dataRef);
		skinPart = hdr.GetBlock(skinInst->skinPartitionRef);

		if (!skinData || !skinPart)
			return;
	}
	else
		return;

	std::vector<Triangle> tris;
	if (!shape->GetTriangles(tris))
		return;

	auto bsdSkinInst = dynamic_cast<BSDismemberSkinInstance*>(skinInst);
	auto bsTriShape = dynamic_cast<BSTriShape*>(shape);
	if (bsTriShape)
		bsTriShape->CalcDataSizes(hdr.GetVersion());

	// Align triangles for comparisons
	for (auto& t : tris)
		t.rot();

	// Make maps of vertices to bones and weights
	std::unordered_map<uint16_t, std::vector<SkinWeight>> vertBoneWeights;
	uint16_t boneIndex = 0;
	for (auto& bone : skinData->bones) {
		for (auto& bw : bone.vertexWeights)
			vertBoneWeights[bw.index].push_back(SkinWeight(boneIndex, bw.weight));

		boneIndex++;
	}

	// Sort weights and corresponding bones
	for (auto& bw : vertBoneWeights)
		sort(bw.second.begin(), bw.second.end(), BoneWeightsSort());

	// Enforce maximum vertex bone weight count
	const uint16_t maxBonesPerVertex = 4;

	for (auto& bw : vertBoneWeights)
		if (bw.second.size() > maxBonesPerVertex)
			bw.second.resize(maxBonesPerVertex);

	skinPart->PrepareTriParts(tris);
	std::vector<int>& triParts = skinPart->triParts;

	uint16_t maxBonesPerPartition = std::numeric_limits<uint16_t>::max();
	if (hdr.GetVersion().IsOB() || hdr.GetVersion().IsFO3())
		maxBonesPerPartition = 18;
	else if (hdr.GetVersion().IsSSE())
		maxBonesPerPartition = 80;

	// Make a list of the bones used by each partition.  If any partition
	// has too many bones, split it.
	std::vector<std::set<int>> partBones(skinPart->partitions.size());
	for (size_t triIndex = 0; triIndex < tris.size(); ++triIndex) {
		int partInd = triParts[triIndex];
		if (partInd < 0)
			continue;

		Triangle tri = tris[triIndex];

		// Get associated bones for the current tri
		std::set<int> triBones;
		for (uint32_t i = 0; i < 3; i++)
			for (auto& tb : vertBoneWeights[tri[i]])
				triBones.insert(tb.index);

		// How many new bones are in the tri's bone list?
		uint16_t newBoneCount = 0;
		for (auto& tb : triBones)
			if (partBones[partInd].find(tb) == partBones[partInd].end())
				newBoneCount++;

		const auto partBonesSize = static_cast<uint16_t>(partBones[partInd].size());
		if (partBonesSize + newBoneCount > maxBonesPerPartition) {
			// Too many bones for this partition, make a new partition starting with this triangle
			for (size_t j = 0; j < tris.size(); ++j)
				if (triParts[j] > partInd || (j >= triIndex && triParts[j] >= partInd))
					++triParts[j];

			partBones.insert(partBones.begin() + partInd + 1, std::set<int>());

			if (bsdSkinInst) {
				BSDismemberSkinInstance::PartitionInfo info;
				info.flags = PF_EDITOR_VISIBLE;
				info.partID = bsdSkinInst->partitions[partInd].partID;
				bsdSkinInst->partitions.insert(partInd + 1, info);
			}

			++partInd;
		}

		partBones[partInd].insert(triBones.begin(), triBones.end());
	}

	// Re-create partitions
	std::vector<NiSkinPartition::PartitionBlock> partitions(partBones.size());
	for (size_t partInd = 0; partInd < partBones.size(); partInd++) {
		NiSkinPartition::PartitionBlock& part = partitions[partInd];
		part.hasBoneIndices = true;
		part.hasFaces = true;
		part.hasVertexMap = true;
		part.hasVertexWeights = true;
		part.numWeightsPerVertex = maxBonesPerVertex;
	}
	skinPart->numPartitions = static_cast<uint32_t>(partitions.size());
	skinPart->partitions = std::move(partitions);

	// Re-create trueTriangles, vertexMap, and triangles for each partition
	skinPart->GenerateTrueTrianglesFromTriParts(tris);
	skinPart->PrepareVertexMapsAndTriangles();

	for (uint32_t partInd = 0; partInd < skinPart->numPartitions; ++partInd) {
		NiSkinPartition::PartitionBlock& part = skinPart->partitions[partInd];

		// Copy relevant data from shape to partition
		if (bsTriShape)
			part.vertexDesc = bsTriShape->vertexDesc;

		std::unordered_map<int, uint8_t> boneLookup;
		boneLookup.reserve(partBones[partInd].size());
		part.numBones = static_cast<uint16_t>(partBones[partInd].size());
		part.bones.reserve(part.numBones);

		for (auto& b : partBones[partInd]) {
			part.bones.push_back(static_cast<uint16_t>(b));
			boneLookup[b] = static_cast<uint8_t>(part.bones.size() - 1);
		}

		for (auto& v : part.vertexMap) {
			BoneIndices b;
			VertexWeight vw;

			uint8_t* pb = &b.i1;
			float* pw = &vw.w1;

			float tot = 0.0f;
			for (size_t bi = 0; bi < vertBoneWeights[v].size(); bi++) {
				if (bi == 4)
					break;

				pb[bi] = boneLookup[vertBoneWeights[v][bi].index];
				pw[bi] = vertBoneWeights[v][bi].weight;
				tot += pw[bi];
			}

			if (tot != 0.0f)
				for (int bi = 0; bi < 4; bi++)
					pw[bi] /= tot;

			part.boneIndices.push_back(b);
			part.vertexWeights.push_back(vw);
		}
	}

	if (bsTriShape) {
		skinPart->numVertices = bsTriShape->GetNumVertices();
		skinPart->dataSize = bsTriShape->dataSize;
		skinPart->vertexSize = bsTriShape->vertexSize;
		skinPart->vertData = bsTriShape->vertData;
		skinPart->vertexDesc = bsTriShape->vertexDesc;
	}

	UpdatePartitionFlags(shape);
}

void NifFile::UpdatePartitionFlags(NiShape* shape) {
	auto bsdSkinInst = hdr.GetBlock<BSDismemberSkinInstance>(shape->SkinInstanceRef());
	if (!bsdSkinInst)
		return;

	auto skinPart = hdr.GetBlock(bsdSkinInst->skinPartitionRef);
	if (!skinPart)
		return;

	for (uint32_t i = 0; i < bsdSkinInst->partitions.size(); i++) {
		PartitionFlags flags = PF_NONE;

		if (hdr.GetVersion().IsFO3()) {
			// Don't make FO3/NV meat caps visible
			if (bsdSkinInst->partitions[i].partID < 100 || bsdSkinInst->partitions[i].partID >= 1000)
				flags = PartitionFlags(flags | PF_EDITOR_VISIBLE);
		}
		else
			flags = PartitionFlags(flags | PF_EDITOR_VISIBLE);

		if (i != 0) {
			// Start a new set if the previous bones are different
			if (skinPart->partitions[i].bones != skinPart->partitions[i - 1].bones)
				flags = PartitionFlags(flags | PF_START_NET_BONESET);
		}
		else
			flags = PartitionFlags(flags | PF_START_NET_BONESET);

		bsdSkinInst->partitions[i].flags = flags;
	}
}

void NifFile::CreateSkinning(NiShape* shape) {
	if (shape->HasType<NiTriShape>() || shape->HasType<NiTriStrips>()) {
		if (shape->SkinInstanceRef()->IsEmpty()) {
			int skinDataID = hdr.AddBlock(std::make_unique<NiSkinData>());
			int partID = hdr.AddBlock(std::make_unique<NiSkinPartition>());

			NiSkinInstance* skinInst;
			int skinInstID;

			if (hdr.GetVersion().File() == NiFileVersion::V20_2_0_7) {
				auto [nifDismemberInstS, nifDismemberInst] = make_unique<BSDismemberSkinInstance>();
				skinInstID = hdr.AddBlock(std::move(nifDismemberInstS));
				skinInst = nifDismemberInst;
			}
			else {
				auto [nifSkinInstS, nifSkinInst] = make_unique<NiSkinInstance>();
				skinInstID = hdr.AddBlock(std::move(nifSkinInstS));
				skinInst = nifSkinInst;
			}

			skinInst->dataRef.index = skinDataID;
			skinInst->skinPartitionRef.index = partID;
			skinInst->targetRef.index = GetBlockID(GetRootNode());
			shape->SkinInstanceRef()->index = skinInstID;
			shape->SetSkinned(true);

			SetDefaultPartition(shape);
		}
	}
	else if (shape->HasType<BSTriShape>()) {
		if (shape->SkinInstanceRef()->IsEmpty()) {
			int skinInstID = 0;
			if (hdr.GetVersion().Stream() == 100) {
				int skinDataID = hdr.AddBlock(std::make_unique<NiSkinData>());

				auto nifSkinPartition = std::make_unique<NiSkinPartition>();
				nifSkinPartition->bMappedIndices = false;
				int partID = hdr.AddBlock(std::move(nifSkinPartition));

				auto nifDismemberInst = std::make_unique<BSDismemberSkinInstance>();

				nifDismemberInst->dataRef.index = skinDataID;
				nifDismemberInst->skinPartitionRef.index = partID;
				nifDismemberInst->targetRef.index = GetBlockID(GetRootNode());

				skinInstID = hdr.AddBlock(std::move(nifDismemberInst));

				shape->SkinInstanceRef()->index = skinInstID;
				shape->SetSkinned(true);

				SetDefaultPartition(shape);
				UpdateSkinPartitions(shape);
			}
			else {
				auto [newSkinInstS, newSkinInst] = make_unique<BSSkinInstance>();
				skinInstID = hdr.AddBlock(std::move(newSkinInstS));

				int boneDataRef = hdr.AddBlock(std::make_unique<BSSkinBoneData>());

				newSkinInst->targetRef.index = GetBlockID(GetRootNode());
				newSkinInst->dataRef.index = boneDataRef;

				shape->SkinInstanceRef()->index = skinInstID;
				shape->SetSkinned(true);
			}
		}
	}

	NiShader* shader = GetShader(shape);
	if (shader)
		shader->SetSkinned(true);
}

void NifFile::SetShapeDynamic(const std::string& shapeName) {
	auto shape = FindBlockByName<NiShape>(shapeName);
	if (!shape)
		return;

	// Set consistency flag to mutable
	auto geomData = hdr.GetBlock<NiGeometryData>(shape->DataRef());
	if (geomData)
		geomData->consistencyFlags = CT_MUTABLE;
}
