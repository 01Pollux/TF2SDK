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
		Invalid = -1,

		Aiming,
		Zoomed,
		Disguising,
		Disguised,
		Stealthed,
		Invulnerable,
		Teleported,
		Taunting,
		Invulnerable_wearingoff,
		Stealthed_blink,
		Selected_to_teleport,
		Critboosted,
		Tmpdamagebonus,
		Feign_death,
		Phase,
		Stunned,
		Offensebuff,
		Shield_charge,
		Demo_buff,
		Energy_buff,
		Radiusheal,
		Health_buff,
		Burning,
		Health_overhealed,
		Urine,
		Bleeding,
		Defensebuff,
		Mad_milk,
		Megaheal,
		Regenondamagebuff,
		Markedfordeath,
		Nohealingdamagebuff,
		Speed_boost,
		Critboosted_pumpkin,
		Critboosted_user_buff,
		Critboosted_demo_charge,
		Sodapopper_hype,
		Critboosted_first_blood,
		Critboosted_bonus_time,
		Critboosted_ctf_capture,
		Critboosted_on_kill,
		Cannot_switch_from_melee,
		Defensebuff_no_crit_block,
		Reprogrammed,
		Critboosted_rage_buff,
		Defensebuff_high,
		Snipercharge_rage_buff,
		Disguise_wearingoff,
		Markedfordeath_silent,
		Disguised_as_dispenser,
		Sapped,
		Invulnerable_hide_unless_damaged,
		Invulnerable_user_buff,
		Halloween_bomb_head,
		Halloween_thriller,
		Radiusheal_on_damage,
		Critboosted_card_effect,
		Invulnerable_card_effect,
		Medigun_uber_bullet_resist,
		Medigun_uber_blast_resist,
		Medigun_uber_fire_resist,
		Medigun_small_bullet_resist,
		Medigun_small_blast_resist,
		Medigun_small_fire_resist,
		Stealthed_user_buff,
		Medigun_debuff,
		Stealthed_user_buff_fading,
		Bullet_immune,
		Blast_immune,
		Fire_immune,
		Prevent_death,
		Mvm_bot_stun_radiowave,
		Halloween_speed_boost,
		Halloween_quick_heal,
		Halloween_giant,
		Halloween_tiny,
		Halloween_in_hell,
		Halloween_ghost_mode,
		Minicritboosted_on_kill,
		Obscured_smoke,
		Parachute_active = 8,
		Blastjumping,
		Halloween_kart,
		Halloween_kart_dash,
		Balloon_head,
		Melee_only,
		Swimming_curse,
		Freeze_input,
		Halloween_kart_cage,
		Donotuse_0,
		Rune_strength,
		Rune_haste,
		Rune_regen,
		Rune_resist,
		Rune_vampire,
		Rune_reflect,
		Rune_precision,
		Rune_agility,
		Grapplinghook,
		Grapplinghook_safefall,
		Grapplinghook_latched,
		Grapplinghook_bleeding,
		Afterburn_immune,
		Rune_knockout,
		Rune_imbalance,
		Critboosted_rune_temp,
		Passtime_interception,
		Swimming_no_effects,
		Purgatory,
		Rune_king,
		Rune_plague,
		Rune_supernova,
		Plague,
		King_buffed,
		Team_glows,
		Knocked_into_air,
		Competitive_winner,
		Competitive_loser,
		Healing_debuff,
		Passtime_penalty_debuff,
		Grappled_to_player,
		Grappled_by_player,
		Parachute_deployed,
		Gas,
		Burning_pyro,
		Rocketpack,
		Lost_footing,
		Air_current,
		Halloween_hell_heal,

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

	SG_DECL_RECVPROP(ITFPlayerInternal, ITFPlayerShared,		"CTFPlayer", "m_Shared",				PlayerShared, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, ITFPlayerClass,			"CTFPlayer", "m_PlayerClass",			PlayerClass, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, int,					"CTFPlayer", "m_nDisguiseTeam",			DisguiseTeam, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, int,					"CTFPlayer", "m_nDisguiseClass",		DisguiseClass, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, int,					"CTFPlayer", "m_iDisguiseTargetIndex",	DisguiseTargetIndex, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, int,					"CTFPlayer", "m_iDisguiseHealth",		DisguiseHealth, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, IHandleEntity,			"CTFPlayer", "m_hDisguiseWeapon",		DisguiseWeaponb, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, float,					"CTFPlayer", "m_flMaxspeed",			MaxSpeed, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					"CTFPlayer", "m_flCloakMeter",			CloakMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					"CTFPlayer", "m_flChargeMeter",			ChargeMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					"CTFPlayer", "m_flRageMeter",			RageMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					"CTFPlayer", "m_flHypeMeter",			HypeMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					"CTFPlayer", "m_flEnergyDrinkMeter",	EnergyDrinkMeter, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, bool,					"CTFPlayer", "m_bRageDraining",			RageDraining, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, bool,					"CTFPlayer", "m_bFeignDeathReady",		FeignDeathReady, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,					"CTFPlayer", "m_bShieldEquipped",		IsShieldEquipped, 0);


	SG_DECL_RECVPROP(ITFPlayerInternal, Angle_F,						"CTFPlayer", "m_angEyeAngles[0]",		EyeAngles, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Angle_F,						"CTFPlayer", "m_vecPunchAngle",			PunchAngle, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Vector3D_F,						"CTFPlayer", "m_vecViewOffset[0]",		ViewOffset, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Vector3D_F,						"CTFPlayer", "m_vecVelocity[0]",		Velocity, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, IBaseHandle[Const::MaxPlayers], "CTFPlayer", "m_hMyWeapons",			MyWeapons, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, IBaseHandle,					"CTFPlayer", "m_hActiveWeapon",			ActiveWeapon, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, IBaseHandle,					"CTFPlayer", "m_hObserverTarget",		ObserverTarget, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Const::PlayerObserverMode,		"CTFPlayer", "m_iObserverMode",			ObserverMode, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, Const::PlayerLifeState,			"CTFPlayer", "m_lifeState",				LifeState, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, Const::TFClass,					"CTFPlayer", "m_iClass",				Class, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, TFPlayerStreak,					"CTFPlayer", "m_nStreaks",				Streaks, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, int,							"CTFPlayer", "m_iHealth",				CurrentHealth, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, float,							"CTFPlayer", "m_flDeathTime",			DeathTime, 0);

	SG_DECL_RECVPROP(ITFPlayerInternal, bool[Const::MaxPlayers],		"CTFPlayer", "m_bPlayerDominatingMe",	PlayerDominatingMe, 0);
	SG_DECL_RECVPROP(ITFPlayerInternal, bool[Const::MaxPlayers],		"CTFPlayer", "m_bPlayerDominated",		PlayerDominated, 0);
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