#pragma once

#include "BaseEntity.hpp"
#include "Utils/Vector.hpp"


TF2_NAMESPACE_BEGIN();

namespace Const
{
	enum class PlayerObserverMode
	{
		// not in spectator mode
		None = 0,
		// special mode for death cam animation
		DeathCam,
		// zooms to a target, and freeze-frames on them
		FreezeCam,	
		// view from a fixed camera position
		Fixed,
		// follow a player in first person view
		InEye,
		// follow a player in third person view
		Chase,
		// PASSTIME point of interest - game objective, big fight, anything interesting; added in the middle of the enum due to tons of hard-coded "<ROAMING" enum compares
		PointOfInterest,
		// free roaming
		FreeCam,

		Count,
	};

	enum class ETFCond
	{
		//0: Revving Minigun, Sniper Rifle.Gives zoomed / revved pose
		Slowed,
		//1: Sniper Rifle zooming
		Zoomed,
		//2: Disguise smoke
		Disguising,
		//3: Disguise
		Disguised,
		//4: Cloak effect
		Cloaked,
		//5: Invulnerability, removed when being healed or by another Uber effect
		Ubercharged,
		//6: Teleport trail effect
		TeleportedGlow,
		//7: Used for taunting, can remove to stop taunting
		Taunting,
		//8: Invulnerability expiration effect
		UberchargeFading,
		//9: Cloak flickering effect
		CloakFlicker,
		//10: Used for teleporting, does nothing applying 
		Teleporting,
		//11: Crit boost, removed when being healed or another Uber effect
		Kritzkrieged,
		//12: Temporary damage buff, something along with attribute 19
		TmpDamageBonus,
		//13: Dead Ringer damage resistance, gives TFCond_Cloaked
		DeadRingered,
		//14: Bonk! Atomic Punch effect
		Bonked,
		//15: Slow effect, can remove to remove stun effects
		Dazed,
		//16: Buff Banner mini-crits, icon, and glow
		Buffed,
		//17: Forced forward, charge effect
		Charging,
		//18: Eyelander eye glow
		DemoBuff,
		//19: Mini-crit effect
		CritCola,
		//20: Ring effect, rings disappear after a taunt ends
		InHealRadius,
		//21: Used for healing, does nothing applying
		Healing,
		//22: Ignite sound and vocals, can remove to remove afterburn
		OnFire,
		//23: Used for overheal, does nothing applying
		Overhealed,
		//24: Jarate effect
		Jarated,
		//25: Bleed effect
		Bleeding,
		//26: Battalion's Backup's defense, icon, and glow
		DefenseBuffed,
		//27: Mad Milk effect
		Milked,
		//28: Quick-Fix Ubercharge's knockback/stun immunity and visual effect
		MegaHeal,
		//29: Concheror's speed boost, heal on hit, icon, and glow
		RegenBuffed,
		//30: Fan o' War marked-for-death effect
		MarkedForDeath,
		//31: Mini-crits, blocks healing, glow, no weapon mini - crit effects
		NoHealingDamageBuff,
		//32: Disciplinary Action speed boost
		SpeedBuffAlly,
		//33: Halloween pumpkin crit-boost
		HalloweenCritCandy,
		//34: Crit-boost and doubles Sentry Gun fire-rate
		CritCanteen,
		//35: Crit glow, adds TFCond_Charging when charge meter is below 75%
		CritDemoCharge,
		//36: Soda Popper multi-jump effect
		CritHype,
		//37: Arena first blood crit-boost
		CritOnFirstBlood,
		//38: End-of-round crit-boost (May not remove correctly?)
		CritOnWin,
		//39: Intelligence capture crit-boost
		CritOnFlagCapture,
		//40: Crit-boost from crit-on-kill weapons
		CritOnKill,
		//41: Prevents switching once melee is out
		RestrictToMelee,
		//42: MvM Bomb Carrier defense buff (TFCond_DefenseBuffed without crit resistance)
		DefenseBuffNoCritBlock,
		//43: No longer functions
		Reprogrammed,
		//44: Phlogistinator crit-boost
		CritMmmph,
		//45: Old Phlogistinator defense buff
		DefenseBuffMmmph,
		//46: Hitman's Heatmaker no-unscope and faster Sniper charge
		FocusBuff,
		//47: Enforcer damage bonus removed
		DisguiseRemoved,
		//48: Marked-for-death without sound effect
		MarkedForDeathSilent,
		//49: Dispenser disguise when crouching, max movement speed, sentries ignore player
		DisguisedAsDispenser,
		//50: Sapper sparkle effect in MvM
		Sapped,
		//51: Out-of-bounds robot invulnerability effect
		UberchargedHidden,
		//52: Invulnerability effect and Sentry Gun damage resistance
		UberchargedCanteen,
		//53: Bomb head effect (does not explode)
		HalloweenBombHead,
		//54: Forced Thriller taunting
		HalloweenThriller,
		//55: Radius healing, adds TFCond_InHealRadius, TFCond_Healing.Removed when a taunt ends, but this condition stays but does nothing
		RadiusHealOnDamage,
		//56: Miscellaneous crit-boost
		CritOnDamage,
		//57: Miscellaneous invulnerability
		UberchargedOnTakeDamage,
		//58: Vaccinator Uber bullet resistance
		UberBulletResist,
		//59: Vaccinator Uber blast resistance
		UberBlastResist,
		//60: Vaccinator Uber fire resistance
		UberFireResist,
		//61: Vaccinator healing bullet resistance
		SmallBulletResist,
		//62: Vaccinator healing blast resistance
		SmallBlastResist,
		//63: Vaccinator healing fire resistance
		SmallFireResist,
		//64: Cloaked until next attack
		Stealthed,
		//65: Unknown
		MedigunDebuff,
		//66: Cloaked, will appear for a few seconds on attackand cloak again
		StealthedUserBuffFade,
		//67: Full bullet immunity
		BulletImmune,
		//68: Full blast immunity
		BlastImmune,
		//69: Full fire immunity
		FireImmune,
		//70: Survive to 1 health, then the condition is removed 
		PreventDeath,
		//71: Stuns bots and applies radio effect
		MVMBotRadiowave,
		//72: Speed boost, non - melee fire rate and reload, infinite air jumps 
		HalloweenSpeedBoost,
		//73: Healing effect, adds TFCond_Healing along with TFCond_MegaHeal temporarily
		HalloweenQuickHeal,
		//74: Double size, x10 max health increase, ammo regeneration, and forced thirdperson
		HalloweenGiant,
		//75: Half size and increased head size
		HalloweenTiny,
		//76: Applies TFCond_HalloweenGhostMode when the player dies
		HalloweenInHell,
		//77: Becomes a ghost unable to attack but can fly
		HalloweenGhostMode,
		//78: Mini-crits effect
		MiniCritOnKill,
		//79: 75% chance to dodge an attack
		DodgeChance,
		//79: 75% chance to dodge an attack
		ObscuredSmoke = 79,
		//80: Parachute effect, removed when touching the ground
		Parachute,
		//81: Player is blast jumping
		BlastJumping,
		//82: Player forced into a Halloween kart
		HalloweenKart,
		//83: Forced forward if in TFCond_HalloweenKart, zoom in effect, and dash animations
		HalloweenKartDash,
		//84: Big head and lowered gravity
		BalloonHead,
		//85: Forced melee, along with TFCond_SpeedBuffAllyand TFCond_HalloweenTiny
		MeleeOnly,
		//86: Swim in the air with Jarate overlay
		SwimmingCurse,
		//87: Prevents player from using controls
		HalloweenKartNoTurn,
		//87: Prevents player from using controls
		FreezeInput = 87,
		//88: Puts a cage around the player if in TFCond_HalloweenKart, otherwise crashes
		HalloweenKartCage,
		//89: Has a powerup
		HasRune,
		//90: Double damage and no damage falloff
		RuneStrength,
		//91: Double fire rate, reload speed, clipand ammo size, and 30 % faster movement speed
		RuneHaste,
		//92: Regen ammo, health, and metal
		RuneRegen,
		//93: Takes 1/2 damage and critical immunity
		RuneResist,
		//94: Takes 3/4 damage, gain health on damage, and 40 % increase in max health
		RuneVampire,
		//95: Attacker takes damage and knockback on hitting the player and 50% increase in max health
		RuneWarlock,
		//96: Less bullet spread, no damage falloff, 250 % faster projectiles, and double damage, faster charge, and faster re - zoom for Sniper Rifles
		RunePrecision,
		//97: Increased movement speed, grappling hook speed, jump height,and instant weapon switch
		RuneAgility,
		//98: Used when a player fires their grappling hook, no effect applying or removing
		GrapplingHook,
		//99: Used when a player is pulled by their grappling hook, no effect applying or removing
		GrapplingHookSafeFall,
		//100: Used when a player latches onto a wall, no effect applying or removing
		GrapplingHookLatched,
		//101: Used when a player is hit by attacker's grappling hook
		GrapplingHookBleeding,
		//102: Deadringer afterburn immunity
		AfterburnImmune,
		//103: Melee and grappling hook only, increased max health, knockback immunity, x4 more damage against buildings, and knockbacks a powerup off a victim on hit
		RuneKnockout,
		//104: Prevents gaining a crit-boost or Uber powerups
		RuneImbalance,
		//105: Crit-boost effect
		CritRuneTemp,
		//106: Used when a player intercepts the Jack/Ball
		PasstimeInterception,
		//107: Swimming in the air without animations or overlay
		SwimmingNoEffects,
		//108: Refills max health,short Uber, escaped the underworld message on removal
		EyeaductUnderworld,
		//109: Increased max health and applies TFCond_KingAura
		KingRune,
		//110: Radius health kit stealing, increased max health, TFCond_Plague on touching a victim
		PlagueRune,
		//111: Charge meter passively increasing, when charged activiated causes radius Bonk stun
		SupernovaRune,
		//112: Plague sound effect and message, blocks King powerup health regen
		Plague,
		//113: Increased fire rate, reload speed, and health regen to players in a radius
		KingAura,
		//114: Outline and health meter of teammates (and disguised spies)
		SpawnOutline,
		//115: Used when a player is airblasted
		KnockedIntoAir,
		//116: Unknown
		CompetitiveWinner,
		//117: Unknown
		CompetitiveLoser,
		//118: Healing debuff from Medics and dispensers
		HealingDebuff,
		//119: Marked-for-death effect
		PasstimePenaltyDebuff,
		//120: Prevents taunting and some Grappling Hook actions
		GrappledToPlayer,
		//121: Unknown
		GrappledByPlayer,
		//122: Parachute deployed, prevents reopening it
		ParachuteDeployed,
		//123: Gas Passer effect
		Gas,
		//124: Dragon's Fury afterburn on Pyros
		BurningPyro,
		//125: Thermal Thruster launched effects, prevents reusing
		RocketPack,
		//126: Less ground friction
		LostFooting,
		//127: Reduced air control and friction
		AirCurrent,
		// 128: Used when a player gets teleported to hell
		HalloweenHellHeal,
		// 129: Reduces effects of certain powerups
		PowerupModeDominant,

		Count,
	};

	static const char* TFClassNames[] =
	{
		"Scout",
		"Sniper",
		"Soldier",
		"Demoman",
		"Medic",
		"Heavy",
		"Pyro",
		"Spy",
		"Engineer"
	};

	enum class PlayerLifeState : uint8_t
	{
		Alive,
		Dying,
		Dead,
		Respawnable,
		DiscardBody
	};

	enum class TFClass
	{
		Unknown,

		Scout,
		Sniper,
		Soldier,
		Demoman,
		Medic,
		Heavy,
		Pyro,
		Spy,
		Engineer,

		Count,
	};

	enum class TFStreakType
	{
		Kills,
		KillsAll,
		Ducks,
		DuckLevelUp,

		Count,
	};

}

class TFPlayerStreak
{
	using streak_type = std::underlying_type_t<Const::TFStreakType>;
public:
	int& operator[](Const::TFStreakType i) noexcept { return Streaks[static_cast<streak_type>(i)]; };
	int operator[](Const::TFStreakType i) const noexcept { return Streaks[static_cast<streak_type>(i)]; };

	void set(Const::TFStreakType i, const int val) noexcept { Streaks[static_cast<streak_type>(i)] = val; }
	int get(Const::TFStreakType i) const noexcept { return Streaks[static_cast<streak_type>(i)]; }

private:
	int Streaks[static_cast<streak_type>(Const::TFStreakType::Count)];
};


class ITFPlayerShared;

struct ITFPlayerClass
{
public:
	Const::TFClass Class;

	char ClassIcon[260];
	char CustomModel[260];

	Vector3D_F CustomModelOffset;
	Angle_F CustomModelRotation;

	bool CustomModelRotates;
	bool CustomModelRotationSet;
	bool CustomModelVisibleToSelf;
	bool UseClassAnimation;

	int ClassModelParity;
	//int OldClassModelParity;
};

class ITFPlayerInternal : public IBaseEntityInternal
{
public:
	static ITFPlayerInternal* GetEntity(int index)
	{
		return static_cast<ITFPlayerInternal*>(IBaseEntityInternal::GetEntity(index));
	}
	static ITFPlayerInternal* GetEntity(IBaseHandle hndl)
	{
		return static_cast<ITFPlayerInternal*>(IBaseEntityInternal::GetEntity(hndl));
	}
	static ITFPlayerInternal* GetLocalPlayer()
	{
		return static_cast<ITFPlayerInternal*>(IBaseEntityInternal::GetLocalPlayer());
	}

	static ITFPlayerInternal* FromShared(ITFPlayerShared* pSharedEnt)
	{
		return *(std::bit_cast<ITFPlayerInternal**>(pSharedEnt) + 0x188);
	}

	Vector3D_F EyePosition() const
	{
		return VecOrigin.get() + ViewOffset.get();
	}

	void EyeVectors(Vector3D_F* fwd = nullptr, Vector3D_F* right = nullptr, Vector3D_F* up = nullptr) const
	{
		Utils::AngleVectors(EyeAngles.get(), fwd, right, up);
	}

	SG_SDK_TF2 bool 
		InCond(Const::ETFCond cond) const;

	SG_SDK_TF2 void 
		AddCond(Const::ETFCond cond, float duration = -1.0);

	SG_SDK_TF2 void 
		RemoveCond(Const::ETFCond cond);

	SG_DECL_RECVPROP(ITFPlayerInternal, ITFPlayerShared,		Const::EntClassID::CTFPlayer, "m_Shared",				PlayerShared, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, ITFPlayerClass,			Const::EntClassID::CTFPlayer, "m_PlayerClass",			PlayerClass, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, int,					Const::EntClassID::CTFPlayer, "m_nDisguiseTeam",		DisguiseTeam, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, int,					Const::EntClassID::CTFPlayer, "m_nDisguiseClass",		DisguiseClass, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, int,					Const::EntClassID::CTFPlayer, "m_iDisguiseTargetIndex",	DisguiseTargetIndex, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, int,					Const::EntClassID::CTFPlayer, "m_iDisguiseHealth",		DisguiseHealth, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, IHandleEntity,			Const::EntClassID::CTFPlayer, "m_hDisguiseWeapon",		DisguiseWeaponb, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, float,					Const::EntClassID::CTFPlayer, "m_flMaxspeed",			MaxSpeed, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					Const::EntClassID::CTFPlayer, "m_flCloakMeter",			CloakMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					Const::EntClassID::CTFPlayer, "m_flChargeMeter",		ChargeMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					Const::EntClassID::CTFPlayer, "m_flRageMeter",			RageMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					Const::EntClassID::CTFPlayer, "m_flHypeMeter",			HypeMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					Const::EntClassID::CTFPlayer, "m_flEnergyDrinkMeter",	EnergyDrinkMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, bool,					Const::EntClassID::CTFPlayer, "m_bRageDraining",		RageDraining, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, bool,					Const::EntClassID::CTFPlayer, "m_bFeignDeathReady",		FeignDeathReady, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					Const::EntClassID::CTFPlayer, "m_bShieldEquipped",		IsShieldEquipped, 0);


	SG_DECL_RECVPROP(ITFPlayerInternal, Angle_F,						Const::EntClassID::CTFPlayer, "m_angEyeAngles[0]",		EyeAngles, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Angle_F,						Const::EntClassID::CTFPlayer, "m_vecPunchAngle",		PunchAngle, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Vector3D_F,						Const::EntClassID::CTFPlayer, "m_vecViewOffset[0]",		ViewOffset, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Vector3D_F,						Const::EntClassID::CTFPlayer, "m_vecVelocity[0]",		Velocity, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, IBaseHandle[Const::MaxPlayers], Const::EntClassID::CTFPlayer, "m_hMyWeapons",			MyWeapons, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, IBaseHandle,					Const::EntClassID::CTFPlayer, "m_hActiveWeapon",		ActiveWeapon, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, IBaseHandle,					Const::EntClassID::CTFPlayer, "m_hObserverTarget",		ObserverTarget, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Const::PlayerObserverMode,		Const::EntClassID::CTFPlayer, "m_iObserverMode",		ObserverMode, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, Const::PlayerLifeState,			Const::EntClassID::CTFPlayer, "m_lifeState",			LifeState, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Const::TFClass,					Const::EntClassID::CTFPlayer, "m_iClass",				Class, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, TFPlayerStreak,					Const::EntClassID::CTFPlayer, "m_nStreaks",				Streaks, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, int,							Const::EntClassID::CTFPlayer, "m_iHealth",				CurrentHealth, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,							Const::EntClassID::CTFPlayer, "m_flDeathTime",			DeathTime, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, bool[Const::MaxPlayers],		Const::EntClassID::CTFPlayer, "m_bPlayerDominatingMe",	PlayerDominatingMe, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, bool[Const::MaxPlayers],		Const::EntClassID::CTFPlayer, "m_bPlayerDominated",		PlayerDominated, 0);
};


template<bool IsLocalPlayer>
class IPlayerWrapper : public IBaseEntityWrapper<ITFPlayerInternal>
{
public:
	using IBaseEntityWrapper<ITFPlayerInternal>::IBaseEntityWrapper;

	IPlayerWrapper() noexcept
	{
		if constexpr (IsLocalPlayer)
			set(ITFPlayerInternal::GetLocalPlayer());
	}
};

using ITFPlayer = IPlayerWrapper<false>;
using ILocalPlayer = IPlayerWrapper<true>;


namespace Utils
{
	SG_SDK_TF2 Angle_F 
		GetAimAngle(const Vector3D_F& start, const Vector3D_F& end, bool useLocalPunchAng = false);
}

TF2_NAMESPACE_END();