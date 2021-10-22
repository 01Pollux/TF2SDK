#pragma once 

#include <Interfaces/GameData.hpp>
#include "EconEntity.hpp"

TF2_NAMESPACE_BEGIN();

enum class EntGrenadeProjType
{
	Regular,
	Detonate,
	DetonateRemote,
	CannonBall
};

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

	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFWeaponBase,	"m_iViewModelIndex",	ViewModelIndex, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFWeaponBase,	"m_iWorldModelIndex",	WorldModelIndex, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFWeaponBase,	"m_flLastFireTime",		LastFireTime, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFWeaponBase,	"m_flObservedCritChance", ObservedCritChance, 0);

	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CTFSniperRifle,	"m_flChargedDamage",	ChargedDamage, 0);

	SG_DECL_RECVPROP(IBaseWeaponInternal,	EntGrenadeProjType,	Const::EntClassID::CTFGrenadePipebombProjectile,	"m_iType",		Type, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	IBaseHandle,		Const::EntClassID::CTFGrenadePipebombProjectile,	"m_hLauncher",	Launcher, 0);

	SG_DECL_RECVPROP(IBaseWeaponInternal,	float,				Const::EntClassID::CWeaponMedigun,	"m_flChargeLevel",		ChargeLevel, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	float,				Const::EntClassID::CWeaponMedigun,	"m_hHealingTarget",		HealingTarget, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	bool,				Const::EntClassID::CWeaponMedigun,	"m_bAttacking",			IsAttacking, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	bool,				Const::EntClassID::CWeaponMedigun,	"m_bChargeRelease",		IsReleasingCharge, 0);
	SG_DECL_RECVPROP(IBaseWeaponInternal,	int,				Const::EntClassID::CWeaponMedigun,	"m_nChargeResistType",	ChargeResistType, 0);

	SG_DECL_RECVPROP(IBaseWeaponInternal,	bool,				Const::EntClassID::CTFKnife,	"m_bReadyToBackstab",	ReadyToBackstab, 0);
};
using IBaseWeapon = IBaseEntityWrapper<IBaseWeaponInternal>;

TF2_NAMESPACE_END();
