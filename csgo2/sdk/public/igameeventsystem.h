#include "../sdk.h"
class IAppSystem;
class CUtlSlot;
class CUtlAbstractDelegate;
class CBaseHandle;
typedef uint16 NetworkMessageId;
typedef uint8 NetworkGroupId;
typedef uint NetworkCategoryId;

enum NetChannelBufType_t
{
	BUF_RELIABLE = 0,
	BUF_UNRELIABLE,
	BUF_VOICE,
};
enum NetworkValidationMode_t
{
	kFuckOffAss_NetworkValidationMode
};
class IProtobufBinding
{
public:
	virtual const char* GetName() = 0;
	virtual int GetSize() = 0;

	virtual const char* ToString(const void* pData, CUtlString& sResult) = 0;

	virtual const char* GetGroup() = 0;
	virtual Color GetGroupColor() = 0;
	virtual NetChannelBufType_t GetBufType() = 0;

	virtual bool ReadFromBuffer(void* pData, bf_read& pBuf) = 0;
	virtual bool WriteToBuffer(const void* pData, bf_write& pBuf) = 0;

	virtual void* AllocateMessage() = 0;
	virtual void DeallocateMessage(void* pMsg) = 0;
	virtual void* AllocateAndCopyConstructNetMessage(const void* pOther) = 0;

	virtual bool OkToRedispatch() = 0;
	virtual void Copy(const void* pFrom, void* pTo) = 0;
	virtual bool unk001() = 0;
};

struct NetMessageInfo_t
{
	int m_nCategories;
	IProtobufBinding* m_pBinding;
	CUtlString m_szGroup;
	NetworkMessageId m_MessageId;
	NetworkGroupId m_GroupId;

	// (1 << 0) - FLAG_RELIABLE
	// (1 << 6) - FLAG_AUTOASSIGNEDID
	// (1 << 7) - FLAG_UNK001
	uint8 m_nFlags;

	int m_unk001;
	int m_unk002;
	bool m_bOkayToRedispatch;
};
enum NetworkSerializationMode_t
{
	NET_SERIALIZATION_MODE_0 = 0x0,
	NET_SERIALIZATION_MODE_1 = 0x1,
	NET_SERIALIZATION_MODE_COUNT = 0x2,
	NET_SERIALIZATION_MODE_DEFAULT = 0x0,
	NET_SERIALIZATION_MODE_SERVER = 0x0,
	NET_SERIALIZATION_MODE_CLIENT = 0x1,
};
enum ETEProtobufIds : int {
	TE_EffectDispatchId = 400,
	TE_ArmorRicochetId = 401,
	TE_BeamEntPointId = 402,
	TE_BeamEntsId = 403,
	TE_BeamPointsId = 404,
	TE_BeamRingId = 405,
	TE_BSPDecalId = 407,
	TE_BubblesId = 408,
	TE_BubbleTrailId = 409,
	TE_DecalId = 410,
	TE_WorldDecalId = 411,
	TE_EnergySplashId = 412,
	TE_FizzId = 413,
	TE_ShatterSurfaceId = 414,
	TE_GlowSpriteId = 415,
	TE_ImpactId = 416,
	TE_MuzzleFlashId = 417,
	TE_BloodStreamId = 418,
	TE_ExplosionId = 419,
	TE_DustId = 420,
	TE_LargeFunnelId = 421,
	TE_SparksId = 422,
	TE_PhysicsPropId = 423,
	TE_PlayerDecalId = 424,
	TE_ProjectedDecalId = 425,
	TE_SmokeId = 426,
	TE_BulletHold = 452
};
class INetworkSerializable
{
public:
	virtual ~INetworkSerializable() = 0;

	virtual const char *GetUnscopedName() = 0;
	virtual NetMessageInfo_t *GetNetMessageInfo() = 0;

	virtual void SetMessageId(unsigned short nMessageId) = 0;

	virtual void AddCategoryMask(int nMask, bool) = 0;

	virtual void SwitchMode(NetworkValidationMode_t nMode) = 0;

	virtual void *AllocateMessage() = 0;
	virtual void DeallocateMessage(void *pMsg) = 0;
	virtual void *AllocateAndCopyConstructNetMessage(void const *pOther) = 0;

	virtual bool Serialize(bf_write &pBuf, void const *pData, NetworkSerializationMode_t unused) = 0;
	virtual bool Unserialize(bf_read &pBuf, void *pData, NetworkSerializationMode_t unused) = 0;
};

class IGameEventSystem : public IAppSystem
{
public:
	//vmt index 11
	virtual void RegisterGameEvent(INetworkSerializable * pEvent) = 0;

	virtual void RegisterGameEventHandlerAbstract(CUtlSlot* nSlot, const CUtlAbstractDelegate& delegate, INetworkSerializable* pEvent) = 0;
	virtual void UnregisterGameEventHandlerAbstract(CUtlSlot* nSlot, const CUtlAbstractDelegate& delegate, INetworkSerializable* pEvent) = 0;

	// Providing nSize has no effect and is unused.
	virtual void PostEventAbstract_Local(CSplitScreenSlot nSlot, INetworkSerializable* pEvent, const void* pData, unsigned long nSize) = 0;

	// Providing nSlot as -1 would select 0nth slot.
	// clients pointer is a masked uint64 value where (client index - 1) is mapped to each bit.
	// Providing nClientCount as -1 and clients pointer as NULL would post event to all available clients.
	// Providing nSize has no effect and is unused.
	virtual void PostEventAbstract(CSplitScreenSlot nSlot, bool bLocalOnly, int nClientCount, const uint64* clients,
		INetworkSerializable* pEvent, const void* pData, unsigned long nSize, NetChannelBufType_t bufType) = 0;
	virtual void PostEventAbstract(CSplitScreenSlot nSlot, bool bLocalOnly, IRecipientFilter* pFilter,
		INetworkSerializable* pEvent, const void* pData, unsigned long nSize) = 0;

	// Posts the event to all clients, even tho the function name tells otherwise
	// Providing nSize has no effect and is unused.
	virtual void PostEntityEventAbstract(const CBaseHandle& hndl, INetworkSerializable* pEvent, const void* pData, unsigned long nSize, NetChannelBufType_t bufType) = 0;

	virtual void ProcessQueuedEvents() = 0;
	virtual int GetEventSource() const = 0;
	virtual void PurgeQueuedEvents() = 0;
};