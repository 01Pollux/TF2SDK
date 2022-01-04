#pragma once 

#include <px/interfaces/GameData.hpp>
#include <tf2/utils/UtlVector.hpp>
#include "BaseEntity.hpp"

TF2_NAMESPACE_BEGIN();

namespace Const
{
	enum class EntBuildingObjType
	{
		Dispenser,
		Teleporter,
		Sentry,
		Sapper,

		Count
	};

	enum class EntBuildingObjMode
	{
		TeleEntrace,
		TeleExit,

		Count
	};

	enum class TeleporterState
	{
		Building,
		Idle,
		Ready,
		Sending,
		Receiving,
		ReceivingRelease,
		Recharging,
		Upgrading,

		Count
	};

	enum class SentryGunState
	{
		Inactive,
		Searching,
		Attacking,

		Count,
	};

	enum class DispenserState
	{
		Idle,
		Upgrading,

		Count

	};
}

class IBaseObjectInternal : public IBaseEntityInternal
{
public:
	static IBaseObjectInternal* GetEntity(int index)
	{
		return static_cast<IBaseObjectInternal*>(IBaseEntityInternal::GetEntity(index));
	}
	static IBaseObjectInternal* GetEntity(IBaseHandle hndl)
	{
		return static_cast<IBaseObjectInternal*>(IBaseEntityInternal::GetEntity(hndl));
	}

	PX_DECL_RECVPROP(IBaseObjectInternal,	int,						Const::EntClassID::CBaseObject, "m_iHealth",						Health, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	int,						Const::EntClassID::CBaseObject, "m_iMaxHealth",						MaxHealth, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	int,						Const::EntClassID::CBaseObject, "m_bHasSapper",						HasSapper, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	Const::EntBuildingObjType,	Const::EntClassID::CBaseObject, "m_iObjectType",					ObjectType, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	Const::EntBuildingObjMode,	Const::EntClassID::CBaseObject, "m_iObjectMode",					ObjectMode, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	bool,						Const::EntClassID::CBaseObject, "m_bBuilding",						IsBuilding, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	bool,						Const::EntClassID::CBaseObject, "m_bPlacing",						IsPlacing, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	bool,						Const::EntClassID::CBaseObject, "m_bCarried",						IsCarried, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	bool,						Const::EntClassID::CBaseObject, "m_bMiniBuilding",					IsMiniBuilding, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	IBaseHandle,				Const::EntClassID::CBaseObject, "m_hBuilder",						Builder, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	int,						Const::EntClassID::CBaseObject, "m_iUpgradeLevel",					UpgradeLevel, 0);
	PX_DECL_RECVPROP(IBaseObjectInternal,	int,						Const::EntClassID::CBaseObject, "m_iUpgradeMetal",					UpgradeMetal, 0);
};
using IBaseObject = IBaseEntityWrapper<IBaseObjectInternal>;


class ISentryGunInternal : public IBaseObjectInternal
{
public:
	PX_DECL_RECVPROP(ISentryGunInternal,	bool,						Const::EntClassID::CObjectSentrygun, "m_iAmmoShells",				AmmoShells, 0);
	PX_DECL_RECVPROP(ISentryGunInternal,	bool,						Const::EntClassID::CObjectSentrygun, "m_iAmmoShells",				MaxAmmoShells, 4);
	PX_DECL_RECVPROP(ISentryGunInternal,	bool,						Const::EntClassID::CObjectSentrygun, "m_iAmmoRockets",				AmmoRockets, 0);
	PX_DECL_RECVPROP(ISentryGunInternal,	bool,						Const::EntClassID::CObjectSentrygun, "m_iAmmoRockets",				MaxAmmoRockets, 4);
	PX_DECL_RECVPROP(ISentryGunInternal,	IBaseHandle,				Const::EntClassID::CObjectSentrygun, "m_bShielded",					IsShielded, 0);
	PX_DECL_RECVPROP(ISentryGunInternal,	int,						Const::EntClassID::CObjectSentrygun, "m_hEnemy",					EnemyTarget, 0);
	PX_DECL_RECVPROP(ISentryGunInternal,	int,						Const::EntClassID::CObjectSentrygun, "m_hAutoAimTarget",			AutoTarget, 0);
	PX_DECL_RECVPROP(ISentryGunInternal,	Const::SentryGunState,		Const::EntClassID::CObjectSentrygun, "m_iState",					State, 0);
	PX_DECL_RECVPROP(ISentryGunInternal,	int,						Const::EntClassID::CObjectSentrygun, "m_bPlayerControlled",			IsPlayerControlled, 0);
};
using ISentryGun = IBaseEntityWrapper<ISentryGunInternal>;


class IDispenserInternal : public IBaseObjectInternal
{
public:
	PX_DECL_RECVPROP(IDispenserInternal,	int,						Const::EntClassID::CObjectDispenser, "m_iAmmoMetal",				AmmoMetal, 0);
	PX_DECL_RECVPROP(IDispenserInternal,	int,						Const::EntClassID::CObjectDispenser, "m_iMiniBombCounter",			MiniBombCounter, 0);
	PX_DECL_RECVPROP(IDispenserInternal,	Const::DispenserState,		Const::EntClassID::CObjectDispenser, "m_iState",					State, 0);
	PX_DECL_RECVPROP(IDispenserInternal,	utils::UtlVector<IBaseHandle>,Const::EntClassID::CObjectDispenser, "m_iMiniBombCounter",		HealingTargets,
		[]() -> int
		{
			return interfaces::SDKManager::Get()->ReadOffset({ "CObjectDispenser", "offsets" }, "m_iMiniBombCounter__To__m_HealingTargets").value_or(0);
		}()
	);
};
using IDispenser = IBaseEntityWrapper<IDispenserInternal>;


class ITeleporterInternal : public IBaseObjectInternal
{
public:
	PX_DECL_RECVPROP(ITeleporterInternal,	int,						Const::EntClassID::CObjectTeleporter, "m_iTimesUsed",				TimesUsed, 0);
	PX_DECL_RECVPROP(ITeleporterInternal,	float,						Const::EntClassID::CObjectTeleporter, "m_flRechargeTime",			RechargeTime, 0);
	PX_DECL_RECVPROP(ITeleporterInternal,	float,						Const::EntClassID::CObjectTeleporter, "m_flCurrentRechargeDuration",CurrentRechageDuration, 0);
	PX_DECL_RECVPROP(ITeleporterInternal,	Const::TeleporterState,		Const::EntClassID::CObjectTeleporter, "m_iState",					State, 0);
};
using ITeleporter = IBaseEntityWrapper<ITeleporterInternal>;


TF2_NAMESPACE_END();
