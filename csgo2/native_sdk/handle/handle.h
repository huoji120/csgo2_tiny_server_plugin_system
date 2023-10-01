#pragma once
#include <cstdint>
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define ENT_ENTRY_MASK		  0x7FFF
inline int PlayerSlot_to_EntityIndex(int PlayerSlot) { return PlayerSlot + 1; }
inline int EntityIndex_to_PlayerSlot(int EntityIndex) { return EntityIndex - 1; }
class CBaseEntity;

class CHandle
{
	CBaseEntity* GetBaseEntity() const;

public:
	bool operator==(CHandle rhs) const { return m_Index == rhs.m_Index; }

	bool IsValid() const { return m_Index != INVALID_EHANDLE_INDEX; }

	int GetEntryIndex() const { return m_Index & ENT_ENTRY_MASK; }

	template <typename T = CBaseEntity>
	T* Get() const
	{
		return reinterpret_cast<T*>(GetBaseEntity());
	}
	auto GetPlayerSlot() {
		return m_Index - 1;
	}
	uint32_t m_Index;
};