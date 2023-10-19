#pragma once
#include "../sdk.h"
#include "../player/playerslot.h"
#include "../player/player.h"
#include "../handle/basehandle.h"
#include "../interfaces/interfaces.h"
class CMsgSource1LegacyGameEvent;
class CUtlString;
class IToolGameEventAPI {
    virtual void unk001(void*) = 0;
};

static auto MurmurHash2(const void* key, int len, uint32 seed) -> uint32_t
{
#define LittleDWord( val )			( val )

    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const uint32 m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value

    uint32 h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char* data = (const unsigned char*)key;

    while (len >= 4)
    {
        uint32 k = LittleDWord(*(uint32*)data);

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch (len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
        h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

struct GameEventKeySymbol_t {
    GameEventKeySymbol_t(const char* keyName) {
        if (keyName != nullptr) {
            m_nHashCode = MurmurHash2(keyName, strlen(keyName), 0x31415926);
            m_pszKeyName = keyName;
        }
    }

    uint64_t m_nHashCode;
    const char* m_pszKeyName;
};

class IHandleEntity
{
    virtual void Schema_DynamicBinding(void**) = 0;
public:
    virtual ~IHandleEntity() = 0;
    virtual const CEntityHandle GetRefEHandle() const = 0;
};
class IGameEvent{
   public:
    virtual ~IGameEvent() {};
    virtual const char* GetName() const = 0;  // get event name
    virtual int GetID() const = 0;

    virtual bool IsReliable() const = 0;  // if event handled reliable
    virtual bool IsLocal() const = 0;     // if event is never networked
    virtual bool IsEmpty(const GameEventKeySymbol_t
                             & keySymbol) = 0;  // check if data field exists

    // Data access
    virtual bool GetBool(const GameEventKeySymbol_t& keySymbol,
                         bool defaultValue = false) = 0;
    virtual int GetInt(const GameEventKeySymbol_t& keySymbol,
                       int defaultValue = 0) = 0;
    virtual uint64 GetUint64(const GameEventKeySymbol_t& keySymbol,
                             uint64 defaultValue = 0) = 0;
    virtual float GetFloat(const GameEventKeySymbol_t& keySymbol,
                           float defaultValue = 0.0f) = 0;
    virtual const char* GetString(const GameEventKeySymbol_t& keySymbol,
                                  const char* defaultValue = "") = 0;
    virtual void* GetPtr(const GameEventKeySymbol_t& keySymbol) = 0;

    virtual CEntityHandle GetEHandle(
        const GameEventKeySymbol_t& keySymbol,
        CEntityHandle defaultValue = CEntityHandle()) = 0;

    // Returns the entity instance, mostly used for _pawn keys, might return 0
    // if used on any other key (even on a controller).
    virtual IHandleEntity* GetEntity(
        const GameEventKeySymbol_t& keySymbol,
        IHandleEntity* fallbackInstance = NULL) = 0;
    virtual CEntityIndex GetEntityIndex(
        const GameEventKeySymbol_t& keySymbol,
        CEntityIndex defaultValue = CEntityIndex(-1)) = 0;

    virtual CPlayerSlot GetPlayerSlot(
        const GameEventKeySymbol_t& keySymbol) = 0;

    virtual IHandleEntity* GetPlayerController(
        const GameEventKeySymbol_t& keySymbol) = 0;
    virtual IHandleEntity* GetPlayerPawn(
        const GameEventKeySymbol_t& keySymbol) = 0;

    // Returns the EHandle for the _pawn entity.
    virtual CEntityHandle GetPawnEHandle(
        const GameEventKeySymbol_t& keySymbol) = 0;
    // Returns the CEntityIndex for the _pawn entity.
    virtual CEntityIndex GetPawnEntityIndex(
        const GameEventKeySymbol_t& keySymbol) = 0;

    virtual void SetBool(const GameEventKeySymbol_t& keySymbol, bool value) = 0;
    virtual void SetInt(const GameEventKeySymbol_t& keySymbol, int value) = 0;
    virtual void SetUint64(const GameEventKeySymbol_t& keySymbol,
                           uint64 value) = 0;
    virtual void SetFloat(const GameEventKeySymbol_t& keySymbol,
                          float value) = 0;
    virtual void SetString(const GameEventKeySymbol_t& keySymbol,
                           const char* value) = 0;
    virtual void SetPtr(const GameEventKeySymbol_t& keySymbol, void* value) = 0;

    virtual void SetEntity(const GameEventKeySymbol_t& keySymbol,
                           CEntityIndex value) = 0;
    virtual void SetEntity(const GameEventKeySymbol_t& keySymbol,
                           IHandleEntity* value) = 0;

    // Also sets the _pawn key
    virtual void SetPlayer(const GameEventKeySymbol_t& keySymbol,
                           CPlayerSlot value) = 0;
    // Also sets the _pawn key (Expects pawn entity to be passed)
    virtual void SetPlayer(const GameEventKeySymbol_t& keySymbol,
                           IHandleEntity* pawn) = 0;

    // Expects pawn entity to be passed, will set the controller entity as a
    // controllerKeyName and pawn entity as a pawnKeyName.
    virtual void SetPlayerRaw(const GameEventKeySymbol_t& controllerKeySymbol,
                              const GameEventKeySymbol_t& pawnKeySymbol,
                              IHandleEntity* pawn) = 0;

    virtual bool HasKey(const GameEventKeySymbol_t& keySymbol) = 0;

    // Something script vm related
    virtual void unk001() = 0;

    // Not based on keyvalues anymore as it seems like
    virtual void* GetDataKeys() const = 0;
};

class IGameEventListener2 {
   public:
    virtual ~IGameEventListener2(void){};

    // FireEvent is called by EventManager if event just occured
    // KeyValue memory will be freed by manager if not needed anymore
    virtual void FireGameEvent(IGameEvent* event) = 0;
};
class IGameEventManager2 : public IBaseInterface, public IToolGameEventAPI {
   public:
    virtual ~IGameEventManager2(void){};

    // load game event descriptions from a file eg "resource\gameevents.res"
    virtual int LoadEventsFromFile(const char* filename, bool bSearchAll) = 0;

    // removes all and anything
    virtual void Reset() = 0;

    // adds a listener for a particular event
    virtual bool AddListener(IGameEventListener2* listener, const char* name,
                             bool bServerSide) = 0;

    // returns true if this listener is listens to given event
    virtual bool FindListener(IGameEventListener2* listener,
                              const char* name) = 0;

    // removes a listener
    virtual void RemoveListener(IGameEventListener2* listener) = 0;

    // create an event by name, but doesn't fire it. returns NULL is event is
    // not known or no listener is registered for it. bForce forces the creation
    // even if no listener is active
    virtual IGameEvent* CreateEvent(const char* name, bool bForce = false,
                                    int* pCookie = NULL) = 0;

    // fires a server event created earlier, if bDontBroadcast is set, event is
    // not send to clients
    virtual bool FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;

    // fires an event for the local client only, should be used only by client
    // code
    virtual bool FireEventClientSide(IGameEvent* event) = 0;

    // create a new copy of this event, must be free later
    virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;

    // if an event was created but not fired for some reason, it has to bee
    // freed, same UnserializeEvent
    virtual void FreeEvent(IGameEvent* event) = 0;

    // write/read event to/from bitbuffer
    virtual bool SerializeEvent(IGameEvent* event,
                                CMsgSource1LegacyGameEvent* ev) = 0;
    virtual IGameEvent* UnserializeEvent(
        const CMsgSource1LegacyGameEvent&
            ev) = 0;  // create new KeyValues, must be deleted

    virtual int LookupEventId(const char* name) = 0;

    virtual void PrintEventToString(IGameEvent* event, CUtlString& out) = 0;

    virtual bool HasEventDescriptor(const char* name) = 0;
};

class CGameEventManager : public IGameEventManager2 {
   public:  // IGameEventManager functions
    virtual ~CGameEventManager() = 0;

    virtual int LoadEventsFromFile(const char* filename) = 0;
    virtual void Reset() = 0;

    virtual bool AddListener(IGameEventListener2* listener, const char* name,
                             bool bServerSide) = 0;
    virtual bool FindListener(IGameEventListener2* listener,
                              const char* name) = 0;
    virtual void RemoveListener(IGameEventListener2* listener) = 0;

    virtual IGameEvent* CreateEvent(const char* name, bool bForce = false) = 0;
    virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;
    virtual bool FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;
    virtual bool FireEventClientSide(IGameEvent* event) = 0;
};
