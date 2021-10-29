#pragma once 

#include <functional>
#include <array>

#include "GameTrace.hpp"
#include "Math/Vector.hpp"
#include "Utils/UtlVector.hpp"


TF2_NAMESPACE_BEGIN();

class IEngineTrace;

namespace Interfaces
{
	TF2_EXPORT_INTERFACE(IEngineTrace, ClientTrace, "EngineTraceClient003");
}


class ICollideable;
struct GameRay;
class GameTrace;
class ITraceFilter;
class ITraceListData;
class IEntityEnumerator;
class IPhysCollide;
class IHandleEntity;
class IBaseEntityInternal;

class IEngineTrace
{
public:
	// Returns the contents mask + entity at a particular world-space position
	virtual int		GetPointContents(const Vector3D_F& vecAbsPosition, IHandleEntity** ppEntity = nullptr) abstract;

	// Get the point contents, but only test the specific entity. This works
	// on static props and brush models.
	//
	// If the entity isn't a static prop or a brush model, it returns CONTENTS_EMPTY and sets
	// bFailed to true if bFailed is non-null.
	virtual int		GetPointContents_Collideable(ICollideable* pCollide, const Vector3D_F& vecAbsPosition) abstract;

	// Traces a ray against a particular entity
	virtual void	ClipRayToEntity(const GameRay& ray, unsigned int fMask, IHandleEntity* pEnt, GameTrace* pTrace) abstract;

	// Traces a ray against a particular entity
	virtual void	ClipRayToCollideable(const GameRay& ray, unsigned int fMask, ICollideable* pCollide, GameTrace* pTrace) abstract;

	// A version that simply accepts a ray (can work as a traceline or tracehull)
	virtual void	TraceRay(const GameRay& ray, unsigned int fMask, ITraceFilter* pTraceFilter, GameTrace* pTrace) abstract;

	// A version that sets up the leaf and entity lists and allows you to pass those in for collision.
	virtual void	SetupLeafAndEntityListRay(const GameRay& ray, ITraceListData& traceData) abstract;
	virtual void    SetupLeafAndEntityListBox(const Vector3D_F& vecBoxMin, const Vector3D_F& vecBoxMax, ITraceListData& traceData) abstract;
	virtual void	TraceRayAgainstLeafAndEntityList(const GameRay& ray, ITraceListData& traceData, unsigned int fMask, ITraceFilter* pTraceFilter, GameTrace* pTrace) abstract;

	// A version that sweeps a collideable through the world
	// abs start + abs end represents the collision origins you want to sweep the collideable through
	// vecAngles represents the collision angles of the collideable during the sweep
	virtual void	SweepCollideable(ICollideable* pCollide, const Vector3D_F& vecAbsStart, const Vector3D_F& vecAbsEnd,
		const Angle_F& vecAngles, unsigned int fMask, ITraceFilter* pTraceFilter, GameTrace* pTrace) abstract;

	// Enumerates over all entities along a ray
	// If triggers == true, it enumerates all triggers along a ray
	virtual void	EnumerateEntities(const GameRay& ray, bool triggers, IEntityEnumerator* pEnumerator) abstract;

	// Same thing, but enumerate entitys within a box
	virtual void	EnumerateEntities(const Vector3D_F& vecAbsMins, const Vector3D_F& vecAbsMaxs, IEntityEnumerator* pEnumerator) abstract;

	// Convert a handle entity to a collideable.  Useful inside enumer
	virtual ICollideable* GetCollideable(IHandleEntity* pEntity) abstract;

	// HACKHACK: Temp for performance measurments
	virtual int GetStatByIndex(int index, bool bClear) abstract;


	//finds brushes in an AABB, prone to some false positives
	virtual void GetBrushesInAABB(const Vector3D_F& vMins, const Vector3D_F& vMaxs, Utils::UtlVector<int>* pOutput, int iContentsMask = 0xFFFFFFFF) abstract;

	//Creates a CPhysCollide out of all displacements wholly or partially contained in the specified AABB
	virtual IPhysCollide* GetCollidableFromDisplacementsInAABB(const Vector3D_F& vMins, const Vector3D_F& vMaxs) abstract;

	//retrieve brush planes and contents, returns true if data is being returned in the output pointers, false if the brush doesn't exist
	virtual bool GetBrushInfo(int iBrush, Utils::UtlVector<Vector4D_F>* pPlanesOut, int* pContentsOut) abstract;

	virtual bool PointOutsideWorld(const Vector3D_F& ptTest) abstract; //Tests a point to see if it's outside any playable area

	// Walks bsp to find the leaf containing the specified point
	virtual int GetLeafContainingPoint(const Vector3D_F& ptTest) abstract;
};


struct GameRay
{
	Vector4D_F  Start;	// starting point, centered within the extents
	Vector4D_F  Delta;	// direction + length of the ray
	Vector4D_F  StartOffset;	// Add this to m_Start to get the actual ray start
	Vector4D_F  Extents;	// Describes an axis aligned box extruded along a ray
	bool		IsRay;	// are the extents zero?
	bool		IsSwept;	// is delta != 0?

	GameRay() = default;
	GameRay(const Vector3D_F& start, const Vector3D_F& end) noexcept { init(start, end); }
	GameRay(const Vector3D_F& start, const Vector3D_F& end, const Vector3D_F& mins, const Vector3D_F& maxs) noexcept { init(start, end, mins, maxs); }

	GameRay(const Vector4D_F& start, const Vector4D_F& end) noexcept { init(start, end); }
	GameRay(const Vector4D_F& start, const Vector4D_F& end, const Vector4D_F& mins, const Vector4D_F& maxs) noexcept { init(start, end, mins, maxs); }

	void init(const Vector3D_F& start, const Vector3D_F& end) noexcept
	{
		init(VectorXD{ start[0], start[1], start[2], 0.f },
			VectorXD{ end[0], end[1], end[2], 0.f }
		);
	}

	void init(const Vector4D_F& start, const Vector4D_F& end) noexcept
	{
		Delta = end - start;

		IsSwept = (Delta.length_sqr() != 0);

		Extents = 0.f;
		IsRay = true;

		StartOffset = 0.f;
		Start = start;
	}

	void init(const Vector3D_F& start, const Vector3D_F& end, const Vector3D_F& mins, const Vector3D_F& maxs) noexcept
	{
		init(VectorXD{ start[0], start[1], start[2], 0.f }, 
			VectorXD{ end[0], end[1], end[2], 0.f },
			VectorXD{ mins[0], mins[1], mins[2], 0.f },
			VectorXD{ maxs[0], maxs[1], maxs[2], 0.f }
		);
	}

	void init(const Vector4D_F& start, const Vector4D_F& end, const Vector4D_F& mins, const Vector4D_F& maxs) noexcept
	{
		VectorXD delta = end - start;
		Delta = end - start;

		IsSwept = (Delta.length_sqr() != 0);

		Extents = (maxs - mins) / 2;
		IsRay = (Extents.length_sqr() < 1e-6);

		VectorXD offset = (maxs + mins) / 2;
		StartOffset = (maxs + mins) / 2;
		Start = offset + StartOffset;
		StartOffset.negate();
	}

	// compute inverse delta
	Vector3D_F invdelta() const noexcept
	{
		Vector3D_F vecInvDelta;
		for (size_t i = 0; i < 3; ++i)
		{
			if (Delta[i] != .0f)
				vecInvDelta[i] = 1.f / Delta[i];
			else
				vecInvDelta[i] = std::numeric_limits<float>::max();
		}
		return vecInvDelta;
	}
};

class IEntityEnumerator
{
public:
	// This gets called with each handle
	virtual bool EnumEntity(IBaseEntityInternal*) abstract;
};

namespace Const
{
	enum class GameTraceType
	{
		Everything = 0,
		// NOTE: This does *not* test static props!!!
		WorldOnly,
		// NOTE: This version will *not* test static props
		EntitiesOnly,
		// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
		EverythingFilterProps,
	};
}

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IClientUnknown* pUnk, uint32_t contentsMask) abstract;

	virtual Const::GameTraceType GetTraceType() const abstract;
};


TF2_NAMESPACE_END();
