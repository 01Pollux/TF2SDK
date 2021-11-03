#pragma once

#include "GameProp/GameProp.hpp"
#include "TF2Consts.hpp"

TF2_NAMESPACE_BEGIN();

class IGameRules;
namespace Interfaces
{
	TF2_EXPORT_INTERFACE_S(IGameRules, GameRules, "CGameRules");
}


namespace Const
{
	enum class RoundState
	{
		// initialize the game, create teams
		Init,

		// Before players have joined the game. Periodically checks to see if enough players are ready
		// to start a game. Also reverts to this when there are no active players
		Pregame,

		// The game is about to start, wait a bit and spawn everyone
		StartGame,

		// All players are respawned, frozen in place
		Preround,

		// Round is on, playing normally
		RoundRunning,

		// Someone has won the round
		TeamWin,

		// Noone has won, manually restart the game, reset scores
		Restart,

		// Noone has won, restart the game
		Stalemate,

		// Game is over, showing the scoreboard etc
		GameOver,

		// Game is over, doing bonus round stuff
		Bonus,

		// Between rounds
		BetweenRounds
	};

	enum class WinReason
	{
		None,
		All_Points_Captured,
		Opponents_Is_Dead,
		Flag_Capture_Limit,
		Defend_Until_Time_Limit,
		Stalemate,
		Time_Limit,
		Win_Limit,
		WinDiff_Limit,
		Reactor_Captured,
		Cores_Collected,
		Reactor_Returned,
		PlayerDestruction_Points,
		Scored,
		Stopwatch_Watching_Rounds,
		Stopwatch_Watching_Final_Rounds,
		Stopwatch_Watching_Playing_Rounds
	};
}


class IGameRules
{
public:
	SG_SDK_TF2 bool ShouldCollide(Const::EntCollisionGroup group0, Const::EntCollisionGroup group1);

	SG_DECL_RECVPROP(IGameRules, bool,				Const::EntClassID::CTFGameRulesProxy, "m_bInWaitingForPlayers", InWaitingForPlayers, 0);
	SG_DECL_RECVPROP(IGameRules, Const::TFTeam,		Const::EntClassID::CTFGameRulesProxy, "m_iWinningTeam",			WinningTeam, 0);
	SG_DECL_RECVPROP(IGameRules, bool,				Const::EntClassID::CTFGameRulesProxy, "m_bInOvertime",			InOvertime , 0);
	SG_DECL_RECVPROP(IGameRules, bool,				Const::EntClassID::CTFGameRulesProxy, "m_bInSetup",				InSetup , 0);
	SG_DECL_RECVPROP(IGameRules, bool,				Const::EntClassID::CTFGameRulesProxy, "m_bSwitchedTeamsThisRound",SwitchedTeamsThisRound, 0);
	SG_DECL_RECVPROP(IGameRules, bool,				Const::EntClassID::CTFGameRulesProxy, "m_bAwaitingReadyRestart",AwaitingReadyRestart, 0);
	SG_DECL_RECVPROP(IGameRules, bool,				Const::EntClassID::CTFGameRulesProxy, "m_bIsUsingSpells",		IsUsingSpells, 0);
	SG_DECL_RECVPROP(IGameRules, bool,				Const::EntClassID::CTFGameRulesProxy, "m_bTruceActive",			TruceActive, 0);
	SG_DECL_RECVPROP(IGameRules, float,				Const::EntClassID::CTFGameRulesProxy, "m_flRestartRoundTime",	MapResetTime, 0);
	SG_DECL_RECVPROP(IGameRules, int,				Const::EntClassID::CTFGameRulesProxy, "m_nRoundsPlayed",		RoundsPlayed, 0);
	SG_DECL_RECVPROP(IGameRules, int,				Const::EntClassID::CTFGameRulesProxy, "m_nBossHealth",			BossHealth, 0);
	SG_DECL_RECVPROP(IGameRules, int,				Const::EntClassID::CTFGameRulesProxy, "m_nMaxBossHealth",		MaxBossHealth, 0);
	SG_DECL_RECVPROP(IGameRules, Const::RoundState, Const::EntClassID::CTFGameRulesProxy, "m_iRoundState",			RoundState, 0);
};


TF2_NAMESPACE_END();
