#include "../sdk.h"
class IBaseInterface
{
public:
	virtual	~IBaseInterface() {}
};

typedef void* (*InstantiateInterfaceFn)();

// Used internally to register classes.
class InterfaceReg
{
public:
	InterfaceReg(InstantiateInterfaceFn fn, const char* pName);

public:
	InstantiateInterfaceFn	m_CreateFn;
	const char* m_pName;

	InterfaceReg* m_pNext; // For the global list.
};
