#pragma once 

#include <Interfaces/GameData.hpp>
#include "EconEntity.hpp"

TF2_NAMESPACE_BEGIN();

namespace Const
{
	enum class EntGrenadeProjType
	{
		Regular,
		Detonate,
		DetonateRemote,
		CannonBall
	};
}

class IBaseProjectileInternal : public IEconEntityInternal
{
public:
	SG_DECL_RECVPROP(IBaseProjectileInternal, IBaseHandle,	Const::EntClassID::CTFBaseProjectile, "m_hLauncher",			Launcher, 0);
	SG_DECL_RECVPROP(IBaseProjectileInternal, IBaseHandle,	Const::EntClassID::CTFBaseProjectile, "m_hOriginalLauncher",	OriginalLauncher, 0);
	SG_DECL_RECVPROP(IBaseProjectileInternal, Vector3D_F,	Const::EntClassID::CTFBaseProjectile, "m_vInitialVelocity",		InitialVelocity, 0);
};
using IBaseProjectile = IBaseEntityWrapper<IBaseProjectileInternal>;

class IBaseRocketInternal : public IEconEntityInternal
{
public:
	SG_DECL_RECVPROP(IBaseRocketInternal, IBaseHandle,	Const::EntClassID::CTFBaseProjectile, "m_hLauncher",			Launcher, 0);
	SG_DECL_RECVPROP(IBaseRocketInternal, IBaseHandle,	Const::EntClassID::CTFBaseProjectile, "m_hOriginalLauncher",	OriginalLauncher, 0);
	SG_DECL_RECVPROP(IBaseRocketInternal, int,			Const::EntClassID::CTFBaseProjectile, "m_iDeflected",			Deflected, 0);
	SG_DECL_RECVPROP(IBaseRocketInternal, Vector3D_F,	Const::EntClassID::CTFBaseProjectile, "m_vInitialVelocity",		InitialVelocity, 0);
};
using IBaseRocket = IBaseEntityWrapper<IBaseRocketInternal>;

class IBaseGrenadeInternal : public IEconEntityInternal
{
public:
	SG_DECL_RECVPROP(IBaseGrenadeInternal, IBaseHandle,					Const::EntClassID::CTFGrenadePipebombProjectile,	"m_hOriginalLauncher",OriginalLauncher, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, Vector3D_F,					Const::EntClassID::CTFGrenadePipebombProjectile,	"m_vInitialVelocity",InitialVelocity, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, IBaseHandle,					Const::EntClassID::CTFGrenadePipebombProjectile,	"m_hDeflectOwner",	DeflectOwner, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, bool,						Const::EntClassID::CTFGrenadePipebombProjectile,	"m_bCritical",		IsCritical, 0);
																		
	SG_DECL_RECVPROP(IBaseGrenadeInternal, bool,						Const::EntClassID::CTFGrenadePipebombProjectile,	"m_bDefensiveBomb",	IsDefensiveBomb, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, IBaseHandle,					Const::EntClassID::CTFGrenadePipebombProjectile,	"m_hLauncher",		Launcher, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, Const::EntGrenadeProjType,	Const::EntClassID::CTFGrenadePipebombProjectile,	"m_iType",			Type, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, bool,						Const::EntClassID::CTFGrenadePipebombProjectile,	"m_bTouched",		IsTouched, 0);
																		
	SG_DECL_RECVPROP(IBaseGrenadeInternal, float,						Const::EntClassID::CTFGrenadePipebombProjectile,	"m_flDamage",		Damage, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, float,						Const::EntClassID::CTFGrenadePipebombProjectile,	"m_DmgRadius",		DamageRadius, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, bool,						Const::EntClassID::CTFGrenadePipebombProjectile,	"m_bIsLive",		IsLive, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, IBaseHandle,					Const::EntClassID::CTFGrenadePipebombProjectile,	"m_hThrower",		Thrower, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, IBaseHandle,					Const::EntClassID::CTFGrenadePipebombProjectile,	"m_vecVelocity",	Velocity, 0);
	SG_DECL_RECVPROP(IBaseGrenadeInternal, int,							Const::EntClassID::CTFGrenadePipebombProjectile,	"m_fFlags",			fFlags, 0);
};
using IBaseGrenade = IBaseEntityWrapper<IBaseGrenadeInternal>;


TF2_NAMESPACE_END();
