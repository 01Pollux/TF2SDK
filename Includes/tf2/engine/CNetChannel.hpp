#pragma once

#include "NetChannel.hpp"
#include "Utils/UtlVector.hpp"
#include "Utils/bitbuf.hpp"

TF2_NAMESPACE_BEGIN();

class CNetChannel : public INetChannel
{
public:
	struct DataFragmenets
	{
		void*			File;			// open file handle
		char			FileName[_MAX_PATH]; // filename
		char*			Buffer;			// if NULL it's a file
		unsigned int	Bytes;			// size in bytes
		unsigned int	Bits;			// size in bits
		unsigned int	TransferID;		// only for files
		bool			IsCompressed;	// true if data is bzip compressed
		unsigned int	UncompressedSize; // full size in bytes
		bool			asTCP;			// send as TCP stream
		int				NumFragments;	// number of total fragments
		int				AckedFragments; // number of fragments send & acknowledged
		int				PendingFragments; // number of fragments send, but not acknowledged yet
	};

	struct SubChannel
	{
		int	StartFraggment[2];
		int	NumFragments[2];
		int	SendSeqNr;
		int	State; // 0 = free, 1 = scheduled to send, 2 = send & waiting, 3 = dirty
		int	Index; // index in m_SubChannels[]
	};

	struct NetFrame
	{
		// Data received from server
		float			Time;			// net_time received/send
		int				Size;			// total size in bytes
		float			Latency;		// raw ping for this packet, not cleaned. set when acknowledged otherwise -1.
		float			AvgLatency;	// averaged ping for this packet
		bool			Valid;			// false if dropped, lost, flushed
		int				Choked;			// number of previously chocked packets
		int				Dropped;
		float			InterpolationAmount;
		unsigned short	MsgGroups[static_cast<size_t>(Const::NetMsgGroup::Count)];	// received bytes for each message group
	};

	struct NetFlow
	{
		float		NextCompute;	// Time when we should recompute k/sec data
		float		AvgBytespersec;	// average bytes/sec
		float		AvgPacketspersec;// average packets/sec
		float		AvgLoss;		// average packet loss [0..1]
		float		AvgChoke;		// average packet choke [0..1]
		float		AvgLatency;		// average ping, not cleaned
		float		Latency;		// current ping, more accurate also more jittering
		int			TotalPackets;	// total processed packets
		int			TotalBytes;		// total processed bytes
		int			CurrentIndex;	// current frame index
		NetFrame	Frames[64];		// frame history
		NetFrame* CurrentFrame;	// current frame
	};

	Const::ConnectionStatus State;

	// last send outgoing sequence number
	int			OutSequenceNr;
	// last received incoming sequnec number
	int			InSequenceNr;
	// last received acknowledge outgoing sequnce number
	int			OutSequenceNrAck;

	// state of outgoing reliable data (0/1) flip flop used for loss detection
	int			OutReliableState;
	// state of incoming reliable data
	int			InReliableState;
	// number of choked packets
	int			ChokedPackets;
	// number of dropped packets
	int			PacketDrop;


	// Reliable data buffer, send which each packet (or put in waiting list)
	Utils::bf_write				StreamReliable;
	uint8_t						ReliableDataBuffer[8 * 1024];	// In SP, we don't need much reliable buffer, so save the memory (this is mostly for xbox).
	Utils::UtlVector<uint8_t>	ReliableDataBufferMP;

	Utils::bf_write	StreamUnreliable;
	uint8_t			UnreliableDataBuffer[1400];

	// unreliable message buffer, cleared wich each packet
	Utils::bf_write	StreamVoice;
	Utils::UtlMemory<uint8_t> VoiceDataBuffer;
};

TF2_NAMESPACE_END();
