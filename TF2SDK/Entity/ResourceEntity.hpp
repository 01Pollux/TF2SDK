#pragma once

#include "BaseEntity.hpp"

TF2_NAMESPACE_BEGIN();

class TFPlayerResourceEntity
{
public:
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iPing",				Ping, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iScore",				Score, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iTotalScore",		TotalScore, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iDeaths",			Deaths, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iTeam",				Team, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, bool[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_bAlive",				IsAlive, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, bool[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_bValid",				IsValid, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, bool[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_bConnected",			IsConnected, 0);

	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iDamage",			Damage, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iDamageBoss",		DamageBoss, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iDamageAssist",		DamageAssist, 0);

	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iHealth",			Health, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iMaxHealth",			MaxHealth, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iMaxBuffedHealth",	MaxBuffedHealth, 0);

	SG_DECL_RECVPROP(TFPlayerResourceEntity, float[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_flNextRespawnTime",	NextSpawnTime, 0);
	SG_DECL_RECVPROP(TFPlayerResourceEntity, int[Const::MaxPlayers],	Const::EntClassID::CTFPlayerResource,	"m_iChargeLevel",		ChargeLevel, 0);

	bool IsValidPlayer(int index) const noexcept
	{
		return IsConnected[index] && IsValid[index];
	}
};

class TFMonsterResourceEntity
{
public:
	SG_DECL_RECVPROP(TFMonsterResourceEntity, int,		Const::EntClassID::CMonsterResource,		"m_iBossHealthPercentageByte",		HealthPerc, 0);
	SG_DECL_RECVPROP(TFMonsterResourceEntity, int,		Const::EntClassID::CMonsterResource,		"m_iBossStunPercentageByte",		StunPerc, 0);

	SG_DECL_RECVPROP(TFMonsterResourceEntity, int,		Const::EntClassID::CMonsterResource,		"m_iBossState",						State, 0);
};

TF2_NAMESPACE_END();