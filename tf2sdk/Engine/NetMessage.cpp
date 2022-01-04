
#include <algorithm>
#include <cctype>
#include <vector>

#include <tf2/engine/NetMessages.hpp>

#include <tf2/utils/KeyValues.hpp>

#include <tf2/consts.hpp>

TF2_NAMESPACE_BEGIN();

bool CLC_VoiceData::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);

	Length = DataOut.bits_written();

	buffer.write_word(Length);	// length in bits

	return buffer.write_bits(DataOut.data(), Length);
}

bool CLC_VoiceData::ReadFromBuffer(utils::bf_read& buffer)
{
	Length = buffer.read_word();	// length in bits

	DataIn = buffer;

	return buffer.seek_relative(Length);
}

bool CLC_Move::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);
	Length = DataOut.bits_written();

	buffer.write_ubit(NewCommands, Const::NetMsg_NewCommandBits);
	buffer.write_ubit(BackupCommands, Const::NetMsg_BackupCommandBits);

	buffer.write_word(Length);

	return buffer.write_bits(DataOut.data(), Length);
}

bool CLC_Move::ReadFromBuffer(utils::bf_read& buffer)
{
	NewCommands = buffer.read_ubit(Const::NetMsg_NewCommandBits);
	BackupCommands = buffer.read_ubit(Const::NetMsg_BackupCommandBits);
	Length = buffer.read_word();
	DataIn = buffer;
	return buffer.seek_relative(Length);
}

bool CLC_ClientInfo::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);

	buffer.write_long(ServerCount);
	buffer.write_long(SendTableCRC);
	buffer.write_bit(IsHLTV ? 1 : 0);
	buffer.write_long(FriendsID);
	buffer.write_string(FriendsName);

	for (int i = 0; i < std::ssize(CustomFiles); i++)
	{
		if (CustomFiles[i] != 0)
		{
			buffer.write_bit(1);
			buffer.write_ubit(CustomFiles[i], 32);
		}
		else
		{
			buffer.write_bit(0);
		}
	}

	buffer.write_bit(IsReplay ? 1 : 0);

	return !buffer.has_overflown();
}

bool CLC_ClientInfo::ReadFromBuffer(utils::bf_read& buffer)
{
	ServerCount = buffer.read_long();
	SendTableCRC = buffer.read_long();
	IsHLTV = buffer.read_bit() != 0;
	FriendsID = buffer.read_long();
	buffer.read_string(FriendsName, sizeof(FriendsName));

	for (int i = 0; i < std::ssize(CustomFiles); i++)
	{
		if (buffer.read_bit() != 0)
		{
			CustomFiles[i] = buffer.read_ubit(32);
		}
		else
		{
			CustomFiles[i] = 0;
		}
	}

	IsReplay = buffer.read_bit() != 0;

	return !buffer.has_overflown();
}

bool CLC_BaselineAck::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);
	buffer.write_long(BaselineTick);
	buffer.write_ubit(BaselineNr, 1);
	return !buffer.has_overflown();
}

bool CLC_BaselineAck::ReadFromBuffer(utils::bf_read& buffer)
{
	BaselineTick = buffer.read_long();
	BaselineNr = buffer.read_ubit(1);
	return !buffer.has_overflown();
}

bool CLC_RespondCvarValue::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);

	buffer.write_sbit(Cookie, 32);
	buffer.write_sbit(static_cast<int>(StatusCode), 4);

	buffer.write_string(CvarName);
	buffer.write_string(CvarValue);

	return !buffer.has_overflown();
}

bool CLC_RespondCvarValue::ReadFromBuffer(utils::bf_read& buffer)
{
	Cookie = buffer.read_sbit(32);
	StatusCode = static_cast<Const::QueryCvarValueStatus>(buffer.read_sbit(4));

	// Read the name.
	buffer.read_string(CvarNameBuffer, sizeof(CvarNameBuffer));
	CvarName = CvarNameBuffer;

	// Read the value.
	buffer.read_string(CvarValueBuffer, sizeof(CvarValueBuffer));
	CvarValue = CvarValueBuffer;

	return !buffer.has_overflown();
}

constexpr const char* MostCommonPathIDs[] =
{
	"GAME",
	"MOD"
};

constexpr const char* MostCommonPrefixes[] =
{
	"materials",
	"models",
	"sounds",
	"scripts"
};

static int FindCommonPathID(const char* pPathID)
{
	for (int i = 0; i < std::ssize(MostCommonPathIDs); i++)
	{
		if (_strcmpi(pPathID, MostCommonPathIDs[i]) == 0)
			return i;
	}
	return -1;
}

static int FindCommonPrefix(const char* pStr)
{
	std::string str(pStr);
	std::transform(str.begin(), str.end(), str.begin(), [](const char c) { return std::tolower(c); });
	for (int i = 0; i < std::ssize(MostCommonPrefixes); i++)
	{
		if (std::strstr(str.c_str(), MostCommonPrefixes[i]) == str.c_str())
		{
			size_t iNextChar = strlen(MostCommonPrefixes[i]);
			if (pStr[iNextChar] == '/' || pStr[iNextChar] == '\\')
				return i;
		}
	}
	return -1;
}


bool CLC_FileCRCCheck::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);

	// Reserved for future use.
	buffer.write_bit(0);

	// Just write a couple bits for the path ID if it's one of the common ones.
	int iCode = FindCommonPathID(PathID);
	if (iCode == -1)
	{
		buffer.write_ubit(0, 2);
		buffer.write_string(PathID);
	}
	else
	{
		buffer.write_ubit(iCode + 1, 2);
	}

	iCode = FindCommonPrefix(Filename);
	if (iCode == -1)
	{
		buffer.write_ubit(0, 3);
		buffer.write_char(1); // so we can detect the new message version
		buffer.write_string(Filename);
	}
	else
	{
		buffer.write_ubit(iCode + 1, 3);
		buffer.write_char(1); // so we can detect the new message version
		buffer.write_string(&Filename[strlen(MostCommonPrefixes[iCode]) + 1]);
	}

	buffer.write_bits(&MD5.bits, sizeof(MD5.bits) * 8);
	buffer.write_ubit(CRCIOs, 32);
	buffer.write_ubit(FileHashType, 32);
	buffer.write_ubit(FileLen, 32);
	buffer.write_ubit(PackFileNumber, 32);
	buffer.write_ubit(PackFileID, 32);
	buffer.write_ubit(FileFraction, 32);
	return !buffer.has_overflown();
}

bool CLC_FileCRCCheck::ReadFromBuffer(utils::bf_read& buffer)
{
	// Reserved for future use.
	buffer.read_bit();

	// Read the path ID.
	int iCode = buffer.read_ubit(2);
	if (iCode == 0)
	{
		buffer.read_string(PathID, sizeof(PathID));
	}
	else if ((iCode - 1) < std::ssize(MostCommonPathIDs))
	{
		strncpy_s(PathID, MostCommonPathIDs[iCode - 1], sizeof(PathID));
	}
	else
	{
		return false;
	}

	// Prefix string
	iCode = buffer.read_ubit(3);

	// Read filename, and check for the new message format version?
	char Temp[_MAX_PATH];
	int c = buffer.read_char();
	bool bNewVersion = false;
	if (c == 1)
	{
		bNewVersion = true;
		buffer.read_string(Temp, sizeof(Temp));
	}
	else
	{
		Temp[0] = (char)c;
		buffer.read_string(Temp + 1, sizeof(Temp) - 1);
	}
	if (iCode == 0)
	{
		strcpy_s(Filename, Temp);
	}
	else if ((iCode - 1) < std::ssize(MostCommonPrefixes))
	{
		sprintf_s(Filename, "%s/%s", MostCommonPrefixes[iCode - 1], Temp);
	}
	else
	{
		return false;
	}

	if (bNewVersion)
	{
		buffer.read_bits(&MD5.bits, sizeof(MD5.bits) * 8);
		CRCIOs = buffer.read_ubit(32);
		FileHashType = buffer.read_ubit(32);
		FileLen = buffer.read_ubit(32);
		PackFileNumber = buffer.read_ubit(32);
		PackFileID = buffer.read_ubit(32);
		FileFraction = buffer.read_ubit(32);
	}
	else
	{
		/* CRC */ buffer.read_ubit(32);
		CRCIOs = buffer.read_ubit(32);
		FileHashType = buffer.read_ubit(32);
	}

	return !buffer.has_overflown();
}

bool CLC_FileMD5Check::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);

	// Reserved for future use.
	buffer.write_bit(0);

	// Just write a couple bits for the path ID if it's one of the common ones.
	int iCode = FindCommonPathID(PathID);
	if (iCode == -1)
	{
		buffer.write_ubit(0, 2);
		buffer.write_string(PathID);
	}
	else
	{
		buffer.write_ubit(iCode + 1, 2);
	}

	iCode = FindCommonPrefix(Filename);
	if (iCode == -1)
	{
		buffer.write_ubit(0, 3);
		buffer.write_string(Filename);
	}
	else
	{
		buffer.write_ubit(iCode + 1, 3);
		buffer.write_string(&Filename[strlen(MostCommonPrefixes[iCode]) + 1]);
	}

	buffer.write_bytes(MD5.bits, MD5_DIGEST_LENGTH);
	return !buffer.has_overflown();
}

bool CLC_FileMD5Check::ReadFromBuffer(utils::bf_read& buffer)
{
	// Reserved for future use.
	buffer.read_bit();

	// Read the path ID.
	int iCode = buffer.read_ubit(2);
	if (iCode == 0)
	{
		buffer.read_string(PathID, sizeof(PathID));
	}
	else if ((iCode - 1) < std::ssize(MostCommonPathIDs))
	{
		strncpy_s(PathID, MostCommonPathIDs[iCode - 1], sizeof(PathID));
	}
	else
	{
		return false;
	}

	// Read the filename.
	iCode = buffer.read_ubit(3);
	if (iCode == 0)
	{
		buffer.read_string(Filename, sizeof(Filename));
	}
	else if ((iCode - 1) < std::ssize(MostCommonPrefixes))
	{
		char Temp[_MAX_PATH];
		buffer.read_string(Temp, sizeof(Temp));
		snprintf(Filename, sizeof(Filename), "%s/%s", MostCommonPrefixes[iCode - 1], Temp);
	}
	else
	{
		return false;
	}

	buffer.read_bytes(MD5.bits, MD5_DIGEST_LENGTH);

	return !buffer.has_overflown();
}

CLC_CmdKeyValues::~CLC_CmdKeyValues()
{
	if (KV)
		KV->DeleteThis();
	KV = nullptr;
}

bool CLC_CmdKeyValues::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool CLC_CmdKeyValues::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

SVC_CmdKeyValues::~SVC_CmdKeyValues()
{
	if (KV)
		KV->DeleteThis();
	KV = nullptr;
}

bool SVC_CmdKeyValues::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_CmdKeyValues::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}


bool SVC_Print::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_Print::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}


bool NET_StringCmd::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);
	return buffer.write_string(Command ? Command : " NET_StringCmd NULL");
}

bool NET_StringCmd::ReadFromBuffer(utils::bf_read& buffer)
{
	Command = CommandBuffer;

	return buffer.read_string(CommandBuffer, sizeof(CommandBuffer));
}


bool SVC_ServerInfo::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_ServerInfo::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool NET_SignonState::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);
	buffer.write_byte(static_cast<uint8_t>(SignonState));
	buffer.write_long(SpawnCount);

	return !buffer.has_overflown();
}

bool NET_SignonState::ReadFromBuffer(utils::bf_read& buffer)
{
	SignonState = static_cast<Const::SignonStateType>(buffer.read_byte());
	SpawnCount = buffer.read_long();

	return !buffer.has_overflown();
}


bool SVC_BSPDecal::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_BSPDecal::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_SetView::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_SetView::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_FixAngle::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_FixAngle::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_CrosshairAngle::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_CrosshairAngle::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_VoiceInit::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_VoiceInit::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}


bool SVC_VoiceData::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_VoiceData::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

#define NET_TICK_SCALEUP	100000.0f

bool NET_Tick::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);
	buffer.write_long(Tick);
	buffer.write_ubit(std::clamp((int)(NET_TICK_SCALEUP * HostFrameTime), 0, 65535), 16);
	buffer.write_ubit(std::clamp((int)(NET_TICK_SCALEUP * HostFrameTimeStdDeviation), 0, 65535), 16);
	return !buffer.has_overflown();
}

bool NET_Tick::ReadFromBuffer(utils::bf_read& buffer)
{
	Tick = buffer.read_long();
	HostFrameTime = (float)buffer.read_ubit(16) / NET_TICK_SCALEUP;
	HostFrameTimeStdDeviation = (float)buffer.read_ubit(16) / NET_TICK_SCALEUP;
	return !buffer.has_overflown();
}

bool SVC_UserMessage::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_UserMessage::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_SetPause::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_SetPause::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_SetPauseTimed::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_SetPauseTimed::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool NET_SetConVar::WriteToBuffer(utils::bf_write& buffer)
{
	buffer.write_ubit(static_cast<uint32_t>(GetType()), Const::NetMsgType_Bits);

	int numvars = ConVars.size();

	// Note how many we're sending
	buffer.write_byte(numvars);

	for (const auto& var : ConVars)
	{
		buffer.write_string(var.Name);
		buffer.write_string(var.Value);
	}

	return !buffer.has_overflown();
}

bool NET_SetConVar::ReadFromBuffer(utils::bf_read& buffer)
{
	int numvars = buffer.read_byte();

	ConVars.clear();

	for (int i = 0; i < numvars; i++)
	{
		cvar_t var;
		buffer.read_string(var.Name, sizeof(var.Name));
		buffer.read_string(var.Value, sizeof(var.Value));
		ConVars.push_to_tail(var);

	}
	return !buffer.has_overflown();
}


bool SVC_UpdateStringTable::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_UpdateStringTable::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_CreateStringTable::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_CreateStringTable::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_Sounds::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_Sounds::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_Prefetch::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_Prefetch::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_TempEntities::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_TempEntities::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_ClassInfo::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_ClassInfo::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_GameEvent::WriteToBuffer(utils::bf_write& buffer)
{
	return false;

}

bool SVC_GameEvent::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_SendTable::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_SendTable::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_EntityMessage::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_EntityMessage::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_PacketEntities::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_PacketEntities::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_Menu::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_Menu::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}

bool SVC_GameEventList::WriteToBuffer(utils::bf_write& buffer)
{
	return false;
}

bool SVC_GameEventList::ReadFromBuffer(utils::bf_read& buffer)
{
	return false;
}


TF2_NAMESPACE_END();