#pragma once

#include "utils/Singleton.h"
#include <vector>
#include <string>

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

typedef void (*LuaRegisterFn)();

class LuaManager : public Singleton<LuaManager>
{
public:
	// Construction, destruction
	LuaManager();
	~LuaManager();

	// Grabber for our main state
	lua_State *Get() const { return m_pLua; }

	// Loads a chunk, and runs it
	int Load(const char *sBuf, unsigned iSize, const char *sName, unsigned iArgs = 0, unsigned iRet = 0);
	int Load(std::string buf, std::string name, unsigned args = 0, unsigned ret = 0);

	// Register a class to be created
	void Register(LuaRegisterFn fn);
	void RegisterAll();

	// Lua stuff (no really)
	void PushSelf();

private:
	// Our main state
	lua_State *m_pLua;

	// The list of stuff to register
	std::vector<LuaRegisterFn> m_vRegisters;
};

// Helper function
void RawGet(int iPos, int iCount);
