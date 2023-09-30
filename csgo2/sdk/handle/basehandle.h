#pragma once
#include "../sdk.h"

class CEntityHandle
{
public:
	union
	{
		uint32_t m_Index;
		struct
		{
			uint32_t m_EntityIndex : 15;
			uint32_t m_Serial : 17;
		} m_Parts;
	};
};
