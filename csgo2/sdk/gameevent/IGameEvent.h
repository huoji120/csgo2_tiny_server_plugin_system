#pragma once
#include "../sdk.h"
#include "../player/playerslot.h"
#include "../player/player.h"
#include "../handle/basehandle.h"
#include "../interfaces/interfaces.h"
class CMsgSource1LegacyGameEvent;
class CUtlString;
class IToolGameEventAPI
{
	virtual void unk001(void*) = 0;
};

struct UnkGameEventStruct_t {
	UnkGameEventStruct_t(const char* keyName) {
		m_Unk = 0;
		m_Key = keyName;
	}

	uint64_t m_Unk;
	const char* m_Key;
};

class IGameEvent
{
public:
	virtual ~IGameEvent() {};
	virtual const char* GetName() const = 0;	// get event name
	virtual int GetID() const = 0;

	virtual bool  IsReliable() const = 0; // if event handled reliable
	virtual bool  IsLocal() const = 0; // if event is never networked
	virtual bool  IsEmpty(const char* keyName = NULL) = 0; // check if data field exists

	// Data access
	virtual bool  GetBool(const char* keyName = NULL, bool defaultValue = false) = 0;
	virtual int   GetInt(const char* keyName = NULL, int defaultValue = 0) = 0;
	virtual uint64_t GetUint64(const char* keyName = NULL, uint64_t defaultValue = 0) = 0;
	virtual float GetFloat(const char* keyName = NULL, float defaultValue = 0.0f) = 0;
	virtual const char* GetString(const char* keyName = NULL, const char* defaultValue = "") = 0;
	virtual void* GetPtr(const char* keyName = NULL, void* defaultValue = NULL) = 0;

	/* These function prototypes and names are very speculative and might be incorrect */
	virtual CEntityHandle GetEHandle(UnkGameEventStruct_t* keyName, CEntityHandle defaultValue) = 0;
	virtual CEntityHandle GetStrictEHandle(UnkGameEventStruct_t* keyName, CEntityHandle defaultValue) = 0;
	virtual CEntityHandle GetEHandle2(UnkGameEventStruct_t* keyName, CEntityHandle defaultValue) = 0;

	virtual CPlayerSlot* GetPlayerSlot(UnkGameEventStruct_t* keyName = NULL) = 0;
	virtual CBasePlayer* GetPlayer(UnkGameEventStruct_t* keyName = NULL) = 0;

	virtual void* GetPlayerPawn(UnkGameEventStruct_t* keyName = NULL) = 0;
	virtual CEntityHandle GetPlayerControllerEHandle(UnkGameEventStruct_t* keyName = NULL) = 0;
	virtual CEntityHandle GetPlayerControllerEHandle2(UnkGameEventStruct_t* keyName = NULL) = 0;
	/* ============================================================ */

	virtual void SetBool(const char* keyName, bool value) = 0;
	virtual void SetInt(const char* keyName, int value) = 0;
	virtual void SetUint64(const char* keyName, uint64_t value) = 0;
	virtual void SetFloat(const char* keyName, float value) = 0;
	virtual void SetString(const char* keyName, const char* value) = 0;
	virtual void SetPtr(const char* keyName, void* value) = 0;

	/* These function prototypes and names are very speculative and might be incorrect */
	virtual void SetEHandleStrict(const char* keyName, CEntityHandle handle) = 0;
	virtual void SetEHandle(const char* keyName, CEntityHandle handle) = 0;

	// Also sets the _pawn key
	virtual void SetPlayerSlot(const char* keyName, CPlayerSlot value) = 0;
	virtual void SetPlayer(const char* keyName, CBasePlayer* value) = 0;
	/* ============================================================ */

	virtual bool HasKey(const char* keyName) = 0;

	// Something script vm related
	virtual void unk001() = 0;

	//virtual KeyValues* GetDataKeys() const = 0;
};
class IGameEventListener2
{
public:
	virtual	~IGameEventListener2(void) {};

	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent(IGameEvent* event) = 0;
};
class IGameEventManager2 : public IBaseInterface, public IToolGameEventAPI
{
public:
	virtual	~IGameEventManager2(void) {};

	// load game event descriptions from a file eg "resource\gameevents.res"
	virtual int LoadEventsFromFile(const char* filename, bool bSearchAll) = 0;

	// removes all and anything
	virtual void  Reset() = 0;

	// adds a listener for a particular event
	virtual bool AddListener(IGameEventListener2* listener, const char* name, bool bServerSide) = 0;

	// returns true if this listener is listens to given event
	virtual bool FindListener(IGameEventListener2* listener, const char* name) = 0;

	// removes a listener 
	virtual void RemoveListener(IGameEventListener2* listener) = 0;

	// create an event by name, but doesn't fire it. returns NULL is event is not
	// known or no listener is registered for it. bForce forces the creation even if no listener is active
	virtual IGameEvent* CreateEvent(const char* name, bool bForce = false, int* pCookie = NULL) = 0;

	// fires a server event created earlier, if bDontBroadcast is set, event is not send to clients
	virtual bool FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;

	// fires an event for the local client only, should be used only by client code
	virtual bool FireEventClientSide(IGameEvent* event) = 0;

	// create a new copy of this event, must be free later
	virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;

	// if an event was created but not fired for some reason, it has to bee freed, same UnserializeEvent
	virtual void FreeEvent(IGameEvent* event) = 0;

	// write/read event to/from bitbuffer
	virtual bool SerializeEvent(IGameEvent* event, CMsgSource1LegacyGameEvent* ev) = 0;
	virtual IGameEvent* UnserializeEvent(const CMsgSource1LegacyGameEvent& ev) = 0; // create new KeyValues, must be deleted

	virtual int LookupEventId(const char* name) = 0;

	virtual void PrintEventToString(IGameEvent* event, CUtlString& out) = 0;

	virtual bool HasEventDescriptor(const char* name) = 0;
};

class CGameEventManager : public IGameEventManager2
{
public:	// IGameEventManager functions
	virtual ~CGameEventManager() = 0;

	virtual int	 LoadEventsFromFile(const char* filename) = 0;
	virtual void Reset() = 0;

	virtual bool AddListener(IGameEventListener2* listener, const char* name, bool bServerSide) = 0;
	virtual bool FindListener(IGameEventListener2* listener, const char* name) = 0;
	virtual void RemoveListener(IGameEventListener2* listener) = 0;

	virtual IGameEvent* CreateEvent(const char* name, bool bForce = false) = 0;
	virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;
	virtual bool FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;
	virtual bool FireEventClientSide(IGameEvent* event) = 0;
};
