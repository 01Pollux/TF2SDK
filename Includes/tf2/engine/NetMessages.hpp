#pragma once

#include "Netchannel.hpp"
#include <tf2/utils/UtlVector.hpp>
#include <tf2/utils/bitbuf.hpp>
#include <tf2/utils/Checksum.hpp>
#include <tf2/math/Vector.hpp>

TF2_NAMESPACE_BEGIN();

namespace Const
{
	static constexpr uint32_t NetMsgType_Bits = 6;

	// Largest # of commands to send in a packet
	static constexpr uint32_t NetMsg_NewCommandBits = 4;
	static constexpr uint32_t NetMsg_NewCommand = (1 << NetMsg_NewCommandBits) - 1;
	// Max number of history commands to send ( 2 by default ) in case of dropped packets
	static constexpr uint32_t NetMsg_BackupCommandBits = 4;
	static constexpr uint32_t NetMsg_BackupCommand = (1 << NetMsg_NewCommandBits) - 1;

	enum class UserMsg
	{
		Geiger,
		Train,
		HudText,
		SayText,
		SayText2,
		TextMsg,
		ResetHUD,
		GameTitle,
		ItemPickup,
		ShowMenu,
		Shake,
		Fade,
		VGUIMenu,
		Rumble,
		CloseCaption,
		SendAudio,
		VoiceMask,
		RequestState,
		Damage,
		HintText,
		KeyHintText,
		HudMsg,
		AmmoDenied,
		AchievementEvent,
		UpdateRadar,
		VoiceSubtitle,
		HudNotify,
		HudNotifyCustom,
		PlayerStatsUpdate,
		MapStatsUpdate,
		PlayerIgnited,
		PlayerIgnitedInv,
		HudArenaNotify,
		UpdateAchievement,
		TrainingMsg,
		TrainingObjective,
		DamageDodged,
		PlayerJarated,
		PlayerExtinguished,
		PlayerJaratedFade,
		PlayerShieldBlocked,
		BreakModel,
		CheapBreakModel,
		BreakModel_Pumpkin,
		BreakModelRocketDud,
		CallVoteFailed,
		VoteStart,
		VotePass,
		VoteFailed,
		VoteSetup,
		PlayerBonusPoints,
		RDTeamPointsChanged,
		SpawnFlyingBird,
		PlayerGodRayEffect,
		PlayerTeleportHomeEffect,
		MVMStatsReset,
		MVMPlayerEvent,
		MVMResetPlayerStats,
		MVMWaveFailed,
		MVMAnnouncement,
		MVMPlayerUpgradedEvent,
		MVMVictory,
		MVMWaveChange,
		MVMLocalPlayerUpgradesClear,
		MVMLocalPlayerUpgradesValue,
		MVMResetPlayerWaveSpendingStats,
		MVMLocalPlayerWaveSpendingValue,
		MVMResetPlayerUpgradeSpending,
		MVMServerKickTimeUpdate,
		PlayerLoadoutUpdated,
		PlayerTauntSoundLoopStart,
		PlayerTauntSoundLoopEnd,
		ForcePlayerViewAngles,
		BonusDucks,
		EOTLDuckEvent,
		PlayerPickupWeapon,
		QuestObjectiveCompleted,
		SPHapWeapEvent,
		HapDmg,
		HapPunch,
		HapSetDrag,
		HapSetConst,
		HapMeleeContact
	};

	enum class SignonStateType
	{
		// no state yet, about to connect
		None,
		// client challenging server, all OOB packets
		Challenge,
		// client is connected to server, netchans ready
		Connected,
		// just got serverinfo and string tables
		New,		
		// received signon buffers
		PreSpawn,
		// ready to receive entity packets
		Spawn,
		// we are fully connected, first non-delta packet received
		Full,
		// server is changing level, please wait
		ChangeLevel	
	};

	enum class NetMsgType
	{
		net_Nop,
		net_Disconnect,
		net_File,

		net_Tick,
		net_StringCmd,
		net_SetConVar,
		net_SignonState,


		// client info (table CRC etc)
		clc_ClientInfo = 8,
		// [CUserCmd]
		clc_Move, 
		// Voicestream data from a client
		clc_VoiceData, 
		// client acknowledges a new baseline seqnr
		clc_BaselineAck,
		// client acknowledges a new baseline seqnr
		clc_ListenEvents, 
		// client is responding to a svc_GetCvarValue message.
		clc_RespondCvarValue,
		// client is sending a file's CRC to the server to be verified.
		clc_FileCRCCheck,
		// client is sending a save replay request to the server.
		clc_SaveReplay,
		clc_CmdKeyValues,
		// client is sending a file's MD5 to the server to be verified.
		clc_FileMD5Check,


		// print text to console
		svc_Print = 7,
		// first message from server about game, map etc
		svc_ServerInfo,
		// sends a sendtable description for a game class
		svc_SendTable,
		// Info about classes (first byte is a CLASSINFO_ define).							
		svc_ClassInfo,
		// tells client if server paused or unpaused
		svc_SetPause,

		// inits shared string tables
		svc_CreateStringTable,
		// updates a string 
		svc_UpdateStringTable,
		// inits used voice codecs & quality
		svc_VoiceInit,
		// Voicestream data from the 
		svc_VoiceData,
		// starts playing 
		svc_Sounds,
		// sets entity as point of view
		svc_SetView,
		// sets/corrects players viewangle
		svc_FixAngle,
		// adjusts crosshair in auto aim mode to lock on 
		svc_CrosshairAngle,
		// add a static decal to the worl BSP
		svc_BSPDecal,

		//, from server side to client side entity
		// a game specific message 
		svc_UserMessage,	
		// a message for an entity
		svc_EntityMessage,	
		// global game event 
		svc_GameEvent,
		// non-delta compressed 
		svc_PacketEntities,
		// non-reliable event 
		svc_TempEntities,
		// only sound indices for 
		svc_Prefetch,	
		// display a menu from a 
		svc_Menu,
		// list of known games events and 
		svc_GameEventList,	
		// Server wants to know the value of a cvar on the 
		svc_GetCvarValue,
		// Server submits KeyValues command for the client
		svc_CmdKeyValues,
		// Timed pause - to avoid breaking demos
		svc_SetPauseTimed,	

		Count
	};

	enum class QueryCvarValueStatus
	{
		// It got the value fine.
		ValueIntact = 0,
		CvarNotFound = 1,
		// There's a ConCommand, but it's not a ConVar.
		NotACvar = 2,	
		// The cvar was marked with FCVAR_SERVER_CAN_NOT_QUERY, so the server is not allowed to have its value.
		CvarProtected = 3
	};
}


struct NetPacket
{
	Const::NetAddress	From;		// sender IP
	int				Source;		// received source 
	double			Received;	// received time
	unsigned char*	Data;		// pointer to raw packet data
	Utils::bf_read	Message;	// easy bitbuf data access
	int				Size;		// size in bytes
	int				WireSize;   // size in bytes before decompression
	bool			Stream;		// was send as stream
	NetPacket*		NextPacket;	// for internal use, should be NULL in public
};

class SendTable;
class KeyValues;
class INetMessageHandler;

using QueryCVarCookie = int;

#define DECLARE_NET_MESSAGE(TYPE, GROUP, NAME, MSGHANDLER)															\
	public:																											\
		MSGHANDLER* MsgHandler{ };																					\
		INetChannel*		GetNetChannel()				const final { return Channel; }								\
		void				SetNetChannel(INetChannel* netchan)		final { Channel = netchan; }					\
		void				SetReliable(bool state)					final { Reliable = state; }						\
		bool				IsReliable()				const final { return Reliable; }							\
		SG_SDK_TF2 bool		ReadFromBuffer(Utils::bf_read& buffer)	final;											\
		SG_SDK_TF2 bool		WriteToBuffer(Utils::bf_write& buffer)	final;											\
		const char*			ToString()					const final { return ""; }									\
		Const::NetMsgType	GetType()					const final { return Const::NetMsgType::NAME##_##TYPE; }	\
		const char*			GetName()					const final { return #NAME## "_" #TYPE; }					\
		Const::NetMsgGroup	GetGroup()					const final { return Const::NetMsgGroup::GROUP; }			\
		bool				Process()						  final { return MsgHandler->Process##TYPE(this); }

class INetMessage
{
public:
	virtual	~INetMessage() = default;

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void	SetNetChannel(INetChannel* netchan) abstract; // netchannel this message is from/for
	virtual void	SetReliable(bool state)			abstract;	// set to true if it's a reliable message

	virtual bool	Process()						abstract; // calles the recently set handler to process this message

	virtual	bool	ReadFromBuffer(Utils::bf_read& buffer) abstract; // returns true if parsing was OK
	virtual	bool	WriteToBuffer(Utils::bf_write& buffer) abstract;	// returns true if writing was OK

	virtual bool	IsReliable()			const abstract;  // true, if message needs reliable handling

	virtual Const::NetMsgType		GetType()		const abstract; // returns module specific header tag eg svc_serverinfo
	virtual Const::NetMsgGroup		GetGroup()		const abstract;	// returns net message group of this message
	virtual const char*				GetName()		const abstract;	// returns network message name, eg "svc_serverinfo"
	virtual INetChannel*			GetNetChannel() const abstract;
	virtual const char*				ToString()		const abstract; // returns a human readable string about message content

	INetMessage() = default;
	INetMessage(const INetMessage&) = default;	INetMessage& operator=(const INetMessage&) = default;
	INetMessage(INetMessage&&) = default;		INetMessage& operator=(INetMessage&&) = default;


	bool			Reliable{ true };
	INetChannel*	Channel{ };
};


class NET_SetConVar : public INetMessage
{
	DECLARE_NET_MESSAGE(SetConVar, StringCmd, net, INetMessageHandler);

	NET_SetConVar() = default;
	NET_SetConVar(const char* name, const char* value)
	{
		insert(name, value);
	}

	void insert(const char* name, const char* value)
	{
		ConVars.push_to_tail({ name, value });
	}

public:
	struct cvar_t
	{
		char	Name[_MAX_PATH];
		char	Value[_MAX_PATH];

		cvar_t() = default;
		cvar_t(const char* name, const char* value)
		{
			sprintf_s(Name, "%s", name);
			sprintf_s(Value, "%s", value);
		}
	};

	Utils::UtlVector<cvar_t> ConVars;
};


class NET_StringCmd : public INetMessage
{
	DECLARE_NET_MESSAGE(StringCmd, StringCmd, net, INetMessageHandler);

	NET_StringCmd() = default;
	NET_StringCmd(const char* cmd) noexcept : Command(cmd) { };

public:
	const char* Command{ };	// execute this command

private:
	char		CommandBuffer[1024];	// buffer for received messages
};


class NET_Tick : public INetMessage
{
	DECLARE_NET_MESSAGE(Tick, Generic, net, INetMessageHandler);

	NET_Tick() noexcept { SetReliable(false); };

	NET_Tick(int tick, float hostFrametime, float hostFrametime_stddeviation) noexcept : Tick(tick), HostFrameTime(hostFrametime), HostFrameTimeStdDeviation(hostFrametime_stddeviation)
	{
		this->Reliable = false;
	};

public:
	int			Tick;
	float		HostFrameTime;
	float		HostFrameTimeStdDeviation;
};


class NET_SignonState : public INetMessage
{
	DECLARE_NET_MESSAGE(SignonState, Signon, net, INetMessageHandler);

	NET_SignonState(Const::SignonStateType state, int spawncount) noexcept : SignonState(state), SpawnCount(spawncount) { };

public:
	Const::SignonStateType	SignonState;
	int						SpawnCount;			// server spawn count (session number)
};




class CLC_ClientInfo : public INetMessage
{
	DECLARE_NET_MESSAGE(ClientInfo, Generic, clc, IClientMessageHandler);

public:
	Utils::CRC32_t	SendTableCRC;
	int				ServerCount;
	bool			IsHLTV;
	bool			IsReplay;
	uint32_t		FriendsID;
	char			FriendsName[32];
	Utils::CRC32_t	CustomFiles[4];
};


class CLC_RespondCvarValue : public INetMessage
{
	int8_t			PAD__[4];
public:
	DECLARE_NET_MESSAGE(RespondCvarValue, Generic, clc, IClientMessageHandler);

	QueryCVarCookie Cookie;

	const char* CvarName;
	const char* CvarValue; // The sender sets this, and it automatically
							   // points it at CvarNameBuffer when
							   // receiving.

	Const::QueryCvarValueStatus StatusCode;

private:
	char CvarNameBuffer[256];
	char CvarValueBuffer[256];
};


class CLC_Move : public INetMessage
{
	DECLARE_NET_MESSAGE(Move, Move, clc, IClientMessageHandler);

	CLC_Move()  noexcept { this->Reliable = false; }

public:
	int				BackupCommands;
	int				NewCommands;
	int				Length;
	Utils::bf_read	DataIn;
	Utils::bf_write	DataOut;
};


class CLC_VoiceData : public INetMessage
{
	DECLARE_NET_MESSAGE(VoiceData, Voice, clc, IClientMessageHandler);

	CLC_VoiceData() noexcept { this->Reliable = false; }

public:
	int				Length;
	Utils::bf_read	DataIn;
	Utils::bf_write	DataOut;
	uint64_t		ID;
};


class CLC_BaselineAck : public INetMessage
{
	DECLARE_NET_MESSAGE(BaselineAck, Entities, clc, IClientMessageHandler);

	CLC_BaselineAck(int tick, int baseline) noexcept : BaselineTick(tick), BaselineNr(baseline) { };

public:
	int		BaselineTick;	// sequence number of baseline
	int		BaselineNr;		// 0 or 1 		
};


class CLC_FileCRCCheck : public INetMessage
{
	DECLARE_NET_MESSAGE(FileCRCCheck, Generic, clc, IClientMessageHandler);

public:	
	char		PathID[_MAX_PATH];
	char			Filename[_MAX_PATH];
	Utils::MD5Value	MD5;
	Utils::CRC32_t	CRCIOs;
	int				FileHashType;
	int				FileLen;
	int				PackFileNumber;
	int				PackFileID;
	int				FileFraction;
};


class CLC_FileMD5Check : public INetMessage
{
	DECLARE_NET_MESSAGE(FileMD5Check, Generic, clc, IClientMessageHandler);

public:
	char		PathID[_MAX_PATH];
	char		Filename[_MAX_PATH];
	Utils::MD5Value	MD5;
};


class CLC_CmdKeyValues : public INetMessage
{
	DECLARE_NET_MESSAGE(CmdKeyValues, Generic, clc, IClientMessageHandler);
	CLC_CmdKeyValues(KeyValues* pKeyValues = nullptr)  noexcept : KV(pKeyValues) { this->Reliable = false; }
	~CLC_CmdKeyValues();

public:
	KeyValues* KV;
};




class SVC_CmdKeyValues : public INetMessage
{
	DECLARE_NET_MESSAGE(CmdKeyValues, Generic, svc, IServerMessageHandler);
	SVC_CmdKeyValues(KeyValues* pKeyValues = nullptr)  noexcept : KV(pKeyValues) { this->Reliable = false; }
	~SVC_CmdKeyValues();

public:
	KeyValues* KV;
};


class SVC_Print : public INetMessage
{
	DECLARE_NET_MESSAGE(Print, Generic, svc, IServerMessageHandler);

	SVC_Print() { Reliable = false; };

	SVC_Print(const char* text) : Text(text) { Reliable = false; };

public:
	const char* Text;	// show this text

private:
	char		TextBuffer[2048];	// buffer for received messages
};

class SVC_ServerInfo : public INetMessage
{
	DECLARE_NET_MESSAGE(ServerInfo, Signon, svc, IServerMessageHandler);

	enum class OSType : char
	{
		Linux = 'L',
		Windows = 'W'
	};
public:	// member vars are public for faster handling
	int				Protocol;	// protocol version
	int				ServerCount;	// number of changelevels since server start
	bool			IsDedicated;  // dedicated server ?	
	bool			IsHLTV;		// HLTV server ?
	bool			IsReplay;	// Replay server ?
	OSType			OS;			// L = linux, W = Win32
	Utils::CRC32_t	MapCRC;		// server map CRC (only used by older demos)
	Utils::MD5Value	MapMD5;		// server map MD5
	int				MaxClients;	// max number of clients on server
	int				MaxClasses;	// max number of server classes
	int				PlayerSlot;	// our client slot number
	float			fTickInterval;// server tick interval
	const char*		GameDir;	// game directory eg "tf2"
	const char*		MapName;	// name of current map 
	const char*		SkyName;	// name of current skybox 
	const char*		HostName;	// server name

private:
	char		GameDirBuffer[_MAX_PATH];// game directory eg "tf2"
	char		MapNameBuffer[_MAX_PATH];// name of current map 
	char		SkyNameBuffer[_MAX_PATH];// name of current skybox 
	char		HostNameBuffer[_MAX_PATH];// name of current skybox 
};

class SVC_SendTable : public INetMessage
{
	DECLARE_NET_MESSAGE(SendTable, Signon, svc, IServerMessageHandler);

public:
	bool			NeedsDecoder;
	int				Length;
	Utils::bf_read	DataIn;
	Utils::bf_write	DataOut;
};

class SVC_ClassInfo : public INetMessage
{
	DECLARE_NET_MESSAGE(ClassInfo, Signon, svc, IServerMessageHandler);

	SVC_ClassInfo() = default;
	SVC_ClassInfo(bool createFromSendTables, int numClasses) : 
		CreateOnClient(createFromSendTables), NumServerClasses(numClasses)
	{ };

public:
	typedef struct class_s
	{
		int		classID;
		char	datatablename[256];
		char	classname[256];
	} class_t;

	bool					CreateOnClient{ };	// if true, client creates own SendTables & classinfos from game.dll
	Utils::UtlVector<class_t>		Classes;
	int						NumServerClasses{ };
};


class SVC_SetPause : public INetMessage
{
	DECLARE_NET_MESSAGE(SetPause, Generic, svc, IServerMessageHandler);

	SVC_SetPause() = default;
	SVC_SetPause(bool state, float end = -1.f) : Paused(state) { }

public:
	bool		Paused;
};

class SVC_SetPauseTimed : public INetMessage
{
	DECLARE_NET_MESSAGE(SetPauseTimed, Generic, svc, IServerMessageHandler);

	SVC_SetPauseTimed() = default;
	SVC_SetPauseTimed(bool bState, float expire_time = -1.f) { Paused = bState; ExpireTime = expire_time; }

public:
	bool		Paused;
	float		ExpireTime;
};


class CNetworkStringTable;

class SVC_CreateStringTable : public INetMessage
{
	DECLARE_NET_MESSAGE(CreateStringTable, Signon, svc, IServerMessageHandler);

public:

	const char* TableName;
	int			MaxEntries;
	int			NumEntries;
	bool		UserDataFixedSize;
	int			UserDataSize;
	int			UserDataSizeBits;
	bool		IsFilenames;
	int			Length;
	Utils::bf_read		DataIn;
	Utils::bf_write	DataOut;
	bool		DataCompressed;

private:
	char		TableNameBuffer[256];
};

class SVC_UpdateStringTable : public INetMessage
{
	DECLARE_NET_MESSAGE(UpdateStringTable, Signon, svc, IServerMessageHandler);

public:
	int				TableID;	// table to be updated
	int				ChangedEntries; // number of how many entries has changed
	int				Length;	// data length in bits
	Utils::bf_read			DataIn;
	Utils::bf_write		DataOut;
};

// SVC_VoiceInit
//   v2 - 2017/02/07
//     - Can detect v2 packets by nLegacyQuality == 255 and presence of additional nSampleRate field.
//     - Added nSampleRate field. Previously, nSampleRate was hard-coded per codec type. ::ReadFromBuffer does a
//       one-time conversion of these old types (which can no longer change)
//     - Marked quality field as deprecated. This was already being ignored. v2 clients send 255
//     - Prior to this the sv_use_steavoice convar was used to switch to steam voice. With this, we properly set
//       VoiceCodec to "steam".  See ::ReadFromBuffer for shim to fallback to the convar for old streams.
//     - We no longer pass "svc_voiceinit NULL" as VoiceCodec if it is not selected, just the empty string.  Nothing
//       used this that I could find.
class SVC_VoiceInit : public INetMessage
{
	DECLARE_NET_MESSAGE(VoiceInit, Signon, svc, IServerMessageHandler);

	SVC_VoiceInit() = default;
	SVC_VoiceInit(const char* codec, size_t size_of_codec, int nSampleRate)
		: SampleRate(nSampleRate)
	{
		strncpy_s(VoiceCodec, sizeof(VoiceCodec), codec ? codec : "", size_of_codec);
	}


public:
	// Used voice codec for voice_init.
	//
	// This used to be a DLL name, then became a whitelisted list of codecs.
	char		VoiceCodec[_MAX_PATH]{ };

	// DEPRECATED:
	//
	// This field used to be a custom quality setting, but it was not honored for a long time: codecs use their own
	// pre-configured quality settings. We never sent anything besides 5, which was then ignored for some codecs.
	//
	// New clients always set 255 here, old clients probably send 5. This could be re-purposed in the future, but beware
	// that very old demos may have non-5 values. It would take more archaeology to determine how to properly interpret
	// those packets -- they're probably using settings we simply don't support any longer.
	//
	// int Quality;

	// The sample rate we are using
	int			SampleRate{ };
};

class SVC_VoiceData : public INetMessage
{
	DECLARE_NET_MESSAGE(VoiceData, Voice, svc, IServerMessageHandler);

	SVC_VoiceData() { Reliable = false; }

public:
	int				FromClient;	// client who has spoken
	bool			Proximity;
	int				Length;		// data length in bits
	uint64_t		xuid;			// X360 player ID

	Utils::bf_read			DataIn;
	void* DataOut;
};

class SVC_Sounds : public INetMessage
{
	DECLARE_NET_MESSAGE(Sounds, Sounds, svc, IServerMessageHandler);

public:

	bool		ReliableSound;
	int			NumSounds;
	int			Length;
	Utils::bf_read		DataIn;
	Utils::bf_write	DataOut;
};

class SVC_Prefetch : public INetMessage
{
	DECLARE_NET_MESSAGE(Prefetch, Sounds, svc, IServerMessageHandler);

public:

	unsigned short	fType;
	unsigned short	SoundIndex;
};

class SVC_SetView : public INetMessage
{
	DECLARE_NET_MESSAGE(SetView, Generic, svc, IServerMessageHandler);

	SVC_SetView() = default;
	SVC_SetView(int entity) : EntityIndex(entity) {  }

public:
	int				EntityIndex;

};

class SVC_FixAngle : public INetMessage
{
	DECLARE_NET_MESSAGE(FixAngle, Generic, svc, IServerMessageHandler);

	SVC_FixAngle() { Reliable = false; };
	SVC_FixAngle(bool relative, Angle_F angle) :
		Angle(angle)
	{
		Reliable = false; Relative = relative; Angle = angle;
	}

public:
	bool			Relative;
	Angle_F			Angle;
};

class SVC_CrosshairAngle : public INetMessage
{
	DECLARE_NET_MESSAGE(CrosshairAngle, Generic, svc, IServerMessageHandler);

	SVC_CrosshairAngle() = default;
	SVC_CrosshairAngle(Angle_F angle) { Angle = angle; }

public:
	Angle_F			Angle;
};

class SVC_BSPDecal : public INetMessage
{
	DECLARE_NET_MESSAGE(BSPDecal, Generic, svc, IServerMessageHandler);

public:
	Vector3D_F	Pos;
	int			DecalTextureIndex;
	int			EntityIndex;
	int			ModelIndex;
	bool		LowPriority;
};

class SVC_GameEvent : public INetMessage
{
	DECLARE_NET_MESSAGE(GameEvent, Events, svc, IServerMessageHandler);

public:
	int			Length;	// data length in bits
	Utils::bf_read		DataIn;
	Utils::bf_write	DataOut;
};

class SVC_UserMessage : public INetMessage
{
	DECLARE_NET_MESSAGE(UserMessage, UserMsgs, svc, IServerMessageHandler);

	SVC_UserMessage() { Reliable = false; }

public:
	int				MsgType;
	int				Length;	// data length in bits
	Utils::bf_read	DataIn;
	Utils::bf_write	DataOut;
};

class SVC_EntityMessage : public INetMessage
{
	DECLARE_NET_MESSAGE(EntityMessage, EntMsgs, svc, IServerMessageHandler);

	SVC_EntityMessage() { Reliable = false; }

public:
	int			EntityIndex;
	int			ClassID;
	int			Length;	// data length in bits
	Utils::bf_read		DataIn;
	Utils::bf_write	DataOut;
};

/* class SVC_SpawnBaseline: public INetMessage
{
	DECLARE_SVC_MESSAGE( SpawnBaseline );
	int	GetGroup() const { return INetChannelInfo::SIGNON; }

public:
	int			EntityIndex;
	int			ClassID;
	int			Length;
	Utils::bf_read		DataIn;
	Utils::bf_write	DataOut;

}; */

class SVC_PacketEntities : public INetMessage
{
	DECLARE_NET_MESSAGE(PacketEntities, Entities, svc, IServerMessageHandler);

public:

	int			MaxEntries;
	int			UpdatedEntries;
	bool		IsDelta;
	bool		UpdateBaseline;
	int			Baseline;
	int			DeltaFrom;
	int			Length;
	Utils::bf_read		DataIn;
	Utils::bf_write	DataOut;
};

class SVC_TempEntities : public INetMessage
{
	DECLARE_NET_MESSAGE(TempEntities, Events, svc, IServerMessageHandler);

	SVC_TempEntities() { Reliable = false; }

public:
	int			NumEntries;
	int			Length;
	Utils::bf_read		DataIn;
	Utils::bf_write	DataOut;
};

class SVC_Menu : public INetMessage
{
public:
	DECLARE_NET_MESSAGE(Menu, Generic, svc, IServerMessageHandler);

	enum class DialogType
	{
		// just an on screen message
		Message = 0,
		// an options menu
		Menu,
		// a richtext dialog
		Text,
		// an entry box
		Entry,
		// Ask the client to connect to a specified IP address. Only the "time" and "title" keys are used.
		AskConnect
	};

	KeyValues*	MenuKeyValues{ };
	DialogType	Type;
	int			iLength;
};

class SVC_GameEventList : public INetMessage
{
public:
	DECLARE_NET_MESSAGE(GameEventList, Generic, svc, IServerMessageHandler);

	int			NumEvents;
	int			Length;
	Utils::bf_read		DataIn;
	Utils::bf_write	DataOut;
};


TF2_NAMESPACE_END();
