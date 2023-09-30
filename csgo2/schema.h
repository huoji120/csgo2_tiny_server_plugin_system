#pragma once
#include "pch.h"
typedef void(__fastcall* FnNetworkStateChanged)(uintptr_t chainEntity, uintptr_t offset, uintptr_t a3);

#define SCHEMA_FIELD_OFFSET(type, varName, extra_offset)														\
	std::add_lvalue_reference_t<type> varName()																	\
	{																											\
		static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClass);									\
		static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);										\
																												\
		static const auto m_offset =																			\
			schema::GetOffset(ThisClass, datatable_hash, #varName, prop_hash);									\
																												\
		return *reinterpret_cast<std::add_pointer_t<type>>(														\
			(uintptr_t)(this) + m_offset + extra_offset);														\
	}																											\
	void varName(type val)																						\
	{																											\
		static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClass);									\
		static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);										\
																												\
		static const auto m_offset =																			\
			schema::GetOffset(ThisClass, datatable_hash, #varName, prop_hash);									\
																												\
		static const auto m_chain =																				\
			schema::FindChainOffset(ThisClass);																	\
																												\
		if (m_chain != 0)																						\
		{																										\
			reinterpret_cast<FnNetworkStateChanged>(Offset::NetworkStateChangedPtr)((uintptr_t)(this) + m_chain, m_offset + extra_offset, 0xFFFFFFFF);	\
		}																										\
		*reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)(this) + m_offset + extra_offset) = val;			\
	}

#define SCHEMA_FIELD(type, varName) \
	SCHEMA_FIELD_OFFSET(type, varName, 0)

#define PSCHEMA_FIELD_OFFSET(type, varName, extra_offset) \
	auto varName()																\
	{																			\
		static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClass);	\
		static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);		\
																				\
		static const auto m_offset =											\
			schema::GetOffset(ThisClass, datatable_hash, #varName, prop_hash);	\
																				\
		return reinterpret_cast<std::add_pointer_t<type>>(						\
			(uintptr_t)(this) + m_offset + extra_offset);						\
	}

#define PSCHEMA_FIELD(type, varName) \
	PSCHEMA_FIELD_OFFSET(type, varName, 0)

namespace schema
{
	int16_t FindChainOffset(const char* className);
	int16_t GetOffset(const char* className, uint32_t classKey, const char* memberName, uint32_t memberKey);
}

#define DECLARE_CLASS(className) static constexpr auto ThisClass = #className;
