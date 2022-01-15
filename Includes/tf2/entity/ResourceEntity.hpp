#pragma once

#include "BaseEntity.hpp"

TF2_NAMESPACE_BEGIN();

class TFPlayerResourceEntity
{
public:
	[[nodiscard]] static PX_SDK_TF2 TFPlayerResourceEntity*
		Get();

	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iPing",				Ping, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iScore",				Score, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iTotalScore",		TotalScore, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iDeaths",			Deaths, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iTeam",				Team, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, bool,	Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_bAlive",				IsAlive, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, bool,	Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_bValid",				IsValid, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, bool,	Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_bConnected",			IsConnected, 0);

	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iDamage",			Damage, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iDamageBoss",		DamageBoss, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iDamageAssist",		DamageAssist, 0);

	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iHealth",			Health, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iMaxHealth",			MaxHealth, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iMaxBuffedHealth",	MaxBuffedHealth, 0);

	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, float,	Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_flNextRespawnTime",	NextSpawnTime, 0);
	PX_DECL_RECVPROP_A(TFPlayerResourceEntity, int,		Const::MaxPlayers,	Const::EntClassID::CTFPlayerResource,	"m_iChargeLevel",		ChargeLevel, 0);

	[[nodiscard]] bool IsValidPlayer(int index) const noexcept
	{
		return IsConnected[index] && IsValid[index];
	}
};

class TFMonsterResourceEntity
{
public:
	[[nodiscard]] static PX_SDK_TF2 TFMonsterResourceEntity*
		Get();

	PX_DECL_RECVPROP(TFMonsterResourceEntity, int,		Const::EntClassID::CMonsterResource,		"m_iBossHealthPercentageByte",		HealthPerc, 0);
	PX_DECL_RECVPROP(TFMonsterResourceEntity, int,		Const::EntClassID::CMonsterResource,		"m_iBossStunPercentageByte",		StunPerc, 0);

	PX_DECL_RECVPROP(TFMonsterResourceEntity, int,		Const::EntClassID::CMonsterResource,		"m_iBossState",						State, 0);
};

TF2_NAMESPACE_END();