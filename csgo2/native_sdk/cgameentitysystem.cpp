#include "cgameentitysystem.h"
#include "../pch.h"

CGameEntitySystem* CGameEntitySystem::GetInstance(){
	return Offset::InterFaces::GameResourceServiceServer->GetGameEntitySystem();
}
