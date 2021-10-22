#pragma once

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "SoundFlags.hpp"
#include "../Consts.hpp"

TF2_NAMESPACE_BEGIN();


class IVEngineServer;
class IServerGameDLL;

class INetChannelInfo;
class IValveGlobalVars;
class EngineEdict;
class CSaveRestoreData;
class EntityDataMap;
class EntityDataMap_TypeDescription;
class CStandardSendProxies;
class IServerGCLobby;
class bf_write;
class IRecipientFilter;
class VPlane;
class ICollideable;
class ISpatialPartition;
class IScratchPad3D;
class CCheckTransmitInfo;
class CSharedEdictChangeInfo;
class IChangeInfoAccessor;
class IAchievementMgr;
class CGamestatsData;
class CSteamID;
class KeyValues;
class IServer;
struct ServerClass;
struct SendTable;
struct client_textmessage_t;
struct PVSInfo_t;

namespace Utils
{
	template<size_t>
	class UtlBitVec;
}

using QueryCvarCookie = int32_t;
enum class EQueryCvarValueStatus;


namespace Interfaces
{
	TF2_EXPORT_INTERFACE(IVEngineServer, EngineServer,	"VEngineServer023");
	TF2_EXPORT_INTERFACE(IServerGameDLL, ServerDLL,		"ServerGameDLL010");
}


struct bbox_t
{
	Vector3D_F mins, maxs;
};


class IVEngineServer
{
public:
	// Tell engine to change level ( "changelevel s1\n" or "changelevel2 s1 s2\n" )
	virtual void		ChangeLevel(const char* s1, const char* s2) abstract;

	// Ask engine whether the specified map is a valid map file (exists and has valid version number).
	virtual int			IsMapValid(const char* filename) abstract;

	// Is this a dedicated server?
	virtual bool		IsDedicatedServer() abstract;

	// Is in Hammer editing mode?
	virtual int			IsInEditMode() abstract;

	// Add to the server/client lookup/precache table, the specified string is given a unique index
	// NOTE: The indices for PrecacheModel are 1 based
	//  a 0 returned from those methods indicates the model or sound was not correctly precached
	// However, generic and decal are 0 based
	// If preload is specified, the file is loaded into the server/client's cache memory before level startup, otherwise
	//  it'll only load when actually used (which can cause a disk i/o hitch if it occurs during play of a level).
	virtual int			PrecacheModel(const char* s, bool preload = false) abstract;
	virtual int			PrecacheSentenceFile(const char* s, bool preload = false) abstract;
	virtual int			PrecacheDecal(const char* name, bool preload = false) abstract;
	virtual int			PrecacheGeneric(const char* s, bool preload = false) abstract;

	// Check's if the name is precached, but doesn't actually precache the name if not...
	virtual bool		IsModelPrecached(char const* s) const abstract;
	virtual bool		IsDecalPrecached(char const* s) const abstract;
	virtual bool		IsGenericPrecached(char const* s) const abstract;

	// Note that sounds are precached using the IEngineSound interface

	// Special purpose PVS checking
	// Get the cluster # for the specified position
	virtual int			GetClusterForOrigin(const Vector3D_F& org) abstract;
	// Get the PVS bits for a specified cluster and copy the bits into outputpvs.  Returns the number of bytes needed to pack the PVS
	virtual int			GetPVSForCluster(int cluster, int outputpvslength, unsigned char* outputpvs) abstract;
	// Check whether the specified origin is inside the specified PVS
	virtual bool		CheckOriginInPVS(const Vector3D_F& org, const unsigned char* checkpvs, int checkpvssize) abstract;
	// Check whether the specified worldspace bounding box is inside the specified PVS
	virtual bool		CheckBoxInPVS(const Vector3D_F& mins, const Vector3D_F& maxs, const unsigned char* checkpvs, int checkpvssize) abstract;

	// Returns the server assigned userid for this player.  Useful for logging frags, etc.  
	//  returns -1 if the edict couldn't be found in the list of players.
	virtual int			GetPlayerUserId(const EngineEdict* e) abstract;
	virtual const char* GetPlayerNetworkIDString(const EngineEdict* e) abstract;

	// Return the current number of used edict slots
	virtual int			GetEntityCount() abstract;
	// Given an edict, returns the entity index
	virtual int			IndexOfEdict(const EngineEdict* pEdict) abstract;
	// Given and entity index, returns the corresponding edict pointer
	virtual EngineEdict* PEntityOfEntIndex(int iEntIndex) abstract;

	// Get stats info interface for a client netchannel
	virtual INetChannelInfo* GetPlayerNetInfo(int playerIndex) abstract;

	// Allocate space for string and return index/offset of string in global string list
	// If iForceEdictIndex is not -1, then it will return the edict with that index. If that edict index
	// is already used, it'll return null.
	virtual EngineEdict* CreateEdict(int iForceEdictIndex = -1) abstract;
	// Remove the specified edict and place back into the free edict list
	virtual void		RemoveEdict(EngineEdict* e) abstract;

	// Memory allocation for entity class data
	virtual void* PvAllocEntPrivateData(long cb) abstract;
	virtual void		FreeEntPrivateData(void* pEntity) abstract;

	// Save/restore uses a special memory allocator (which zeroes newly allocated memory, etc.)
	virtual void* SaveAllocMemory(size_t num, size_t size) abstract;
	virtual void		SaveFreeMemory(void* pSaveMem) abstract;

	// Emit an ambient sound associated with the specified entity
	virtual void		EmitAmbientSound(int entindex, const Vector3D_F& pos, const char* samp, float vol, Const::SoundFlags::type soundlevel, int fFlags, int pitch, float delay = 0.0f) abstract;

	// Fade out the client's volume level toward silence (or fadePercent)
	virtual void        FadeClientVolume(const EngineEdict* pEdict, float fadePercent, float fadeOutSeconds, float holdTime, float fadeInSeconds) abstract;

	// Sentences / sentence groups
	virtual int			SentenceGroupPick(int groupIndex, char* name, int nameBufLen) abstract;
	virtual int			SentenceGroupPickSequential(int groupIndex, char* name, int nameBufLen, int sentenceIndex, int reset) abstract;
	virtual int			SentenceIndexFromName(const char* pSentenceName) abstract;
	virtual const char* SentenceNameFromIndex(int sentenceIndex) abstract;
	virtual int			SentenceGroupIndexFromName(const char* pGroupName) abstract;
	virtual const char* SentenceGroupNameFromIndex(int groupIndex) abstract;
	virtual float		SentenceLength(int sentenceIndex) abstract;

	// Issue a command to the command parser as if it was typed at the server console.	
	virtual void		ServerCommand(const char* str) abstract;
	// Execute any commands currently in the command parser immediately (instead of once per frame)
	virtual void		ServerExecute() abstract;
	// Issue the specified command to the specified client (mimics that client typing the command at the console).
	virtual void		ClientCommand(EngineEdict* pEdict, const char* szFmt, ...) abstract;

	// Set the lightstyle to the specified value and network the change to any connected clients.  Note that val must not 
	//  change place in memory (use MAKE_STRING) for anything that's not compiled into your mod.
	virtual void		LightStyle(int style, const char* val) abstract;

	// Project a static decal onto the specified entity / model (for level placed decals in the .bsp)
	virtual void		StaticDecal(const Vector3D_F& originInEntitySpace, int decalIndex, int entityIndex, int modelIndex, bool lowpriority) abstract;

	// Given the current PVS(or PAS) and origin, determine which players should hear/receive the message
	virtual void		Message_DetermineMulticastRecipients(bool usepas, const Vector3D_F& origin, Utils::UtlBitVec<Const::MaxPlayers_Absolute>& playerbits) abstract;

	// Begin a message from a server side entity to its client side counterpart (func_breakable glass, e.g.)
	virtual bf_write* EntityMessageBegin(int ent_index, ServerClass* ent_class, bool reliable) abstract;
	// Begin a usermessage from the server to the client .dll
	virtual bf_write* UserMessageBegin(IRecipientFilter* filter, int msg_type) abstract;
	// Finish the Entity or UserMessage and dispatch to network layer
	virtual void		MessageEnd() abstract;

	// Print szMsg to the client console.
	virtual void		ClientPrintf(EngineEdict* pEdict, const char* szMsg) abstract;

	// SINGLE PLAYER/LISTEN SERVER ONLY (just matching the client .dll api for this)
	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void		Con_NPrintf(int pos, const char* fmt, ...) abstract;
	// SINGLE PLAYER/LISTEN SERVER ONLY(just matching the client .dll api for this)
	// Similar to Con_NPrintf, but allows specifying custom text color and duration information
	virtual void		Con_NXPrintf(const struct con_nprint_s* info, const char* fmt, ...) abstract;

	// Change a specified player's "view entity" (i.e., use the view entity position/orientation for rendering the client view)
	virtual void		SetView(const EngineEdict* pClient, const EngineEdict* pViewent) abstract;

	// Get a high precision timer for doing profiling work
	virtual float		Time() abstract;

	// Set the player's crosshair angle
	virtual void		CrosshairAngle(const EngineEdict* pClient, float pitch, float yaw) abstract;

	// Get the current game directory (hl2, tf2, hl1, cstrike, etc.)
	virtual void        GetGameDir(char* szGetGameDir, int maxlength) abstract;

	// Used by AI node graph code to determine if .bsp and .ain files are out of date
	virtual int 		CompareFileTime(const char* filename1, const char* filename2, int* iCompare) abstract;

	// Locks/unlocks the network string tables (.e.g, when adding bots to server, this needs to happen).
	// Be sure to reset the lock after executing your code!!!
	virtual bool		LockNetworkStringTables(bool lock) abstract;

	// Create a bot with the given name.  Returns NULL if fake client can't be created
	virtual EngineEdict* CreateFakeClient(const char* netname) abstract;

	// Get a convar keyvalue for s specified client
	virtual const char* GetClientConVarValue(int clientIndex, const char* name) abstract;

	// Parse a token from a file
	virtual const char* ParseFile(const char* data, char* token, int maxlen) abstract;
	// Copies a file
	virtual bool		CopyFile(const char* source, const char* destination) abstract;

	// Reset the pvs, pvssize is the size in bytes of the buffer pointed to by pvs.
	// This should be called right before any calls to AddOriginToPVS
	virtual void		ResetPVS(uint8_t* pvs, int pvssize) abstract;
	// Merge the pvs bits into the current accumulated pvs based on the specified origin ( not that each pvs origin has an 8 world unit fudge factor )
	virtual void		AddOriginToPVS(const Vector3D_F& origin) abstract;

	// Mark a specified area portal as open/closed.
	// Use SetAreaPortalStates if you want to set a bunch of them at a time.
	virtual void		SetAreaPortalState(int portalNumber, int isOpen) abstract;

	// Queue a temp entity for transmission
	virtual void		PlaybackTempEntity(IRecipientFilter& filter, float delay, const void* pSender, const SendTable* pST, int classID) abstract;
	// Given a node number and the specified PVS, return with the node is in the PVS
	virtual int			CheckHeadnodeVisible(int nodenum, const uint8_t* pvs, int vissize) abstract;
	// Using area bits, cheeck whether area1 flows into area2 and vice versa (depends on area portal state)
	virtual int			CheckAreasConnected(int area1, int area2) abstract;
	// Given an origin, determine which area index the origin is within
	virtual int			GetArea(const Vector3D_F& origin) abstract;
	// Get area portal bit set
	virtual void		GetAreaBits(int area, unsigned char* bits, int buflen) abstract;
	// Given a view origin (which tells us the area to start looking in) and a portal key,
	// fill in the plane that leads out of this area (it points into whatever area it leads to).
	virtual bool		GetAreaPortalPlane(Vector3D_F const& vViewOrigin, int portalKey, VPlane* pPlane) abstract;

	// Save/restore wrapper - FIXME:  At some point we should move this to it's own interface
	virtual bool		LoadGameState(char const* pMapName, bool createPlayers) abstract;
	virtual void		LoadAdjacentEnts(const char* pOldLevel, const char* pLandmarkName) abstract;
	virtual void		ClearSaveDir() abstract;

	// Get the pristine map entity lump string.  (e.g., used by CS to reload the map entities when restarting a round.)
	virtual const char* GetMapEntitiesString() abstract;

	// Text message system -- lookup the text message of the specified name
	virtual client_textmessage_t* TextMessageGet(const char* pName) abstract;

	// Print a message to the server log file
	virtual void		LogPrint(const char* msg) abstract;

	// Builds PVS information for an entity
	virtual void		BuildEntityClusterList(EngineEdict* pEdict, PVSInfo_t* pPVSInfo) abstract;

	// A solid entity moved, update spatial partition
	virtual void SolidMoved(EngineEdict* pSolidEnt, ICollideable* pSolidCollide, const Vector3D_F* pPrevAbsOrigin, bool testSurroundingBoundsOnly) abstract;
	// A trigger entity moved, update spatial partition
	virtual void TriggerMoved(EngineEdict* pTriggerEnt, bool testSurroundingBoundsOnly) abstract;

	// Create/destroy a custom spatial partition
	virtual ISpatialPartition* CreateSpatialPartition(const Vector3D_F& worldmin, const Vector3D_F& worldmax) abstract;
	virtual void 		DestroySpatialPartition(ISpatialPartition*) abstract;

	// Draw the brush geometry in the map into the scratch pad.
	// Flags is currently unused.
	virtual void		DrawMapToScratchPad(IScratchPad3D* pPad, unsigned long iFlags) abstract;

	// This returns which entities, to the best of the server's knowledge, the client currently knows about.
	// This is really which entities were in the snapshot that this client last acked.
	// This returns a bit vector with one bit for each entity.
	//
	// USE WITH CARE. Whatever tick the client is really currently on is subject to timing and
	// ordering differences, so you should account for about a quarter-second discrepancy in here.
	// Also, this will return NULL if the client doesn't exist or if this client hasn't acked any frames yet.
	// 
	// iClientIndex is the CLIENT index, so if you use pPlayer->entindex(), subtract 1.
	virtual const Utils::UtlBitVec<Const::MaxEdicts>* GetEntityTransmitBitsForClient(int iClientIndex) abstract;

	// Is the game paused?
	virtual bool		IsPaused() abstract;

	// Marks the filename for consistency checking.  This should be called after precaching the file.
	virtual void		ForceExactFile(const char* s) abstract;
	virtual void		ForceModelBounds(const char* s, const Vector3D_F& mins, const Vector3D_F& maxs) abstract;
	virtual void		ClearSaveDirAfterClientLoad() abstract;

	// Sets a USERINFO client ConVar for a fakeclient
	virtual void		SetFakeClientConVarValue(EngineEdict* pEntity, const char* cvar, const char* value) abstract;

	// Marks the material (vmt file) for consistency checking.  If the client and server have different
	// contents for the file, the client's vmt can only use the VertexLitGeneric shader, and can only
	// contain $baseTexture and $bumpmap vars.
	virtual void		ForceSimpleMaterial(const char* s) abstract;

	// Is the engine in Commentary mode?
	virtual int			IsInCommentaryMode() abstract;


	// Mark some area portals as open/closed. It's more efficient to use this
	// than a bunch of individual SetAreaPortalState calls.
	virtual void		SetAreaPortalStates(const int* portalNumbers, const int* isOpen, int nPortals) abstract;

	// Called when relevant edict state flags change.
	virtual void		NotifyEdictFlagsChange(int iEdict) abstract;

	// Only valid during CheckTransmit. Also, only the PVS, networked areas, and
	// m_pTransmitInfo are valid in the returned strucutre.
	virtual const CCheckTransmitInfo* GetPrevCheckTransmitInfo(EngineEdict* pPlayerEdict) abstract;

	virtual CSharedEdictChangeInfo* GetSharedEdictChangeInfo() abstract;

	// Tells the engine we can immdiately re-use all edict indices
	// even though we may not have waited enough time
	virtual void			AllowImmediateEdictReuse() abstract;

	// Returns true if the engine is an internal build. i.e. is using the internal bugreporter.
	virtual bool		IsInternalBuild() abstract;

	virtual IChangeInfoAccessor* GetChangeAccessor(const EngineEdict* pEdict) abstract;

	// Name of most recently load .sav file
	virtual char const* GetMostRecentlyLoadedFileName() abstract;
	virtual char const* GetSaveFileName() abstract;

	// Matchmaking
	virtual void MultiplayerEndGame() abstract;
	virtual void ChangeTeam(const char* pTeamName) abstract;

	// Cleans up the cluster list
	virtual void CleanUpEntityClusterList(PVSInfo_t* pPVSInfo) abstract;

	virtual void SetAchievementMgr(IAchievementMgr* pAchievementMgr) abstract;
	virtual IAchievementMgr* GetAchievementMgr() abstract;

	virtual int	GetAppID() abstract;

	virtual bool IsLowViolence() abstract;

	// Call this to find out the value of a cvar on the client.
	//
	// It is an asynchronous query, and it will call IServerGameDLL::OnQueryCvarValueFinished when
	// the value comes in from the client.
	//
	// Store the return value if you want to match this specific query to the OnQueryCvarValueFinished call.
	// Returns InvalidQueryCvarCookie if the entity is invalid.
	virtual QueryCvarCookie StartQueryCvarValue(EngineEdict* pPlayerEntity, const char* pName) abstract;

	virtual void InsertServerCommand(const char* str) abstract;

	// Fill in the player info structure for the specified player index (name, model, etc.)
	virtual bool GetPlayerInfo(int ent_num, Const::PlayerInfo* pinfo) abstract;

	// Returns true if this client has been fully authenticated by Steam
	virtual bool IsClientFullyAuthenticated(EngineEdict* pEdict) abstract;

	// This makes the host run 1 tick per frame instead of checking the system timer to see how many ticks to run in a certain frame.
	// i.e. it does the same thing timedemo does.
	virtual void SetDedicatedServerBenchmarkMode(bool bBenchmarkMode) abstract;

	// Methods to set/get a gamestats data container so client & server running in same process can send combined data
	virtual void SetGamestatsData(CGamestatsData* pGamestatsData) abstract;
	virtual CGamestatsData* GetGamestatsData() abstract;

	// Returns the SteamID of the specified player. It'll be NULL if the player hasn't authenticated yet.
	virtual const CSteamID* GetClientSteamID(EngineEdict* pPlayerEdict) abstract;

	// Returns the SteamID of the game server
	virtual const CSteamID* GetGameServerSteamID() abstract;

	// Send a client command keyvalues
	// keyvalues are deleted inside the function
	virtual void ClientCommandKeyValues(EngineEdict* pEdict, KeyValues* pCommand) abstract;

	// Returns the SteamID of the specified player. It'll be NULL if the player hasn't authenticated yet.
	virtual const CSteamID* GetClientSteamIDByPlayerIndex(int entnum) abstract;
	// Gets a list of all clusters' bounds.  Returns total number of clusters.
	virtual int GetClusterCount() abstract;
	virtual int GetAllClusterBounds(bbox_t* pBBoxList, int maxBBox) abstract;

	// Create a bot with the given name.  Returns NULL if fake client can't be created
	virtual EngineEdict* CreateFakeClientEx(const char* netname, bool bReportFakeClient = true) abstract;

	// Server version from the steam.inf, this will be compared to the GC version
	virtual int GetServerVersion() const abstract;

	// Get sv.GetTime()
	virtual float GetServerTime() const abstract;

	// Exposed for server plugin authors
	virtual IServer* GetIServer() abstract;

	virtual bool IsPlayerNameLocked(const EngineEdict* pEdict) abstract;
	virtual bool CanPlayerChangeName(const EngineEdict* pEdict) abstract;

	// Find the canonical name of a map, given a partial or non-canonical map name.
	// Except in the case of an exact match, pMapName is updated to the canonical name of the match.
	// NOTE That this is subject to the same limitation as ServerGameDLL::CanProvideLevel -- This is non-blocking, so it
	//      is possible that blocking ServerGameDLL::PrepareLevelResources call may be able to pull a better match than
	//      is immediately available to this call (e.g. blocking lookups of cloud maps)
	enum class FindMapResult
	{
		// A direct match for this name was found
		Found,
		// No match for this map name could be found.
		NotFound,
		// A fuzzy match for this mapname was found and pMapName was updated to the full name.
		// Ex: cp_dust -> cp_dustbowl
		FuzzyMatch,
		// A match for this map name was found, and the map name was updated to the canonical version of the
		// name.
		// Ex: workshop/1234 -> workshop/cp_qualified_name.ugc1234
		NonCanonical,
		// No currently available match for this map name could be found, but it may be possible to load ( see caveat
		// about PrepareLevelResources above )
		PossiblyAvailable
	};
	virtual FindMapResult FindMap( /* in/out */ char* pMapName, int nMapNameMax) abstract;

	virtual void SetPausedForced(bool bPaused, float flDuration = -1.f) abstract;
};


class IServerGameDLL
{
public:
	using CreateInterfaceFunction = void* (*)(const char*, int*);
	// Initialize the game (one-time call when the DLL is first loaded )
	// Return false if there is an error during startup.
	virtual bool			DLLInit(
		CreateInterfaceFunction engineFactory,
		CreateInterfaceFunction physicsFactory,
		CreateInterfaceFunction fileSystemFactory,
		IValveGlobalVars* pGlobals
	) abstract;

	// Setup replay interfaces on the server
	virtual bool			ReplayInit(CreateInterfaceFunction fnReplayFactory) abstract;

	// This is called when a new game is started. (restart, map)
	virtual bool			GameInit() abstract;

	// Called any time a new level is started (after GameInit() also on level transitions within a game)
	virtual bool			LevelInit(
		char const* pMapName,
		char const* pMapEntities,
		char const* pOldLevel,
		char const* pLandmarkName,
		bool loadGame,
		bool background
	) abstract;

	// The server is about to activate
	virtual void			ServerActivate(EngineEdict* pEdictList, int edictCount, int clientMax) abstract;

	// The server should run physics/think on all edicts
	virtual void			GameFrame(bool simulating) abstract;

	// Called once per simulation frame on the final tick
	virtual void			PreClientUpdate(bool simulating) abstract;

	// Called when a level is shutdown (including changing levels)
	virtual void			LevelShutdown() abstract;
	// This is called when a game ends (server disconnect, death, restart, load)
	// NOT on level transitions within a game
	virtual void			GameShutdown() abstract;

	// Called once during DLL shutdown
	virtual void			DLLShutdown() abstract;

	// Get the simulation interval (must be compiled with identical values into both client and game .dll for MOD!!!)
	// Right now this is only requested at server startup time so it can't be changed on the fly, etc.
	virtual float			GetTickInterval() const abstract;

	// Give the list of datatable classes to the engine.  The engine matches class names from here with
	//  EngineEdict::classname to figure out how to encode a class's data for networking
	virtual ServerClass* GetAllServerClasses() abstract;

	// Returns string describing current .dll.  e.g., TeamFortress 2, Half-Life 2.  
	//  Hey, it's more descriptive than just the name of the game directory
	virtual const char* GetGameDescription() abstract;

	// Let the game .dll allocate it's own network/shared string tables
	virtual void			CreateNetworkStringTables() abstract;

	// Save/restore system hooks
	virtual CSaveRestoreData* SaveInit(int size) abstract;
	virtual void			SaveWriteFields(CSaveRestoreData*, const char*, void*, EntityDataMap*, EntityDataMap_TypeDescription*, int) abstract;
	virtual void			SaveReadFields(CSaveRestoreData*, const char*, void*, EntityDataMap*, EntityDataMap_TypeDescription*, int) abstract;
	virtual void			SaveGlobalState(CSaveRestoreData*) abstract;
	virtual void			RestoreGlobalState(CSaveRestoreData*) abstract;
	virtual void			PreSave(CSaveRestoreData*) abstract;
	virtual void			Save(CSaveRestoreData*) abstract;
	virtual void			GetSaveComment(char* comment, int maxlength, float flMinutes, float flSeconds, bool bNoTime = false) abstract;
	virtual void			WriteSaveHeaders(CSaveRestoreData*) abstract;
	virtual void			ReadRestoreHeaders(CSaveRestoreData*) abstract;
	virtual void			Restore(CSaveRestoreData*, bool) abstract;
	virtual bool			IsRestoring() abstract;

	// Returns the number of entities moved across the transition
	virtual int				CreateEntityTransitionList(CSaveRestoreData*, int) abstract;
	// Build the list of maps adjacent to the current map
	virtual void			BuildAdjacentMapList() abstract;

	// Retrieve info needed for parsing the specified user message
	virtual bool			GetUserMessageInfo(int msg_type, char* name, int maxnamelength, int& size) abstract;

	// Hand over the StandardSendProxies in the game DLL's module.
	virtual CStandardSendProxies* GetStandardSendProxies() abstract;

	// Called once during startup, after the game .dll has been loaded and after the client .dll has also been loaded
	virtual void			PostInit() abstract;
	// Called once per frame even when no level is loaded...
	virtual void			Think(bool finalTick) abstract;

	virtual void			PreSaveGameLoaded(char const* pSaveName, bool bCurrentlyInGame) abstract;

	// Returns true if the game DLL wants the server not to be made public.
	// Used by commentary system to hide multiplayer commentary servers from the master.
	virtual bool			ShouldHideServer() abstract;

	virtual void			InvalidateMdlCache() abstract;

	// * This function is new with version 6 of the interface.
	//
	// This is called when a query from IServerPluginHelpers::StartQueryCvarValue is finished.
	// iCookie is the value returned by IServerPluginHelpers::StartQueryCvarValue.
	// Added with version 2 of the interface.
	virtual void			OnQueryCvarValueFinished(
		QueryCvarCookie iCookie,
		EngineEdict* pPlayerEntity,
		EQueryCvarValueStatus eStatus,
		const char* pCvarName, 
		const char* pCvarValue
	) abstract;

	// Called after the steam API has been activated post-level startup
	virtual void			GameServerSteamAPIActivated() abstract;

	// Called after the steam API has been shutdown post-level startup
	virtual void			GameServerSteamAPIShutdown() abstract;

	virtual void			SetServerHibernation(bool bHibernating) abstract;

	// interface to the new GC based lobby system
	virtual IServerGCLobby* GetServerGCLobby() abstract;

	// Return override string to show in the server browser
	// "map" column, or NULL to just use the default value
	// (the map name)
	virtual const char* GetServerBrowserMapOverride() abstract;

	// Get gamedata string to send to the master serer updater.
	virtual const char* GetServerBrowserGameData() abstract;

	// Called to add output to the status command
	virtual void 			Status(void (*print) (const char* fmt, ...)) abstract;

	// Informs the game we would like to load this level, giving it a chance to prepare dynamic resources.
	//
	// - pszMapName is the name of the map we're looking for, and may be overridden to e.g. the canonical name of the
	//   map.
	//
	// - pszMapFile is the file we intend to use for this map ( e.g. maps/<mapname>.bsp ), and may be overridden to the
	//   file representing this map name. ( e.g. /path/to/steamapps/workshop/cp_mymap.ugc12345.bsp )
	//
	// This call is blocking, and may block for extended periods. See AsyncPrepareLevelResources below.
	virtual void PrepareLevelResources(
		/* in/out */ char* pszMapName, 
		size_t nMapNameSize,
		/* in/out */ char* pszMapFile, 
		size_t nMapFileSize
	) abstract;

	// Asynchronous version of PrepareLevelResources. Returns preparation status of map when called.
	// If passed, flProgress is filled with the current progress percentage [ 0.f to 1.f ] for the InProgress
	// result
	enum class PrepareLevelResourcesResult
	{
		// Good to go
		Prepared,
		// Game DLL is async preparing (e.g. streaming resources). flProgress will be filled if passed.
		InProgress
	};
	virtual PrepareLevelResourcesResult AsyncPrepareLevelResources(
		_Inout_bytecount_(nMapNameSize) char* pszMapName,
		size_t nMapNameSize,
		_Inout_bytecount_(nMapFileSize) char* pszMapFile,
		size_t nMapFileSize,
		float* flProgress = nullptr
	) abstract;

	// Ask the game DLL to evaluate what it would do with this map name were it passed to PrepareLevelResources.
	// NOTE That this is this is syncronous and non-blocking, so it is possible that async PrepareLevelResources call
	//      may be able to pull a better match than is immediately available to this call (e.g. blocking lookups of
	//      cloud maps)
	enum class CanProvideLevelResult
	{
		// Have no knowledge of this level name, it will be up to the engine to provide. (e.g. as maps/levelname.bsp)
		CannotProvide,
		// Can provide resources for this level, and pMapName has been updated to the canonical name we would provide it
		// under (as with PrepareLevelResources)
		CanProvide,
		// We recognize this level name as something we might be able to prepare, but without a blocking/async call to
		// PrepareLevelResources, it is not possible to say whether it is available.
		Possibly
	};
	virtual CanProvideLevelResult CanProvideLevel(_Inout_bytecount_(nMapNameMax) char* pMapName, int nMapNameMax) abstract;

	// Called to see if the game server is okay with a manual changelevel or map command
	virtual bool			IsManualMapChangeOkay(const char** pszReason) abstract;
};

TF2_NAMESPACE_END();