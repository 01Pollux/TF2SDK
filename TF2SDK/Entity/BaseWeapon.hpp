#pragma once 

#include <Interfaces/GameData.hpp>
#include "EconEntity.hpp"

TF2_NAMESPACE_BEGIN();

namespace Const
{
	enum class WeaponType
	{
		None = 0,
		Bat,
		Bat_wood,
		Bottle,
		Fireaxe,
		Club,
		Crowbar,
		Knife,
		Fists,
		Shovel,
		Wrench,
		Bonesaw,
		Shotgun_primary,
		Shotgun_soldier,
		Shotgun_hwg,
		Shotgun_pyro,
		Scattergun,
		Sniperrifle,
		Minigun,
		Smg,
		Syringegun_medic,
		Tranq,
		Rocketlauncher,
		Grenadelauncher,
		Pipebomblauncher,
		Flamethrower,
		Grenade_normal,
		Grenade_concussion,
		Grenade_nail,
		Grenade_mirv,
		Grenade_mirv_demoman,
		Grenade_napalm,
		Grenade_gas,
		Grenade_emp,
		Grenade_caltrop,
		Grenade_pipebomb,
		Grenade_smoke_bomb,
		Grenade_heal,
		Grenade_stunball,
		Grenade_jar,
		Grenade_jar_milk,
		Pistol,
		Pistol_scout,
		Revolver,
		Nailgun,
		Pda,
		Pda_engineer_build,
		Pda_engineer_destroy,
		Pda_spy,
		Builder,
		Medigun,
		Grenade_mirvbomb,
		Flamethrower_rocket,
		Grenade_demoman,
		Sentry_bullet,
		Sentry_rocket,
		Dispenser,
		Invis,
		Flaregun,
		Lunchbox,
		Jar,
		Compound_bow,
		Buff_item,
		Pumpkin_bomb,
		Sword,
		Rocketlauncher_directhit,
		Lifeline,
		Laser_pointer,
		Dispenser_gun,
		Sentry_revenge,
		Jar_milk,
		Handgun_scout_primary,
		Bat_fish,
		Crossbow,
		Stickbomb,
		Handgun_scout_secondary,
		Soda_popper,
		Sniperrifle_decap,
		Raygun,
		Particle_cannon,
		Mechanical_arm,
		Drg_pomson,
		Bat_giftwrap,
		Grenade_ornament_ball,
		Flaregun_revenge,
		Pep_brawler_blaster,
		Cleaver,
		Grenade_cleaver,
		Sticky_ball_launcher,
		Grenade_sticky_ball,
		Shotgun_building_rescue,
		Cannon,
		Throwable,
		Grenade_throwable,
		Pda_spy_build,
		Grenade_waterballoon,
		Harvester_saw,
		Spellbook,
		Spellbook_projectile,
		Sniperrifle_classic,
		Parachute,
		Grapplinghook,
		Passtime_gun,

		Charged_smg,

		Count
	};
}

class GameTrace;

class IBaseWeaponInternal : public IEconEntityInternal
{
public:
	static IBaseWeaponInternal* GetEntity(int index)
	{
		return static_cast<IBaseWeaponInternal*>(IBaseEntityInternal::GetEntity(index));
	}
	static IBaseWeaponInternal* GetEntity(IBaseHandle hndl)
	{
		return static_cast<IBaseWeaponInternal*>(IBaseEntityInternal::GetEntity(hndl));
	}

	SG_SDK_TF2 int 
		GetWeaponSlot() const noexcept;

	SG_SDK_TF2 bool 
		DoSwingTrace(GameTrace& trace) noexcept;

	SG_SDK_TF2 int
		GetSwingRange() const noexcept;

	SG_SDK_TF2 Const::WeaponType
		GetWeaponId() const noexcept;

	/*bool GetProjectileMinMaxs(Vector3D_F& mins, Vector3D_F& maxs) const noexcept
	{
		switch (this->GetWeaponId())
		{
		case Const::WeaponType::Crossbow:
		{
			mins = { -3.f, -3.f, -3.f };
			maxs = { 3.f, 3.f, 3.f };
			return true;
		}
		case Const::WeaponType::Compound_bow:
		case Const::WeaponType::Jar:
		case Const::WeaponType::Jar_milk:
		case Const::WeaponType::Shotgun_building_rescue:
		case Const::WeaponType::Spellbook:
		case Const::WeaponType::Mechanical_arm:
		case Const::WeaponType::Flamethrower_rocket:
		{
			mins = { -1.f, -1.f, -1.f };
			maxs = { 1.f, 1.f, 1.f };
			return true;
		}
		}
	}*/

	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFWeaponBase,	"m_iViewModelIndex",	ViewModelIndex, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFWeaponBase,	"m_iWorldModelIndex",	WorldModelIndex, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFWeaponBase,	"m_flLastFireTime",		LastFireTime, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFWeaponBase,	"m_flObservedCritChance", ObservedCritChance, 0);

	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFSniperRifle,	"m_flChargedDamage",	ChargedDamage, 0);

	SG_DECL_RECVPROP(IBaseWeaponInternal,	float,				Const::EntClassID::CWeaponMedigun,	"m_flChargeLevel",		ChargeLevel, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	float,				Const::EntClassID::CWeaponMedigun,	"m_hHealingTarget",		HealingTarget, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	bool,				Const::EntClassID::CWeaponMedigun,	"m_bAttacking",			IsAttacking, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	bool,				Const::EntClassID::CWeaponMedigun,	"m_bChargeRelease",		IsReleasingCharge, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CWeaponMedigun,	"m_nChargeResistType",	ChargeResistType, 0);

	SG_DECL_RECVPROP(IBaseWeaponInternal,	bool,				Const::EntClassID::CTFKnife,	"m_bReadyToBackstab",	ReadyToBackstab, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	IBaseHandle,		Const::EntClassID::CTFKnife,	"m_bReadyToBackstab",	BackstabVictim, -4);
};
using IBaseWeapon = IBaseEntityWrapper<IBaseWeaponInternal>;

TF2_NAMESPACE_END();
