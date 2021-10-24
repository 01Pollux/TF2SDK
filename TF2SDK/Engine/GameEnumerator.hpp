#pragma once

#include "TF2Config.hpp"
#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN();

class ISpatialPartition;

TF2_EXPORT_INTERFACE(ISpatialPartition, SpatialPartition, "SpatialPartition001");

struct GameRay;
class IBaseEntityInternal;

namespace Const
{
	namespace GameTracePartition
	{
		// every edict_t that isn't SOLID_TRIGGER or SOLID_NOT (and static props)
		static constexpr uint32_t EngineSolidEdicts		= 1 << 0;
		// every edict_t that IS SOLID_TRIGGER
		static constexpr uint32_t EngineTriggerEdicts	= 1 << 1;
		static constexpr uint32_t ClientSolidEdicts		= 1 << 2;
		// these are client-side only objects that respond to being forces, etc.
		static constexpr uint32_t ClientResponsiveEdicts= 1 << 3;
		// everything in solid & trigger except the static props, includes SOLID_NOTs
		static constexpr uint32_t EngineNonStaticEdicts	= 1 << 4;
		static constexpr uint32_t ClientStaticProps		= 1 << 5;
		static constexpr uint32_t EngineStaticProps		= 1 << 6;
		// everything except the static props
		static constexpr uint32_t ClientNonStaticProps	= 1 << 7;


		// These are the only handles in the spatial partition that the game is controlling (everything but static props)
		// These masks are used to handle updating the dirty spatial partition list in each game DLL
		static constexpr uint32_t ClientGameEdicts = ClientNonStaticProps | ClientResponsiveEdicts | ClientSolidEdicts;
		static constexpr uint32_t ServerGameEdicts = EngineSolidEdicts | EngineTriggerEdicts | EngineNonStaticEdicts;
		// Use this to look for all client edicts.
		static constexpr uint32_t ClientAllEdicts = ClientGameEdicts | ClientStaticProps;
	}

	//-----------------------------------------------------------------------------
	// Clients that want to know about all elements within a particular
	// volume must inherit from this
	//-----------------------------------------------------------------------------
	enum class PartitionRetVal
	{
		Continue,
		Stop
	};

	using SpatialPartitionHandle = unsigned short;
	using SpatialPartitionListMask = int;
	using SpatialTempHandle = int;

	constexpr SpatialPartitionHandle PARTITION_INVALID_HANDLE = static_cast<SpatialPartitionHandle>(~0);
}

//-----------------------------------------------------------------------------
// Any search in the CSpatialPartition must use this to filter out entities it doesn't want.
// You're forced to use listMasks because it can filter by listMasks really fast. Any other
// filtering can be done by EnumElement.
//-----------------------------------------------------------------------------

class IPartitionEnumerator
{
public:
	virtual Const::PartitionRetVal EnumElement(IBaseEntityInternal* pEntity) abstract;
};


//-----------------------------------------------------------------------------
// Installs a callback to call right before a spatial partition query occurs
//-----------------------------------------------------------------------------
class IPartitionQueryCallback
{
public:
	virtual void OnPreQuery_V1() abstract;
	virtual void OnPreQuery(Const::SpatialPartitionListMask listMask) abstract;
	virtual void OnPostQuery(Const::SpatialPartitionListMask listMask) abstract;
};


//-----------------------------------------------------------------------------
// This is the spatial partition manager, groups objects into buckets
//-----------------------------------------------------------------------------
class ISpatialPartition
{
public:
	// Add a virtual destructor to silence the clang warning.
	// This is harmless but not important since the only derived class
	// doesn't have a destructor.
	virtual ~ISpatialPartition() = default;

	// Create/destroy a handle for this dude in our system. Destroy
	// will also remove it from all lists it happens to be in
	virtual Const::SpatialPartitionHandle CreateHandle(IBaseEntityInternal* pEntity) abstract;

	// A fast method of creating a handle + inserting into the tree in the right place
	virtual Const::SpatialPartitionHandle CreateHandle(IBaseEntityInternal* pEntity,
		Const::SpatialPartitionListMask listMask, const Vector3D_F& mins, const Vector3D_F& maxs) abstract;

	virtual void DestroyHandle(Const::SpatialPartitionHandle handle) abstract;

	// Adds, removes an handle from a particular spatial partition list
	// There can be multiple partition lists; each has a unique id
	virtual void Insert(Const::SpatialPartitionListMask listMask,
		Const::SpatialPartitionHandle handle) abstract;
	virtual void Remove(Const::SpatialPartitionListMask listMask,
		Const::SpatialPartitionHandle handle) abstract;

	// Same as calling Remove() then Insert(). For performance-sensitive areas where you want to save a call.
	virtual void RemoveAndInsert(Const::SpatialPartitionListMask removeMask, Const::SpatialPartitionListMask insertMask,
		Const::SpatialPartitionHandle handle) abstract;

	// This will remove a particular handle from all lists
	virtual void Remove(Const::SpatialPartitionHandle handle) abstract;

	// Call this when an entity moves...
	virtual void ElementMoved(Const::SpatialPartitionHandle handle,
		const Vector3D_F& mins, const Vector3D_F& maxs) abstract;

	// A fast method to insert + remove a handle from the tree...
	// This is used to suppress collision of a single model..
	virtual Const::SpatialTempHandle HideElement(Const::SpatialPartitionHandle handle) abstract;
	virtual void UnhideElement(Const::SpatialPartitionHandle handle, Const::SpatialTempHandle tempHandle) abstract;

	// Installs callbacks to get called right before a query occurs
	virtual void InstallQueryCallback_V1(IPartitionQueryCallback* pCallback) abstract;
	virtual void RemoveQueryCallback(IPartitionQueryCallback* pCallback) abstract;

	// Gets all entities in a particular volume...
	// if coarseTest == true, it'll return all elements that are in
	// spatial partitions that intersect the box
	// if coarseTest == false, it'll return only elements that truly intersect
	virtual void EnumerateElementsInBox(
		Const::SpatialPartitionListMask listMask,
		const Vector3D_F& mins,
		const Vector3D_F& maxs,
		bool coarseTest,
		IPartitionEnumerator* pIterator
	) abstract;

	virtual void EnumerateElementsInSphere(
		Const::SpatialPartitionListMask listMask,
		const Vector3D_F& origin,
		float radius,
		bool coarseTest,
		IPartitionEnumerator* pIterator
	) abstract;

	virtual void EnumerateElementsAlongRay(
		Const::SpatialPartitionListMask listMask,
		const GameRay& ray,
		bool coarseTest,
		IPartitionEnumerator* pIterator
	) abstract;

	virtual void EnumerateElementsAtPoint(
		Const::SpatialPartitionListMask listMask,
		const Vector3D_F& pt,
		bool coarseTest,
		IPartitionEnumerator* pIterator
	) abstract;

	// For debugging.... suppress queries on particular lists
	virtual void SuppressLists(Const::SpatialPartitionListMask nListMask, bool bSuppress) abstract;
	virtual Const::SpatialPartitionListMask GetSuppressedLists() abstract;

	virtual void RenderAllObjectsInTree(float flTime) abstract;
	virtual void RenderObjectsInPlayerLeafs(const Vector3D_F& vecPlayerMin, const Vector3D_F& vecPlayerMax, float flTime) abstract;
	virtual void RenderLeafsForRayTraceStart(float flTime) abstract;
	virtual void RenderLeafsForRayTraceEnd() abstract;
	virtual void RenderLeafsForHullTraceStart(float flTime) abstract;
	virtual void RenderLeafsForHullTraceEnd() abstract;
	virtual void RenderLeafsForBoxStart(float flTime) abstract;
	virtual void RenderLeafsForBoxEnd() abstract;
	virtual void RenderLeafsForSphereStart(float flTime) abstract;
	virtual void RenderLeafsForSphereEnd() abstract;

	virtual void RenderObjectsInBox(const Vector3D_F& vecMin, const Vector3D_F& vecMax, float flTime) abstract;
	virtual void RenderObjectsInSphere(const Vector3D_F& vecCenter, float flRadius, float flTime) abstract;
	virtual void RenderObjectsAlongRay(const GameRay& ray, float flTime) abstract;

	virtual void ReportStats(const char* pFileName) abstract;

	virtual void InstallQueryCallback(IPartitionQueryCallback* pCallback) abstract;
};

TF2_NAMESPACE_END();
