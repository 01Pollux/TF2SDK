#pragma once

#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN();

class INetChannel;

namespace Const
{
	enum class NetMsgFlowType
	{
		Outgoing,
		Incoming,
	};

	enum class NetMsgGroup
	{
		Generic = 0,	// must be first and is default group
		LocalPlayer,	// bytes for local player entity update
		OtherPlayers,	// bytes for other players update
		Entities,		// all other entity bytes
		Sounds,			// game sounds
		Events,			// event messages
		UserMsgs,		// user messages
		EntMsgs,		// entity messages
		Voice,			// voice data
		StringTable,	// a stringtable update
		Move,			// client move cmds
		StringCmd,		// string command
		Signon,			// various signondata

		Count,			// must be last and is not a real group
	};

	enum NetAddressType
	{
		Null = 0,
		LoopBack,
		Broadcast,
		IP,
	};

	struct NetAddress
	{
		NetAddressType	Type;
		unsigned char	IP[4];
		unsigned short	Port;
	};

	enum class ConnectionStatus
	{
		Disconnected = 0,
		Connecting,
		ConnectionFailed,
		Conntected
	};
}

class INetChannel_Internal
{
public:
	virtual const char* GetName() const abstract;	// get channel name
	virtual const char* GetAddress() const abstract; // get channel IP address as string
	virtual float		GetTime() const abstract;	// current net time
	virtual float		GetTimeConnected() const abstract;	// get connection time in seconds
	virtual int			GetBufferSize() const abstract;	// netchannel packet history size
	virtual int			GetDataRate() const abstract; // send data rate in byte/sec

	virtual bool		IsLoopback() const abstract;	// true if loopback channel
	virtual bool		IsTimingOut() const abstract;	// true if timing out
	virtual bool		IsPlayback() const abstract;	// true if demo playback

	virtual float		GetLatency(Const::NetMsgFlowType flow) const abstract;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(Const::NetMsgFlowType flow) const abstract; // average packet latency in seconds
	virtual float		GetAvgLoss(Const::NetMsgFlowType flow) const abstract;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(Const::NetMsgFlowType flow) const abstract;	 // avg packet choke[0..1]
	virtual float		GetAvgData(Const::NetMsgFlowType flow) const abstract;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(Const::NetMsgFlowType flow) const abstract; // avg packets/sec
	virtual int			GetTotalData(Const::NetMsgFlowType flow) const abstract;	 // total flow in/out in bytes
	virtual int			GetSequenceNr(Const::NetMsgFlowType flow) const abstract;	// last send seq number
	virtual bool		IsValidPacket(Const::NetMsgFlowType flow, int frame_number) const abstract; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(Const::NetMsgFlowType flow, int frame_number) const abstract; // time when packet was send
	virtual int			GetPacketBytes(Const::NetMsgFlowType flow, int frame_number, int group) const abstract; // group size of this packet
	virtual bool		GetStreamProgress(Const::NetMsgFlowType flow, int* received, int* total) const abstract;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived() const abstract;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(Const::NetMsgFlowType flow, int frame_number) const abstract;
	virtual void		GetPacketResponseLatency(Const::NetMsgFlowType flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const abstract;
	virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation) const abstract;

	virtual float		GetTimeoutSeconds() const abstract;
};

class IDemoRecorder;
class INetMessage;
class INetChannelHandler;
class BfWrite;

class INetChannel : public INetChannel_Internal
{
public:
	virtual	~INetChannel() = default;

	virtual void	SetDataRate(float rate) abstract;
	virtual bool	RegisterMessage(INetMessage* msg) abstract;
	virtual void	SetTimeout(float seconds) abstract;
	virtual void	SetDemoRecorder(IDemoRecorder* recorder) abstract;
	virtual void	SetChallengeNr(unsigned int chnr) abstract;

	virtual void	Reset() abstract;
	virtual void	Clear() abstract;
	virtual void	Shutdown(const char* reason) abstract;

	virtual void	ProcessPlayback() abstract;
	virtual void	ProcessPacket(struct netpacket_s* packet, bool bHasHeader) abstract;

	virtual bool	SendNetMsg(INetMessage& msg, bool bForceReliable = false, bool bVoice = false) abstract;
	virtual bool	SendData(BfWrite& msg, bool bReliable = true) abstract;
	virtual bool	SendFile(const char* filename, unsigned int transferID) abstract;
	virtual void	DenyFile(const char* filename, unsigned int transferID) abstract;
	virtual void	RequestFile_OLD(const char* filename, unsigned int transferID) abstract;	// get rid of this function when we version the 
	virtual void	SetChoked() abstract;
	virtual int		SendDatagram(BfWrite* data) abstract;
	virtual bool	Transmit(bool onlyReliable = false) abstract;

	virtual const Const::NetAddress& GetRemoteAddress() const abstract;
	virtual INetChannelHandler* GetMsgHandler() const abstract;
	virtual int				GetDropNumber() const abstract;
	virtual int				GetSocket() const abstract;
	virtual unsigned int	GetChallengeNr() const abstract;
	virtual void			GetSequenceData(int& nOutSequenceNr, int& nInSequenceNr, int& nOutSequenceNrAck) abstract;
	virtual void			SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) abstract;

	virtual void	UpdateMessageStats(int msggroup, int bits) abstract;
	virtual bool	CanPacket() const abstract;
	virtual bool	IsOverflowed() const abstract;
	virtual bool	IsTimedOut() const abstract;
	virtual bool	HasPendingReliableData() abstract;

	virtual void	SetFileTransmissionMode(bool bBackgroundMode) abstract;
	virtual void	SetCompressionMode(bool bUseCompression) abstract;
	virtual unsigned int RequestFile(const char* filename) abstract;
	virtual float	GetTimeSinceLastReceived() const abstract;	// get time since last received packet in seconds

	virtual void	SetMaxBufferSize(bool bReliable, int nBytes, bool bVoice = false) abstract;

	virtual bool	IsNull() const abstract;
	virtual int		GetNumBitsWritten(bool bReliable) abstract;
	virtual void	SetInterpolationAmount(float flInterpolationAmount) abstract;
	virtual void	SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation) abstract;

	// Max # of payload bytes before we must split/fragment the packet
	virtual void	SetMaxRoutablePayloadSize(int nSplitSize) abstract;
	virtual int		GetMaxRoutablePayloadSize() abstract;

	virtual int		GetProtocolVersion() abstract;
};


#define PROCESS_NET_MESSAGE( name )	\
	virtual bool Process##name( NET_##name *msg ) abstract

#define PROCESS_CLC_MESSAGE( name )	\
	virtual bool Process##name( CLC_##name *msg ) abstract

#define PROCESS_SVC_MESSAGE( name )	\
	virtual bool Process##name( SVC_##name *msg ) abstract


class NET_Tick;
class NET_StringCmd;
class NET_SetConVar;
class NET_SignonState;

class INetMessageHandler
{
public:
	PROCESS_NET_MESSAGE(Tick);
	PROCESS_NET_MESSAGE(StringCmd);
	PROCESS_NET_MESSAGE(SetConVar);
	PROCESS_NET_MESSAGE(SignonState);
};


class CLC_ClientInfo;
class CLC_Move;
class CLC_VoiceData;
class CLC_BaselineAck;
class CLC_ListenEvents;
class CLC_RespondCvarValue;
class CLC_FileCRCCheck;
class CLC_FileMD5Check;
class CLC_SaveReplay;
class CLC_CmdKeyValues;

class IClientMessageHandler : public INetMessageHandler
{
public:
	virtual ~IClientMessageHandler(void) {};

	PROCESS_CLC_MESSAGE(ClientInfo);
	PROCESS_CLC_MESSAGE(Move);
	PROCESS_CLC_MESSAGE(VoiceData);
	PROCESS_CLC_MESSAGE(BaselineAck);
	PROCESS_CLC_MESSAGE(ListenEvents);
	PROCESS_CLC_MESSAGE(RespondCvarValue);
	PROCESS_CLC_MESSAGE(FileCRCCheck);
	PROCESS_CLC_MESSAGE(FileMD5Check);
	PROCESS_CLC_MESSAGE(SaveReplay);
	PROCESS_CLC_MESSAGE(CmdKeyValues);
};


class SVC_Print;
class SVC_ServerInfo;
class SVC_SendTable;
class SVC_ClassInfo;
class SVC_SetPause;
class SVC_CreateStringTable;
class SVC_UpdateStringTable;
class SVC_VoiceInit;
class SVC_VoiceData;
class SVC_Sounds;
class SVC_SetView;
class SVC_FixAngle;
class SVC_CrosshairAngle;
class SVC_BSPDecal;
class SVC_GameEvent;
class SVC_UserMessage;
class SVC_EntityMessage;
class SVC_PacketEntities;
class SVC_TempEntities;
class SVC_Prefetch;
class SVC_Menu;
class SVC_GameEventList;
class SVC_GetCvarValue;
class SVC_CmdKeyValues;
class SVC_SetPauseTimed;

class IServerMessageHandler : public INetMessageHandler
{
public:
	virtual ~IServerMessageHandler(void) = default;

	// Returns dem file protocol version, or, if not playing a demo, just returns PROTOCOL_VERSION
	virtual int GetDemoProtocolVersion() const abstract;

	PROCESS_SVC_MESSAGE(Print);
	PROCESS_SVC_MESSAGE(ServerInfo);
	PROCESS_SVC_MESSAGE(SendTable);
	PROCESS_SVC_MESSAGE(ClassInfo);
	PROCESS_SVC_MESSAGE(SetPause);
	PROCESS_SVC_MESSAGE(CreateStringTable);
	PROCESS_SVC_MESSAGE(UpdateStringTable);
	PROCESS_SVC_MESSAGE(VoiceInit);
	PROCESS_SVC_MESSAGE(VoiceData);
	PROCESS_SVC_MESSAGE(Sounds);
	PROCESS_SVC_MESSAGE(SetView);
	PROCESS_SVC_MESSAGE(FixAngle);
	PROCESS_SVC_MESSAGE(CrosshairAngle);
	PROCESS_SVC_MESSAGE(BSPDecal);
	PROCESS_SVC_MESSAGE(GameEvent);
	PROCESS_SVC_MESSAGE(UserMessage);
	PROCESS_SVC_MESSAGE(EntityMessage);
	PROCESS_SVC_MESSAGE(PacketEntities);
	PROCESS_SVC_MESSAGE(TempEntities);
	PROCESS_SVC_MESSAGE(Prefetch);
	PROCESS_SVC_MESSAGE(Menu);
	PROCESS_SVC_MESSAGE(GameEventList);
	PROCESS_SVC_MESSAGE(GetCvarValue);
	PROCESS_SVC_MESSAGE(CmdKeyValues);
	PROCESS_SVC_MESSAGE(SetPauseTimed);
};

TF2_NAMESPACE_END();
