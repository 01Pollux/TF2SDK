#pragma once

#include <tf2/entity/BaseEntity.hpp>
#include <tf2/studio/BSP.hpp>

#include <tf2/math/Vector.hpp>
#include <tf2/utils/UtlVector.hpp>

#include "GameEnumerator.hpp"

TF2_NAMESPACE_BEGIN();

class IBaseEntityInternal;
class IPhysCollide;

namespace Const
{
	// Note: These flags need to match the bspfile.h DISPTRI_TAG_* flags.
	namespace DispSurfFlags
	{
		static constexpr uint32_t Surface	= 1 << 0;
		static constexpr uint32_t Walkable	= 1 << 1;
		static constexpr uint32_t Buildable = 1 << 2;
		static constexpr uint32_t SurProp1	= 1 << 3;
		static constexpr uint32_t SurProp2	= 1 << 4;
	}
}


class GameTrace
{
public:
	// Returns true if hEnt points at the world entity.
	// If this returns true, then you can't use GetHitBoxIndex().
	bool DidHitWorld() const;

	// Returns true if we hit something and it wasn't the world.
	bool DidHitNonWorldEntity() const;

	// Returns true if there was any kind of impact at all
	bool DidHit() const;

	bool DidHit(const IBaseEntityInternal*) const;

	// Displacement flags tests.
	bool IsDispSurface() const noexcept { return ((DispFlags & Const::DispSurfFlags::Surface) != 0); }
	bool IsDispSurfaceWalkable() const noexcept { return ((DispFlags & Const::DispSurfFlags::Walkable) != 0); }
	bool IsDispSurfaceBuildable() const noexcept { return ((DispFlags & Const::DispSurfFlags::Buildable) != 0); }
	bool IsDispSurfaceProp1() const noexcept { return ((DispFlags & Const::DispSurfFlags::SurProp1) != 0); }
	bool IsDispSurfaceProp2() const noexcept { return ((DispFlags & Const::DispSurfFlags::SurProp2) != 0); }

public:

	// these members are aligned!!
	Vector3D_F		StartPos;				// start position
	Vector3D_F		EndPos;					// final position
	cplane_t		Plane;					// surface normal at impact

	float			Fraction;				// time completed, 1.0 = didn't hit anything

	int				Contents;				// contents on other side of surface hit
	unsigned short	DispFlags;				// displacement flags for marking surfaces with data

	bool			AllSolid;				// if true, plane is not valid
	bool			StartSolid;				// if true, the initial point was in a solid area

	float		Fractionleftsolid;		// time we left a solid, only valid if we started in solid
	csurface_t	Surface;				// surface hit (impact surface)

	int			HitGroup;				// 0 == generic, non-zero is specific body part
	short		PhysicsBone;			// physics bone hit by trace in studio

	IBaseEntityInternal* Entity;

	// NOTE: this member is overloaded.
	// If hEnt points at the world entity, then this is the static prop index.
	// Otherwise, this is the hitbox index.
	int			Hitbox;					// box hit by trace in studio

	GameTrace() = default;
	GameTrace(const GameTrace&) = delete;
	GameTrace& operator=(const GameTrace&) = delete;
};

inline bool GameTrace::DidHit() const
{
	return Fraction < 1.0f || AllSolid || StartSolid;
}

inline bool GameTrace::DidHit(const IBaseEntityInternal* pEnt) const
{
	return Fraction != 1.0f ? Entity == pEnt : true;
}

inline bool GameTrace::DidHitWorld() const
{
	return Entity->GetEntIndex() == 0;
}

inline bool GameTrace::DidHitNonWorldEntity() const
{
	return Entity && !DidHitWorld();
}



constexpr size_t TLD_DEF_LEAF_MAX = 256;
constexpr size_t TLD_DEF_ENTITY_MAX = 1024;

class ITraceListData : public IPartitionEnumerator
{
public:
	ITraceListData(int nLeafMax = TLD_DEF_LEAF_MAX, int nEntityMax = TLD_DEF_ENTITY_MAX) noexcept
	{
		LeafList.resize(nLeafMax);
		EntityList.resize(nEntityMax);
	}

	void Reset() noexcept
	{
		LeafCount = 0;
		EntityCount = 0;
	}

	bool	IsEmpty() const noexcept { return (LeafCount == 0 && EntityCount == 0); }

	int		LeafCountMax() const noexcept { return LeafList.size(); }
	void    LeafCountReset() noexcept { LeafCount = 0; }

	int		EntityCountMax() const noexcept { return EntityList.size(); }
	void	EntityCountReset() noexcept { EntityCount = 0; }

	// For leaves...
	void AddLeaf(int iLeaf) noexcept
	{
		if (LeafCount >= LeafList.size())
			LeafList.push_to_tail_multiple(LeafList.size());

		LeafList[LeafCount] = iLeaf;
		LeafCount++;
	}

	// For entities...
	Const::PartitionRetVal EnumElement(IBaseEntityInternal* pHandleEntity) override
	{
		if (EntityCount >= EntityList.size())
			EntityList.push_to_tail_multiple(EntityList.size());

		EntityList[EntityCount] = pHandleEntity;
		EntityCount++;

		return Const::PartitionRetVal::Continue;
	}

public:

	uint32_t					LeafCount{ };
	Utils::UtlVector<int>		LeafList;

	uint32_t								EntityCount{ };
	Utils::UtlVector<IBaseEntityInternal*>	EntityList;
};


namespace Const
{
	namespace ContentsFlags
	{
		// No contents
		static constexpr uint32_t Empty				= 0;

		// an eye is never valid in a solid
		static constexpr uint32_t Solid				= 0x1;
		// translucent, but not watery (glass)
		static constexpr uint32_t Window			= 0x2;
		static constexpr uint32_t Any 				= 0x4;
		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
		static constexpr uint32_t Grate				= 0x8;
		static constexpr uint32_t Slime				= 0x10;
		static constexpr uint32_t Water				= 0x20;
		// block AI line of sight
		static constexpr uint32_t BlockLos			= 0x40;
		// things that cannot be seen through (may be non-solid though)
		static constexpr uint32_t Opaque			= 0x80;

		static constexpr uint32_t LastVisible		= Opaque;
		static constexpr uint32_t AllVisible		= LastVisible | (LastVisible - 1);

		static constexpr uint32_t TestFogVolume		= 0x100;
		// per team contents used to differentiate collisions 
		static constexpr uint32_t Team1				= 0x800;
		// between players and objects on different teams
		static constexpr uint32_t Team2				= 0x1000;
		// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
		static constexpr uint32_t IgnoreNoDrawOpaque= 0x2000;
		// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
		static constexpr uint32_t Moveable			= 0x4000;
		// remaining contents are non-visible, and don't eat brushes
		static constexpr uint32_t AreaPortal		= 0x8000;
		static constexpr uint32_t PlayerClip		= 0x10000;
		static constexpr uint32_t MonsterClip		= 0x20000;

		// currents can be added to any other contents, and may be mixed
		static constexpr uint32_t Current0			= 0x40000;
		static constexpr uint32_t Current90			= 0x80000;
		static constexpr uint32_t Current180		= 0x100000;
		static constexpr uint32_t Current270		= 0x200000;
		static constexpr uint32_t CurrentUp			= 0x400000;
		static constexpr uint32_t CurrentDown		= 0x800000;

		// removed before bsping an entity
		static constexpr uint32_t Origin			= 0x1000000;

		// should never be on a brush, only in game
		static constexpr uint32_t Monster			= 0x2000000;
		static constexpr uint32_t Debris			= 0x4000000;
		// brushes to be added after vis leafs
		static constexpr uint32_t Detail			= 0x8000000;
		// auto set if any surface has trans
		static constexpr uint32_t Translucent		= 0x10000000;
		static constexpr uint32_t Ladder			= 0x20000000;
		// use accurate hitboxes on trace
		static constexpr uint32_t Hitbox			= 0x40000000;
	}

	// NOTE: These are stored in a short in the engine now.  Don't use more than 16 bits
	namespace SurfaceFlags
	{
		// value will hold the light strength
		static constexpr uint32_t Light			= 0x0001;
		// don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
		static constexpr uint32_t Sky2D			= 0x0002;
		// don't draw, but add to skybox
		static constexpr uint32_t Sky			= 0x0004;
		// turbulent water warp
		static constexpr uint32_t Warp			= 0x0008;
		static constexpr uint32_t Trans			= 0x0010;
		// the surface can not have a portal placed on it
		static constexpr uint32_t NoPortal		= 0x0020;
		// FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
		static constexpr uint32_t Trigger		= 0x0040;
		// don't bother referencing the texture
		static constexpr uint32_t NoDraw		= 0x0080;
		// make a primary bsp splitter
		static constexpr uint32_t Hint			= 0x0100;

		// completely ignore, allowing non-closed brushes
		static constexpr uint32_t Skip			= 0x0200;
		// Don't calculate light
		static constexpr uint32_t NoLight		= 0x0400;
		// calculate three lightmaps for the surface for bumpmapping
		static constexpr uint32_t BumpLight		= 0x0800;
		// Don't receive shadows
		static constexpr uint32_t NoShadows		= 0x1000;
		// Don't receive decals
		static constexpr uint32_t NoDecals		= 0x2000;
		// Don't subdivide patches on this surface 
		static constexpr uint32_t NoChop		= 0x4000;
		// surface is part of a hitbox
		static constexpr uint32_t Hitbox		= 0x8000;
	}

	namespace TraceMask
	{
		static constexpr uint32_t All			= 0xFFFFFFFF;
		// everything that is normally solid
		static constexpr uint32_t Solid =
			ContentsFlags::Solid |
			ContentsFlags::Moveable |
			ContentsFlags::Window |
			ContentsFlags::Monster |
			ContentsFlags::Grate;
		// everything that blocks player movement
		static constexpr uint32_t PlayerSolid =
			ContentsFlags::PlayerClip |
			Solid;
		// blocks npc movement
		static constexpr uint32_t NpcSolid =
			ContentsFlags::MonsterClip |
			Solid;
		// water physics in these contents
		static constexpr uint32_t Water =
			ContentsFlags::Water |
			ContentsFlags::Moveable |
			ContentsFlags::Slime;
		// everything that blocks lighting
		static constexpr uint32_t Opaque =
			ContentsFlags::Solid |
			ContentsFlags::Moveable |
			ContentsFlags::Opaque;
		// everything that blocks lighting, but with monsters added.
		static constexpr uint32_t Opaque_And_Npc =
			Opaque |
			ContentsFlags::Monster;
		// everything that blocks line of sight for AI
		static constexpr uint32_t BlockLos =
			ContentsFlags::Solid |
			ContentsFlags::Moveable |
			ContentsFlags::BlockLos;
		// everything that blocks line of sight for AI plus NPCs
		static constexpr uint32_t BlockLos_And_Npc =
			BlockLos |
			ContentsFlags::Monster;
		// everything that blocks line of sight for players
		static constexpr uint32_t Visible =
			Opaque |
			ContentsFlags::IgnoreNoDrawOpaque;
		// everything that blocks line of sight for players, but with monsters added.
		static constexpr uint32_t Visible_And_Npc =
			Opaque_And_Npc |
			ContentsFlags::IgnoreNoDrawOpaque;
		// bullets see these as solid
		static constexpr uint32_t Shot =
			ContentsFlags::Solid |
			ContentsFlags::Moveable |
			ContentsFlags::Monster |
			ContentsFlags::Window |
			ContentsFlags::Debris |
			ContentsFlags::Hitbox;
		// non-raycasted weapons see this as solid (includes grates)
		static constexpr uint32_t Shot_Hull =
			Solid |
			ContentsFlags::Debris;
		// hits solids (not grates) and passes through everything else
		static constexpr uint32_t Shot_Portal =
			ContentsFlags::Solid |
			ContentsFlags::Moveable |
			ContentsFlags::Window |
			ContentsFlags::Monster;
		// everything normally solid, except monsters (world+brush only)
		static constexpr uint32_t Shot_BrushOnly =
			ContentsFlags::Solid |
			ContentsFlags::Moveable |
			ContentsFlags::Window |
			ContentsFlags::Grate;
		// everything normally solid for player movement, except monsters (world+brush only)
		static constexpr uint32_t PlayerSolid_BrushOnly =
			ContentsFlags::Solid |
			ContentsFlags::Moveable |
			ContentsFlags::Window |
			ContentsFlags::PlayerClip |
			ContentsFlags::Grate;
		// everything normally solid for npc movement, except monsters (world+brush only)
		static constexpr uint32_t NpcSolid_BrushOnly =
			ContentsFlags::Solid |
			ContentsFlags::Moveable |
			ContentsFlags::Window |
			ContentsFlags::MonsterClip |
			ContentsFlags::Grate;
		// just the world, used for route rebuilding
		static constexpr uint32_t NpcWorldStatic =
			ContentsFlags::Solid |
			ContentsFlags::Window |
			ContentsFlags::MonsterClip |
			ContentsFlags::Grate;
		// These are things that can split areaportals
		static constexpr uint32_t Split_AreaPortal =
			ContentsFlags::Water |
			ContentsFlags::Slime;
		// UNDONE: This is untested, any moving water
		static constexpr uint32_t Current =
			ContentsFlags::Current0 |
			ContentsFlags::Current90 |
			ContentsFlags::Current180 |
			ContentsFlags::Current270 |
			ContentsFlags::CurrentUp |
			ContentsFlags::CurrentDown;
		// everything that blocks corpse movement
		// UNDONE: Not used yet / may be deleted
		static constexpr uint32_t DeadSolid =
			ContentsFlags::Solid |
			ContentsFlags::PlayerClip |
			ContentsFlags::Window |
			ContentsFlags::Grate;
	}
}

TF2_NAMESPACE_END();
