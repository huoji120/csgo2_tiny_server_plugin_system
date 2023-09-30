#include "cschemasystem.h"

auto CSchemaSystemTypeScope::FindDeclaredClass(const char* pClass) -> SchemaClassInfoData_t*
{
	SchemaClassInfoData_t* rv = nullptr;
	CALL_VIRTUAL(void, 2, this, &rv, pClass);
	return rv;
}

auto CSchemaSystem::FindTypeScopeForModule(const char* module) -> CSchemaSystemTypeScope*
{
	return CALL_VIRTUAL(CSchemaSystemTypeScope*, 13, this, module, nullptr);
}