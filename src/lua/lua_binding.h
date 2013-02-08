#pragma once

#include "lua_manager.h"
#include <vector>

// The registration type, class name, member function
template <typename T>
struct RegType
{
	const char *sName;
	int (*memberfn)( T *pClass );
};

// Convenience
#define LUA	LuaManager::GetSingleton()->Get()
#define ToStr(i) ( (const char *)luaL_checkstring( LUA, i ) )
#define ToInt(i) ( (int)luaL_checknumber( LUA, i ) )
#define ToFloat(i) ( (float)luaL_checknumber( LUA, i ) )
#define ToBool(i) ( !!lua_toboolean( LUA, i ) )

template <typename T>
class Lunar
{
	// Convenience
	typedef struct { T *pT; } userdataType;
	
public:
	~Lunar()
	{
		// Clean up the methods vector
		delete s_pvMethods;
		s_pvMethods = NULL;
	}
	
	// The main registration of a class
	static void Register()
	{
		// Create the main table for this class
		lua_newtable(LUA);
		int iMethodsPos = lua_gettop(LUA);
		
		// Do the same, but for the metatable
		luaL_newmetatable(LUA, m_sClassName);
		int iMetaPos = lua_gettop(LUA);
		
		// Push the class to the global index
		lua_pushvalue(LUA, iMethodsPos);
		Set(LUA_GLOBALSINDEX, m_sClassName);
		
		// Attach metatable
		lua_pushvalue(LUA, iMethodsPos);
		Set(iMetaPos, "__metatable");
		
		// Attach the __index
		lua_pushvalue(LUA, iMethodsPos);
		Set(iMetaPos, "__index");
		
		// Attach the __index
		lua_pushvalue(LUA, iMethodsPos);
		Set(iMetaPos, "__newindex");
		
		// Now give it a tostring()
		lua_pushcfunction(LUA, ToString);
		Set(iMetaPos, "__tostring");
		
		// Fill in all the methods for this class...
		for (unsigned i = 0 ; i < s_pvMethods->size() ; i++)
		{
			// The regtype
			const MyRegType *l = &(*s_pvMethods)[i];
			
			// Push the name of the function
			lua_pushstring(LUA, l->sName);
			
			// Push the function itself, and attach it to the right table
			lua_pushlightuserdata(LUA, (void *)l);
			lua_pushcclosure(LUA, Thunk, 1);
			lua_settable(LUA, iMethodsPos);
		}
		
		lua_pop(LUA, 2);
	}
	
	// Get userdata from Lua stack, and return pointer to T
	static T *Check(int iArg)
	{
		// We test this out
		userdataType *ud = static_cast<userdataType*>( luaL_checkudata( LUA, iArg, m_sClassName ) );
		
		// If it's NULL, oh crap
		if(ud == NULL)
		{
			luaL_typerror(LUA, iArg, m_sClassName);
			return NULL;
		}
		
		// Return a pointer to this class
		return ud->pT;
	}
	
	// Push a class as a singleton onto the lua stack
	static int Push(T *p)
	{
		// This class as a userdata type
		userdataType *ud = static_cast<userdataType*>(lua_newuserdata(LUA, sizeof(userdataType)));
		ud->pT = p;
		
		// Get the metatable for this class, and set it
		luaL_getmetatable(LUA, m_sClassName);
		lua_setmetatable(LUA, -2);
		return 1;
	}
	
	// Create the methods vector
	static void CreateMethodsVector()
	{
		if(s_pvMethods == NULL)
			s_pvMethods = new RegTypeVector;
	}
	
public:
	// This class name
	static const char *m_sClassName;
	
	// Convenience
	typedef RegType<T> MyRegType;
	typedef std::vector<MyRegType> RegTypeVector;
	
	// The methods vector
	static RegTypeVector *s_pvMethods;
	
private:
	// Member function dispatch
	static int Thunk(lua_State *L)
	{
		// Self/this
		T *pObj = Check(1);
		
		// Remove self, args start at 1
		lua_remove(L, 1);
		
		// Get the member function from upvalue
		MyRegType *l = static_cast<MyRegType*>(lua_touserdata(L, lua_upvalueindex(1)));
		
		// Sanity!
		if(l == NULL)
			return 0;
		
		// Call the function
		return (*(l->memberfn))(pObj);
	}
	
	static void Set(int iIndex, const char *sKey)
	{
		lua_pushstring(LUA, sKey);
		lua_insert(LUA, -2);
		lua_settable(LUA, iIndex);
	}
	
	static int ToString(lua_State *L)
	{
		// The buffer to store into
		char sBuf[32];
		
		// Cast to this type
		userdataType *pData = static_cast<userdataType*>(lua_touserdata(L, 1));
		
		// Sanity
		if(pData == NULL)
		{
			lua_pushstring(L, "NULL");
			return 1;
		}
		
		// Get the object
		T *pObj = pData->pT;
		
		// Set it
		snprintf(sBuf, 32, "%p", (void *)pObj);
		
		// Send it out
		lua_pushfstring(L, "%s (%s)", m_sClassName, sBuf);
		return 1;
	}
};

// I like a good excuse to code with meth :awesome:
#define LUA_ADD_METHOD(meth) \
	{ \
		Lunar<T>::CreateMethodsVector(); \
		RegType<T> r = { #meth, meth }; \
		Lunar<T>::s_pvMethods->push_back( r ); \
	}

#define LUA_REGISTER_CLASS( T ) \
	static Lua##T<T> register##T; \
	template<> const char *Lunar<T>::m_sClassName = #T; \
	template<> Lunar<T>::RegTypeVector *Lunar<T>::s_pvMethods = NULL; \
	void T::PushSelf() { Lua##T<T>::Push( this ); }
