#pragma once
#include "../sdk.h"
#include "IAppSystem.h"
#include "../tier1/bufferstring.h"
#include "../convar/convar.hpp"
#include "../protobuf-2.6.1/src/google/protobuf/message.h"
struct vis_info_t;
class IHLTVServer;
class IHLTVDirector;
class CSteamID;
class CCommand;
class CEntityLump;
class CResourceManifestPrerequisite;
class INetChannelInfo;
class Vector;
class CPlayerBitVec;
class edict_t;
class CGamestatsData;
class KeyValues;
class CGlobalVars;
class CSharedEdictChangeInfo;
class IAchievementMgr;
class CCommandContext;
typedef uint32_t SpawnGroupHandle_t;
typedef uint32_t SwapChainHandle_t;
struct CEntityIndex
{
	CEntityIndex(int index)
	{
		_index = index;
	}

	int Get() const
	{
		return _index;
	}

	int _index;

	bool operator==(const CEntityIndex& other) const { return other._index == _index; }
	bool operator!=(const CEntityIndex& other) const { return other._index != _index; }
};
class CPlayerUserId
{
public:
	CPlayerUserId(int index)
	{
		_index = index;
	}

	int Get() const
	{
		return _index;
	}

	bool operator==(const CPlayerUserId& other) const { return other._index == _index; }
	bool operator!=(const CPlayerUserId& other) const { return other._index != _index; }

private:
	short _index;
};

class ISource2Engine : public IAppSystem
{
public:
	// Is the game paused?
	virtual bool		IsPaused() = 0;

	// What is the game timescale multiplied with the host_timescale?
	virtual float		GetTimescale(void) const = 0;

	virtual void* FindOrCreateWorldSession(const char* pszWorldName, CResourceManifestPrerequisite*) = 0;

	virtual CEntityLump* GetEntityLumpForTemplate(const char*, bool, const char*, const char*) = 0;

	virtual uint32_t		GetStatsAppID() const = 0;

	virtual void* UnknownFunc1(const char* pszFilename, void* pUnknown1, void* pUnknown2, void* pUnknown3) = 0;
	virtual void		UnknownFunc2() = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Interface the engine exposes to the game DLL
//-----------------------------------------------------------------------------
class IVEngineServer2 : public ISource2Engine
{
public:
	virtual EUniverse	GetSteamUniverse() const = 0;

	virtual void		unk001() = 0;
	virtual void		unk002() = 0;
	virtual void		unk003() = 0;
	virtual void		unk004() = 0;
	virtual void		unk005() = 0;


	// Tell engine to change level ( "changelevel s1\n" or "changelevel2 s1 s2\n" )
	virtual void		ChangeLevel(const char* s1, const char* s2) = 0;

	// Ask engine whether the specified map is a valid map file (exists and has valid version number).
	virtual int			IsMapValid(const char* filename) = 0;

	// Is this a dedicated server?
	virtual bool		IsDedicatedServer(void) = 0;

	// Is this an HLTV relay?
	virtual bool		IsHLTVRelay(void) = 0;

	// Is server only accepting local connections?
	virtual bool		IsServerLocalOnly(void) = 0;

	// Add to the server/client lookup/precache table, the specified string is given a unique index
	// NOTE: The indices for PrecacheModel are 1 based
	//  a 0 returned from those methods indicates the model or sound was not correctly precached
	// However, generic and decal are 0 based
	// If preload is specified, the file is loaded into the server/client's cache memory before level startup, otherwise
	//  it'll only load when actually used (which can cause a disk i/o hitch if it occurs during play of a level).
	virtual int			PrecacheDecal(const char* name, bool preload = false) = 0;
	virtual bool		IsDecalPrecached(const char* s) const = 0;
	virtual int			GetPrecachedDecalIndex(const char* s) const = 0;

	virtual void		UnknownFunc3() = 0;
	virtual void		UnknownFunc4() = 0;

	virtual int			PrecacheGeneric(const char* s, bool preload = false) = 0;
	virtual bool		IsGenericPrecached(char const* s) const = 0;

	// Returns the server assigned userid for this player.  Useful for logging frags, etc.  
	//  returns -1 if the edict couldn't be found in the list of players.
	virtual CPlayerUserId GetPlayerUserId(CPlayerSlot clientSlot) = 0;
	virtual const char* GetPlayerNetworkIDString(CPlayerSlot clientSlot) = 0;
	// Get stats info interface for a client netchannel
	virtual INetChannelInfo* GetPlayerNetInfo(CPlayerSlot nSlot) = 0;

	virtual bool		IsUserIDInUse(int userID) = 0;	// TERROR: used for transitioning
	virtual int			GetLoadingProgressForUserID(int userID) = 0;	// TERROR: used for transitioning

	// Given the current PVS(or PAS) and origin, determine which players should hear/receive the message
	virtual void		Message_DetermineMulticastRecipients(bool usepas, const Vector& origin, CPlayerBitVec& playerbits) = 0;

	// Issue a command to the command parser as if it was typed at the server console.
	virtual void		ServerCommand(const char* str) = 0;
	// Issue the specified command to the specified client (mimics that client typing the command at the console).
	virtual void		ClientCommand(CPlayerSlot nSlot, const char* szFmt, ...) FMTFUNCTION(3, 4) = 0;

	// Set the lightstyle to the specified value and network the change to any connected clients.  Note that val must not 
	//  change place in memory (use MAKE_STRING) for anything that's not compiled into your mod.
	virtual void		LightStyle(int style, const char* val) = 0;

	// Print szMsg to the client console.
	virtual void		ClientPrintf(CPlayerSlot nSlot, const char* szMsg) = 0;

	virtual bool		IsLowViolence() = 0;
	virtual bool		SetHLTVChatBan(int tvslot, bool bBanned) = 0;
	virtual bool		IsAnyClientLowViolence() = 0;

	// Get the current game directory (hl2, tf2, hl1, cstrike, etc.)
	virtual void        GetGameDir(CBufferString& gameDir) = 0;

	// Create a bot with the given name.  Player index is -1 if fake client can't be created
	virtual CPlayerSlot	CreateFakeClient(const char* netname) = 0;

	// Get a convar keyvalue for s specified client
	virtual const char* GetClientConVarValue(CPlayerSlot clientIndex, const char* name) = 0;

	// Print a message to the server log file
	virtual void		LogPrint(const char* msg) = 0;
	virtual bool		IsLogEnabled() = 0;

	virtual bool IsSplitScreenPlayer(CPlayerSlot player) = 0;
	virtual edict_t* GetSplitScreenPlayerAttachToEdict(CPlayerSlot player) = 0;
	virtual int	GetNumSplitScreenUsersAttachedToEdict(CPlayerSlot player) = 0;
	virtual edict_t* GetSplitScreenPlayerForEdict(CPlayerSlot player, int nSlot) = 0;

	// Ret types might be all wrong for these. Haven't researched yet.
	virtual void	UnloadSpawnGroup(SpawnGroupHandle_t spawnGroup, /*ESpawnGroupUnloadOption*/ int) = 0;
	virtual void	SetSpawnGroupDescription(SpawnGroupHandle_t spawnGroup, const char* pszDescription) = 0;
	virtual bool	IsSpawnGroupLoaded(SpawnGroupHandle_t spawnGroup) const = 0;
	virtual bool	IsSpawnGroupLoading(SpawnGroupHandle_t spawnGroup) const = 0;
	virtual void	MakeSpawnGroupActive(SpawnGroupHandle_t spawnGroup) = 0;
	virtual void	SynchronouslySpawnGroup(SpawnGroupHandle_t spawnGroup) = 0;
	virtual void	SynchronizeAndBlockUntilLoaded(SpawnGroupHandle_t spawnGroup) = 0;

	virtual void SetTimescale(float flTimescale) = 0;

	virtual uint32_t		GetAppID() = 0;

	// Returns the SteamID of the specified player. It'll be NULL if the player hasn't authenticated yet.
	virtual const CSteamID* GetClientSteamID(CPlayerSlot clientIndex) = 0;

	// Methods to set/get a gamestats data container so client & server running in same process can send combined data
	virtual void SetGamestatsData(CGamestatsData* pGamestatsData) = 0;
	virtual CGamestatsData* GetGamestatsData() = 0;

	// Send a client command keyvalues
	// keyvalues are deleted inside the function
	virtual void ClientCommandKeyValues(CPlayerSlot client, KeyValues* pCommand) = 0;

	// This makes the host run 1 tick per frame instead of checking the system timer to see how many ticks to run in a certain frame.
	// i.e. it does the same thing timedemo does.
	virtual void SetDedicatedServerBenchmarkMode(bool bBenchmarkMode) = 0;

	// Returns true if this client has been fully authenticated by Steam
	virtual bool IsClientFullyAuthenticated(CPlayerSlot slot) = 0;

	virtual CGlobalVars* GetServerGlobals() = 0;

	// Sets a USERINFO client ConVar for a fakeclient
	virtual void		SetFakeClientConVarValue(CPlayerSlot clientIndex, const char* cvar, const char* value) = 0;

	virtual CSharedEdictChangeInfo* GetSharedEdictChangeInfo() = 0;

	virtual void SetAchievementMgr(IAchievementMgr* pAchievementMgr) = 0;
	virtual IAchievementMgr* GetAchievementMgr() = 0;

	// Fill in the player info structure for the specified player index (name, model, etc.)
	virtual bool GetPlayerInfo(CPlayerSlot clientIndex, google::protobuf::Message& info) = 0;

	// Returns the XUID of the specified player. It'll be NULL if the player hasn't connected yet.
	virtual uint64_t GetClientXUID(CPlayerSlot clientIndex) = 0;

	virtual void* GetPVSForSpawnGroup(SpawnGroupHandle_t spawnGroup) = 0;
	virtual SpawnGroupHandle_t	FindSpawnGroupByName(const char* szName) = 0;

	// Returns the SteamID of the game server
	virtual CSteamID	GetGameServerSteamID() = 0;

	virtual int GetBuildVersion(void) const = 0;

	virtual bool IsClientLowViolence(CEntityIndex clientIndex) = 0;

#if 0 // Don't really match the binary
	virtual void DisconnectClient(CEntityIndex clientIndex, /* ENetworkDisconnectionReason */ int reason) = 0;

	virtual void GetAllSpawnGroupsWithPVS(CUtlVector<SpawnGroupHandle_t>* spawnGroups, CUtlVector<IPVS*>* pOut) = 0;

	virtual void P2PGroupChanged() = 0;
#endif

	virtual void unk006() = 0;
	virtual void unk007() = 0;
	virtual void unk008() = 0;
	virtual void unk009() = 0;
	virtual void unk010() = 0;
	virtual void unk011() = 0;
	virtual void unk012() = 0;
	virtual void unk013() = 0;

	virtual void OnKickClient(const CCommandContext& context, const CCommand& cmd) = 0;

	// Kicks the slot with the specified NetworkDisconnectionReason.
	// Note that the internal reason is never displayed to the user.
	//
	// AM TODO: add header ref for ENetworkDisconnectReason from proto header
	virtual void KickClient(CPlayerSlot slot, const char* szInternalReason, /*ENetworkDisconnectionReason*/ char reason) = 0;

	virtual void unk015() = 0;
	virtual void unk016() = 0;
	virtual void unk017() = 0;
	virtual void unk018() = 0;
	virtual void unk019() = 0;
	virtual void unk020() = 0;
	virtual void unk021() = 0;
	virtual void unk022() = 0;
	virtual void unk023() = 0;

	virtual void SetClientUpdateRate(CEntityIndex clientIndex, float flUpdateRate) = 0;

	virtual void unk024() = 0;
	virtual void unk025() = 0;
};
//-----------------------------------------------------------------------------
// Purpose: Player / Client related functions
//-----------------------------------------------------------------------------
class ISource2GameClients : public IAppSystem
{
public:
	//vmt index 11
	virtual void			OnClientConnected(CPlayerSlot slot, const char* pszName, uint64_t xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer) = 0; 

	// Called when the client attempts to connect (doesn't get called for bots)
	// returning false would reject the connection with the pRejectReason message
	virtual bool			ClientConnect(CPlayerSlot slot, const char* pszName, uint64_t xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason);

	// Client is connected and should be put in the game
	// type values could be:
	// 0 - player
	// 1 - fake player (bot)
	// 2 - unknown
	virtual void			ClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64_t xuid) = 0;

	// Client is going active
	// If bLoadGame is true, don't spawn the player because its state is already setup.
	virtual void			ClientActive(CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64_t xuid) = 0;

	virtual void			ClientFullyConnect(CPlayerSlot slot) = 0;

	//vmt index 16
	// Client is disconnecting from server
	virtual void			ClientDisconnect(CPlayerSlot slot, /* ENetworkDisconnectionReason */ int reason, const char* pszName, uint64_t xuid, const char* pszNetworkID) = 0;

	// Sets the client index for the client who typed the command into his/her console
	// virtual void			SetCommandClient( CPlayerSlot slot) = 0;

	// The client has typed a command at the console
	virtual void			ClientCommand(CPlayerSlot slot, const CCommand& args) = 0;

	// A player changed one/several replicated cvars (name etc)
	virtual void			ClientSettingsChanged(CPlayerSlot slot) = 0;

	// Determine PVS origin and set PVS for the player/viewentity
	virtual void			ClientSetupVisibility(CPlayerSlot slot, vis_info_t* visinfo) = 0;

	// A block of CUserCmds has arrived from the user, decode them and buffer for execution during player simulation
	virtual float			ProcessUsercmds(CPlayerSlot slot, void* buf, int numcmds, bool ignore, bool paused) = 0;

	virtual bool			IsPlayerSlotOccupied(CPlayerSlot slot) = 0;

	virtual bool			IsPlayerAlive(CPlayerSlot slot) = 0;

	virtual int				GetPlayerScore(CPlayerSlot slot) = 0;

	// Get the ear position for a specified client
	virtual void			ClientEarPosition(CPlayerSlot slot, Vector* pEarOrigin) = 0;

	// Anything this game .dll wants to add to the bug reporter text (e.g., the entity/model under the picker crosshair)
	//  can be added here
	virtual void			GetBugReportInfo(CBufferString& buf) = 0;

	// TERROR: A player sent a voice packet
	virtual void			ClientVoice(CPlayerSlot slot) = 0;

	// A user has had their network id setup and validated 
	virtual void			NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) = 0;

	// The client has submitted a keyvalues command
	virtual void			ClientCommandKeyValues(CPlayerSlot slot, KeyValues* pKeyValues) = 0;

	virtual bool			ClientCanPause(CPlayerSlot slot) = 0;

	virtual void			HLTVClientFullyConnect(int index, const CSteamID& steamID) = 0;

	virtual bool			CanHLTVClientConnect(int index, const CSteamID& steamID, int* pRejectReason) = 0;

	virtual void			StartHLTVServer(CEntityIndex index) = 0;

	virtual void			SendHLTVStatusMessage(IHLTVServer*, bool, bool, const char*, int, int, int) = 0;

	virtual IHLTVDirector* GetHLTVDirector(void) = 0;

	virtual void			unk002(CPlayerSlot slot) = 0;
	virtual void			unk003(CPlayerSlot slot) = 0;

	// Something NetMessage related
	virtual void			unk004() = 0;

	// Something pawn related
	virtual void			unk005() = 0;
	virtual void			unk006() = 0;

	virtual void			unk007() = 0;
	virtual void			unk008() = 0;
};
typedef ISource2GameClients IServerGameClients;