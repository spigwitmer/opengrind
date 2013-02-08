#include "lua_manager.h"
#include "lua_binding.h"
#include "utils/logger.h"
#include <cstring>

using namespace std;

void RawGet(int pos, int count)
{
	for(int i = 1 ; i <= count ; i++)
		lua_rawgeti(LUA, pos, i);
}

LuaManager::LuaManager()
{
	// Initialize the state
	m_pLua = lua_open();

	luaL_openlibs(m_pLua);
}

LuaManager::~LuaManager()
{
	lua_close(m_pLua);
}

int LuaManager::Load(string buf, string name, unsigned args, unsigned rets)
{
	return Load(buf.c_str(), buf.length(), name.c_str(), args, rets);
}

int LuaManager::Load(const char *sBuf, unsigned iSize, const char *sName, unsigned iArgs, unsigned iRets)
{
	int iRet = luaL_loadbuffer(m_pLua, sBuf, iSize, sName);

	if(iRet != 0)
	{
		LOG->Error("Error loading %s: %s", sName, lua_tostring(m_pLua, -1));
		lua_pop(m_pLua, 1);
		return iRet;
	}

	// Execute
	iRet = lua_pcall(m_pLua, iArgs, iRets, 0);

	if(iRet != 0)
	{
		LOG->Error("Error running %s: %s", sName, lua_tostring(m_pLua, -1));
		lua_pop(m_pLua, 1);
		return iRet;
	}

	LOG->Trace("Loaded \"%s\"", sName);

	return 0;
}

void LuaManager::Register(LuaRegisterFn fn)
{
	// Add this registration function to the queue
	m_vRegisters.push_back(fn);
}

void LuaManager::RegisterAll()
{
	// Call every lua->Register() function
	for(unsigned i = 0 ; i < m_vRegisters.size() ; i++)
		m_vRegisters[i]();
	
	// Clear it, incase of reinit maybe...
	m_vRegisters.clear();
}
