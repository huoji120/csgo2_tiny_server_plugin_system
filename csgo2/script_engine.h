#pragma once
#include "head.h"
namespace ScriptEngine {
	auto Init() -> void;
	auto callFunction(const char* funcName) -> int;
}