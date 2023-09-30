#pragma once
#include "../pch.h"
#include "../vmt.h"

struct SchemaClassFieldData_t
{
	const char *m_name;
	char pad0[0x8];
	short m_offset;
	char pad1[0xE];
};

class SchemaClassInfoData_t;

struct SchemaBaseClassInfoData_t
{
	unsigned int m_offset;
	SchemaClassInfoData_t *m_class;
};

class SchemaClassInfoData_t
{
public:
	auto GetName()
	{
		return m_name;
	}

	auto GetFieldsSize()
	{
		return m_align;
	}

	auto GetFields()
	{
		return m_fields;
	}

	auto GetParent()
	{
		return m_schema_parent->m_class;
	}

private:
	char pad_0x0000[0x8];						// 0x0000

	const char *m_name;							// 0x0008
	char *m_module;								// 0x0010

	int m_size;									// 0x0018
	std::int16_t m_align;						// 0x001C

	std::int16_t m_static_size;					// 0x001E
	std::int16_t m_metadata_size;				// 0x0020
	std::int16_t m_i_unk1;						// 0x0022
	std::int16_t m_i_unk2;						// 0x0024
	std::int16_t m_i_unk3;						// 0x0026

	SchemaClassFieldData_t *m_fields;			// 0x0028

	char pad_0x0030[0x8];						// 0x0030
	SchemaBaseClassInfoData_t *m_schema_parent; // 0x0038

	char pad_0x0038[0x10];						// 0x0038
};

class CSchemaSystemTypeScope
{
public:
	auto FindDeclaredClass(const char* pClass) -> SchemaClassInfoData_t*;
};

class CSchemaSystem
{
public:
	auto FindTypeScopeForModule(const char* module) ->CSchemaSystemTypeScope*;
};