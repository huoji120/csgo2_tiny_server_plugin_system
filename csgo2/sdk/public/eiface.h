#pragma once
#include "../sdk.h"
#include "IAppSystem.h"
#include "../tier1/bufferstring.h"
#include "../convar/convar.hpp"
struct vis_info_t;
class IHLTVServer;
class IHLTVDirector;
class CSteamID;
class CCommand;
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
//-----------------------------------------------------------------------------
// Purpose: Player / Client related functions
//-----------------------------------------------------------------------------
class ISource2GameClients : public IAppSystem
{
public:
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

	// Client is disconnecting from server
	virtual void			ClientDisconnect(CPlayerSlot slot, /* ENetworkDisconnectionReason */ int reason,
								const char* pszName, uint64_t xuid, const char* pszNetworkID) = 0;

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
	virtual void			ClientEarPosition(CPlayerSlot slot, void* pEarOrigin) = 0;

	// Anything this game .dll wants to add to the bug reporter text (e.g., the entity/model under the picker crosshair)
	//  can be added here
	virtual void			GetBugReportInfo(CBufferString& buf) = 0;

	// TERROR: A player sent a voice packet
	virtual void			ClientVoice(CPlayerSlot slot) = 0;

	// A user has had their network id setup and validated 
	virtual void			NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) = 0;

	// The client has submitted a keyvalues command
	virtual void			ClientCommandKeyValues(CPlayerSlot slot, void* pKeyValues) = 0;

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
