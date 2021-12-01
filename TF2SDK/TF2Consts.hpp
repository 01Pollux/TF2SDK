#pragma once

#include "TF2Config.hpp"


TF2_NAMESPACE_BEGIN(::Const);

// the message a server sends when a clients steam login is expired
#define INVALID_STEAM_TICKET				"Invalid STEAM UserID Ticket"
#define INVALID_STEAM_LOGON					"No Steam logon"
#define INVALID_STEAM_VACBANSTATE			"VAC banned from secure server"
#define INVALID_STEAM_LOGGED_IN_ELSEWHERE	"This Steam account is being used in another location"


// This is the default, see shareddefs.h for mod-specific value, which can override this
static constexpr float DefaultTick	= 0.015f;
static constexpr float MinTick		= 0.001f;
static constexpr float MaxTick		= 0.1f;


// This is the max # of players the engine can handle
static constexpr size_t MaxPlayers_Absolute = 255u;
static constexpr size_t MaxPlayers_AbsoluteDw = (MaxPlayers_Absolute / 32) + 1;


static constexpr size_t MaxPlayers = 34u;
// a player name may have 31 chars + 0 on the PC.
// the 360 only allows 15 char + 0, but stick with the larger PC size for cross-platform communication
static constexpr size_t MaxPlayer_Name = 32u;


static constexpr size_t MaxWeapons = 48u;
static constexpr size_t MaxWeapons_Slot = 6u;
static constexpr size_t MaxWeapons_Position = 20u;
static constexpr size_t MaxItem_Types = 6u;


static constexpr size_t MaxMap_Name = 32u;
// num chars for a network (i.e steam) ID
static constexpr size_t MaxNetwordId_Size = 64u;


// BUGBUG: Reconcile with or derive this from the engine's internal definition!
// FIXME: I added an extra bit because I needed to make it signed
static constexpr size_t ModelIndex_Bits = 11u;


// How many bits to use to encode an edict.
static constexpr size_t MaxEdicts_Bits = 11u;
// Max # of edicts in a level
static constexpr size_t MaxEdicts = 1 << MaxEdicts_Bits;
// Max # of entities in a level
static constexpr size_t MaxEntities = MaxEdicts;


// How many bits to use to encode an server class index
static constexpr size_t MaxServerClasses_Bits = 9;
// Max # of networkable server classes
static constexpr size_t MaxServerClass = 1 << MaxServerClasses_Bits;


// Hashed CD Key (32 hex alphabetic chars + 0 terminator )
static constexpr size_t MaxPlayerGUID_Size = 32u;


// Used for networking ehandles.
static constexpr size_t EHandle_EntryBits	= MaxEdicts_Bits + 1;
static constexpr size_t EHandle_EntrySize	= 1 << EHandle_EntryBits;
static constexpr size_t EHandle_EntryMask	= EHandle_EntrySize - 1;
static constexpr int EHandle_EntryInvalid	= -1;

// Networked ehandles use less bits to encode the serial number.
static constexpr size_t EHandle_SerialBits_Size			= 32u - EHandle_EntryBits;
static constexpr size_t EHandle_SerialBits				= 10u;
static constexpr size_t EHandle_NumNeworked_SerialBits	= MaxEdicts_Bits + EHandle_SerialBits;
static constexpr size_t EHandle_Bits					= (1 << EHandle_NumNeworked_SerialBits) - 1;



// This is the maximum amount of data a PackedEntity can have. Having a limit allows us
// to use static arrays sometimes instead of allocating memory all over the place.
static constexpr size_t MaxPacked_EntityData = 1 << 14;

// This is the maximum number of properties that can be delta'd. Must be evenly divisible by 8.
static constexpr size_t MaxPacked_EntityProps = 1 << 12;

// a client can have up to 4 customization files (logo, sounds, models, txt).
static constexpr size_t CustomFiles_Count = 4u;
static constexpr size_t CustomFiles_Size = (1 << 15) * CustomFiles_Count;


static constexpr int MaxWorldCoord = 1 << 14;
static constexpr int MinWorldCoord = -MaxWorldCoord;

static constexpr float MaxWorldCoordF = MaxWorldCoord;
static constexpr float MinWorldCoordF = MinWorldCoord;

static constexpr double MaxWorldCoord_Fraction = 1.0 - (1.0 / 16.0);
static constexpr double MinWorldCoord_Fraction = -MaxWorldCoord_Fraction;

// Width of the coord system, which is TOO BIG to send as a client/server coordinate value
static constexpr size_t WorldCoord_Extent = 2 * MaxWorldCoord;

// Maximum traceable distance ( assumes cubic world and trace from one corner to opposite )
// COORD_EXTENT * sqrt(3)
static constexpr double MaxTrace_Length = 1.732050807569 * WorldCoord_Extent;

// This value is the LONGEST possible range (limited by max valid coordinate number, not 2x)
static constexpr size_t WorldCoord_Range = MaxWorldCoord;


// Constants shared by the engine and dlls
// This header file included by engine files and DLL files.
// Most came from server.h

// CBaseEntity::m_fFlags
// PLAYER SPECIFIC FLAGS FIRST BECAUSE WE USE ONLY A FEW BITS OF NETWORK PRECISION
namespace EntFlags
{
	// At rest / on the ground
	static constexpr uint32_t OnGround		= 1 << 0;
	// Player flag -- Player is fully crouched
	static constexpr uint32_t Ducking		= 1 << 1;
	// player jumping out of water
	static constexpr uint32_t WaterJump		= 1 << 3;
	// Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
	static constexpr uint32_t OnTrain		= 1 << 4;
	// Indicates the entity is standing in rain
	static constexpr uint32_t InRain		= 1 << 5;
	// Player is frozen for 3rd person camera
	static constexpr uint32_t Frozen		= 1 << 6;
	// Player can't move, but keeps key inputs for controlling another entity
	static constexpr uint32_t AtControls	= 1 << 7;
	// Is a player
	static constexpr uint32_t Client		= 1 << 8;
	// Fake client, simulated server side; don't send network messages to them
	static constexpr uint32_t FakeClient	= 1 << 9;
	// In water
	static constexpr uint32_t InWater		= 1 << 10;
	// NOTE if you move things up, make sure to change this value
	static constexpr uint32_t PlayerFlags_Bits = 11;
	

	// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
	// Changes the SV_Movestep() behavior to not need to be on ground
	static constexpr uint32_t Fly			= 1 << 11;
	// Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
	static constexpr uint32_t Swim			= 1 << 12;
	static constexpr uint32_t Conveyor		= 1 << 13;
	static constexpr uint32_t Npc			= 1 << 14;
	static constexpr uint32_t Godmode		= 1 << 15;
	static constexpr uint32_t NoTarget		= 1 << 16;
	// set if the crosshair needs to aim onto the entity
	static constexpr uint32_t AimTarget		= 1 << 17;
	// not all corners are valid
	static constexpr uint32_t PartialGround	= 1 << 18;
	// Eetsa static prop!		
	static constexpr uint32_t StaticProp	= 1 << 19;
	// worldgraph has this ent listed as something that blocks a connection
	static constexpr uint32_t Graphed		= 1 << 20;
	static constexpr uint32_t Grenade		= 1 << 21;
	// Changes the SV_Movestep() behavior to not do any processing
	static constexpr uint32_t StepMovement	= 1 << 22;
	// Doesn't generate touch functions, generates Untouch() for anything it was touching when this flag was set
	static constexpr uint32_t DontTouch		= 1 << 23;
	// Base velocity has been applied this frame (used to convert base velocity into momentum)
	static constexpr uint32_t BaseVelocity	= 1 << 24;
	// Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
	static constexpr uint32_t WorldBrush	= 1 << 25;
	// Terrible name. This is an object that NPCs should see. Missiles, for example.
	static constexpr uint32_t Object		= 1 << 26;
	// This entity is marked for death -- will be freed by game DLL
	static constexpr uint32_t KillMe		= 1 << 27;
	// You know...
	static constexpr uint32_t OnFire		= 1 << 28;
	// We're dissolving!
	static constexpr uint32_t Dissolving	= 1 << 29;
	// In the process of turning into a client side ragdoll.
	static constexpr uint32_t TransRagdoll	= 1 << 30;
	// pusher that can't be blocked by the player
	static constexpr uint32_t Unblockable	= 1 << 31;
}


namespace DmgInfo
{
	// generic damage was done
	static constexpr uint32_t Generic = 0;
	// crushed by falling or moving object.
	// NOTE: It's assumed crush damage is occurring as a result of physics collision,
	// so no extra physics force is generated by crush damage.
	// DON'T use DMG_CRUSH when damaging entities unless it's the result of a physics
	// collision.You probably want DMG_CLUB instead
	static constexpr uint32_t Crush			= 1 << 0;
	// shot
	static constexpr uint32_t Bullet		= 1 << 1;
	// cut, clawed, stabbed
	static constexpr uint32_t Slash			= 1 << 2;
	// heat burned
	static constexpr uint32_t Burn			= 1 << 3;
	// hit by a vehicle
	static constexpr uint32_t Vehicle		= 1 << 4;
	// fell too far
	static constexpr uint32_t Fall			= 1 << 5;
	// explosive blast damage
	static constexpr uint32_t Blast			= 1 << 6;
	// crowbar, punch, headbutt
	static constexpr uint32_t Club			= 1 << 7;
	// electric shock
	static constexpr uint32_t Shock			= 1 << 8;
	// sound pulse shockwave
	static constexpr uint32_t Sonic			= 1 << 9;
	// laser or other high energy beam
	static constexpr uint32_t EnergyBeam	= 1 << 10;
	// Prevent a physics force
	static constexpr uint32_t NoPhysForce	= 1 << 11;
	// with this bit OR'd in, no damage type will be able to gib victims upon death
	static constexpr uint32_t NoGib			= 1 << 12;
	// with this bit OR'd in, any damage type can be made to gib victims upon death
	static constexpr uint32_t AlwaysGib		= 1 << 13;
	// Drowning
	static constexpr uint32_t Drown			= 1 << 14;
	// slows affected creature down
	static constexpr uint32_t Paralyze		= 1 << 15;
	 // nerve toxins, very bad
	static constexpr uint32_t NerveGas		= 1 << 16;
	// blood poisoning - heals over time like drowning damage
	static constexpr uint32_t Poison		= 1 << 17;
	// radiation exposure
	static constexpr uint32_t Radiation		= 1 << 18;
	// drowning recovery
	static constexpr uint32_t DrownRecover	= 1 << 19;
	// toxic chemicals or acid burns
	static constexpr uint32_t Acid			= 1 << 20;
	// in an oven
	static constexpr uint32_t SlowBurn		= 1 << 21;
	// with this bit OR'd in, no ragdoll will be created, and the target will be quietly removed.
	// use this to kill an entity that you've already got a server-side ragdoll for
	static constexpr uint32_t NoRagdoll		= 1 << 22;
	// Hit by manipulator. Usually doesn't do any damage
	static constexpr uint32_t PhysGun		= 1 << 23;
	// Shot by Cremator
	static constexpr uint32_t Plasma		= 1 << 24;
	// Hit by the airboat's gun
	static constexpr uint32_t AirBoat		= 1 << 25;
	// Dissolving!
	static constexpr uint32_t Dissolve		= 1 << 26;
	// A blast on the surface of water that cannot harm things underwater
	static constexpr uint32_t BlastSurface	= 1 << 27;
	static constexpr uint32_t Direct		= 1 << 28;
	// not quite a bullet. Little, rounder, different
	static constexpr uint32_t BuckShot		= 1 << 29;

	// TF2 crits and minicrits
	static constexpr uint32_t Crit					= Acid;
	// No damage falloff
	static constexpr uint32_t MaxRadius				= EnergyBeam;
	// 't do damage falloff too close
	static constexpr uint32_t NoClosedDistanceMod	= Poison;
	// 50% damage falloff
	static constexpr uint32_t HalfFallOff			= Radiation;
	// Do damage falloff
	static constexpr uint32_t UseDistanceMod		= SlowBurn;
	// Ignite victim
	static constexpr uint32_t Ignite				= Plasma;
	// hit location damage (Like the sniperrifle and ambassador)
	static constexpr uint32_t UseHitLocation		= AirBoat;
}


// edict->movetype values
enum class EntMoveType : uint8_t
{
	// never moves
	None,
	// For players -- in TF2 commander view, etc.
	Isometric,
	// Player only - moving on the ground
	Walk,
	// gravity, special edge handling -- monsters use this
	Step,
	// No gravity, but still collides with stuff
	Fly,
	// flies through the air + is affected by gravity
	FlyGravity,
	// uses VPHYSICS for simulation
	VPhysics,
	// no clip to world, push and crush
	Push,
	// No gravity, no collisions, still do velocity/avelocity
	NoClip,
	// Used by players only when going onto a ladder
	Ladder,
	// Observer movement, depends on player's observer mode
	Observer,
	// Allows the entity to describe its own physics
	Custom,

	Last = Custom,

	MaxBits = 4
};


// edict->movecollide values
enum class EntMoveCollide
{
	Default = 0,

	// These ones only work for EntMoveType::Fly + EntMoveType::FlyGravity
	// bounces, reflects, based on elasticity of surface and object - applies friction (adjust velocity)
	FlyBounce,
	// Touch() will modify the velocity however it likes
	FlyCustom,
	// slides along surfaces (no bounce) - applies friciton (adjusts velocity)
	FlySlide,

	// Number of different movecollides
	Count,

	// When adding new movecollide types, make sure this is correct
	MaxBits = 3
};

// edict->solid values
// NOTE: Some movetypes will cause collisions independent of SOLID_NOT/SOLID_TRIGGER when the entity moves
// SOLID only effects OTHER entities colliding with this one when they move - UGH!

// Solid type basically describes how the bounding volume of the object is represented
// NOTE: SOLID_BBOX MUST BE 2, and SOLID_VPHYSICS MUST BE 6
// NOTE: These numerical values are used in the FGD by the prop code (see prop_dynamic)
enum class EntSolidType
{
	// no solid model
	None,
	// a BSP tree
	BSP,
	// an AABB
	BBOX,
	// an OBB (not implemented yet)
	OBB,
	// an OBB, constrained so that it can only yaw
	OBB_Yaw,
	// Always call into the entity for tests
	Custom,
	// solid vphysics object, get vcollide from the model and collide with that
	VPhysics,
	
	Count,
};


namespace EntSolidFlags
{
	// Ignore solid type + always call into the entity for ray tests
	constexpr uint32_t Custom_Ray_Test			= 0x0001;
	// Ignore solid type + always call into the entity for swept box tests
	constexpr uint32_t Custom_Box_Test			= 0x0002;
	// Are we currently not solid?
	constexpr uint32_t Not_Solid				= 0x0004;
	// This is something may be collideable but fires touch functions
	constexpr uint32_t Trigger					= 0x0008;
	// even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
	// You can't stand on this
	constexpr uint32_t Not_Standable			= 0x0010;
	// Contains volumetric contents (like water)
	constexpr uint32_t Volume_Contents			= 0x0020;
	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
	constexpr uint32_t Force_World_Align		= 0x0040;
	// Uses a special trigger bounds separate from the normal OBB
	constexpr uint32_t Use_Trigger_Bounds		= 0x0080;
	// Collisions are defined in root pare	nt's local coordinate space
	constexpr uint32_t Root_Parents_Align		= 0x0100;
	// This trigger will touch debris objects
	constexpr uint32_t Trigger_Touch_Debris		= 0x0200;
};

constexpr bool IsSolidType(EntSolidType solidType, uint32_t solidFlags)
{
	return (solidType != EntSolidType::None) && (solidFlags & ~EntSolidFlags::Not_Solid);
}


// entity effects
namespace EntEffects
{
	// Performs bone merge on client side
	constexpr uint32_t BoneMerge		= 0x001;
	// DLIGHT centered at entity origin
	constexpr uint32_t BrightLight		= 0x002;
	// player flashlight
	constexpr uint32_t DimLight			= 0x004;
	// don't interpolate the next frame
	constexpr uint32_t NoInterp			= 0x008;
	// Don't cast no shadow
	constexpr uint32_t NoShadow			= 0x010;
	// don't draw entity
	constexpr uint32_t NoDraw			= 0x020;
	// Don't receive no shadow
	constexpr uint32_t NoReceiveshadow	= 0x040;
	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
	constexpr uint32_t BoneMerge_FastCull= 0x080;

	// parent and uses the parent's bbox + the max extents of the aiment.
	// Otherwise, it sets up the parent's bones every frame to figure out where to place
	// the aiment, which is inefficient because it'll setup the parent's bones even if
	// the parent is not in the PVS.
	// blink an item so that the user notices it.
	constexpr uint32_t ItemBlink		= 0x100;
	// always assume that the parent entity is animating
	constexpr uint32_t ParentAnimate	= 0x200;

	static constexpr size_t Bits = 3;
	static constexpr size_t Mask = (1 << Bits) - 1;

	// How many bits does the muzzle flash parity thing get?
	static constexpr size_t MuzzleFlash_Bits = 2;
};

// Trains
static constexpr size_t SpawFlags_Train_WaitRetrigger = 1;
// Train is not solid -- used to make water trains
static constexpr size_t SpawFlags_Train_Passable = 8;

// view angle update types for CPlayerState::fixangle
namespace PlayerStateAngle
{
	static constexpr int None = 0;
	static constexpr int Absolute = 1;
	static constexpr int Relative = 2;
}

// Break Model Defines
namespace BreakModel
{
	static constexpr uint32_t Glass		= 1 << 0;
	static constexpr uint32_t Metal		= 1 << 1;
	static constexpr uint32_t Flesh		= 1 << 2;
	static constexpr uint32_t Wood		= 1 << 3;

	static constexpr uint32_t Smoke		= 1 << 4;
	static constexpr uint32_t Trans		= 1 << 5;
	static constexpr uint32_t Concrete	= 1 << 6;

	// If this is set, then we share a lighting origin with the last non-slave breakable sent down to the client
	static constexpr uint32_t Slave		= 1 << 7;
}

// Colliding temp entity sounds
namespace TEBounce
{
	static constexpr uint32_t Glass = BreakModel::Glass;
	static constexpr uint32_t Metal = BreakModel::Metal;
	static constexpr uint32_t Flesh = BreakModel::Flesh;
	static constexpr uint32_t Wood = BreakModel::Wood;

	static constexpr uint32_t Sharp = BreakModel::Smoke;
	static constexpr uint32_t Shell = BreakModel::Trans;
	static constexpr uint32_t Concrete = BreakModel::Concrete;
	static constexpr uint32_t ShotShell = BreakModel::Slave;

	// Temp entity bounce sound types
	static constexpr int Null_Sound = 0;
	static constexpr int Shell_Sound = 1;
	static constexpr int ShotShell_Sound = 2;
}


// Rendering constants
// if this is changed, update common/MaterialSystem/Sprite.cpp
enum class RenderMode
{
	// src
	Normal,
	// c*a+dest*(1-a)
	TransColor,	
	// src*a+dest*(1-a)
	TransTexture,
	// src*a+dest -- No Z buffer checks -- Fixed size in screen space
	Glow,
	// src*srca+dest*(1-srca)
	TransAlpha,
	// src*a+dest
	TransAdd,
	// not drawn, used for environmental effects
	Environmental,
	// use a fractional frame value to blend between animation frames
	TransAddFrameBlend,
	// src + dest*(1-a)
	TransAlphaAdd,
	// Same as kRenderGlow but not fixed size in screen space
	WorldGlow,
	// Don't render.
	None,

	Count
};

enum class RenderFx
{
	None,
	PulseSlow,
	PulseFast,
	PulseSlowWide,
	PulseFastWide,
	FadeSlow,
	FadeFast,
	SolidSlow,
	SolidFast,
	StrobeSlow,
	StrobeFast,
	StrobeFaster,
	FlickerSlow,
	FlickerFast,
	NoDissipation,
	// Distort/scale/translate flicker
	Distort,
	// kRenderFxDistort + distance fade
	Hologram,
	// Scale up really big!
	Explode,
	// Glowing Shell
	GlowShell,
	// Keep this sprite from getting very small (SPRITES only!)
	ClampMinScale,
	// for environmental rendermode, make rain
	EnvRain,
	//  for environmental rendermode, make snow
	EnvSnow,
	// TEST CODE for experimental spotlight
	Spotlight,
	// HACKHACK: TEST CODE for signalling death of a ragdoll character
	Ragdoll,
	PulseFastWider,

	Count
};

enum class EntCollisionGroup
{
	None,
	// Collides with nothing but world and static stuff
	Debris,
	// Same as debris, but hits triggers
	DebrisTirgger,
	// Collides with everything except other interactive debris or debris
	InteractiveDebris,
	// Collides with everything except interactive debris or debris
	Interactive,
	Player,
	BreakableGlass,
	Vehicle,
	// For HL2, same as Collision_Group_Player, for
	PlayerMovements,

	// TF2, this filters out other players and CBaseObjects
	// Generic NPC group
	NPC,
	// for any entity inside a vehicle
	InVehicle,
	// for any weapons that need collision detection
	Weapon,
	// vehicle clip brush to restrict vehicle movement
	VehicleClip,
	// Projectiles!
	Projectile,
	// Blocks entities not permitted to get near moving doors
	DoorBlocket,
	// Doors that the player shouldn't collide with
	PassableDoor,
	// Things that are dissolving are in this group
	Dissolving,
	// Nonsolid on client and server, pushaway in player code
	PushAway,
	// Used so NPCs in scripts ignore the player.
	NPCActor,
	// Used for NPCs in scripts that should not collide with each other
	NPCScripted,

	Grenades,

	Object,

	Object_SolidToPlayerMovement,

	CombatObject,

	Rockets,

	Respawn_Rooms,
	
	Tank,

	Rocket_But_Not_With_Other_Rockets,

	Count
};

static constexpr float SoundNormal_ClipDist = 1000.f;

// How many networked area portals do we allow?
static constexpr size_t MaxArea_StateBytes = 32u;
static constexpr size_t MaxAreaPortal_StateBytes = 24u;

// user message max payload size (note, this value is used by the engine, so MODs cannot change it)
static constexpr size_t MaxUserMsgData= 255u;
static constexpr size_t MaxEntityMsgData = 255u;

using QueryCvarCookie = int;
static constexpr QueryCvarCookie QueryCvarCookie_Invalid = -1;


enum class TFTeam
{
	Unknown,
	Spectator,
	Red,
	Blue,

	Count
};

namespace TFTeamNames
{
	constexpr const char* CStrings[]{
		"Spectator",
		"Red",
		"Blue"
	};

	constexpr const wchar_t* WStrings[]{
		L"Spectator",
		L"Red",
		L"Blue"
	};
}

enum class PlayerHitboxIdx
{
	Invalid = -1,
	Head,
	Pelvis,
	Spine_0,
	Spine_1,
	Spine_2,
	Spine_3,
	UpperArm_L,
	LowerArm_L,
	Hand_L,
	UpperArm_R,
	LowerArm_R,
	Hand_R,
	Hip_L,
	Knee_L,
	Foot_L,
	Hip_R,
	Knee_R,
	Foot_R
};

TF2_NAMESPACE_END();