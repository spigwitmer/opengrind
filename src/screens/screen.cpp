#include "screens/screen.h"
#include "utils/logger.h"
#include "lua/lua_manager.h"
#include "input/input_manager.h"
#include "utils/timer.h"

using namespace std;

REGISTER_SCREEN(Screen)

Screen::Screen(string sName) :
	m_error(false),
	m_name(sName),
	m_projection(1.0)
{
	EventManager::Register("ScreenChanged", this);
	EventManager::Register("Input", this);
	EventManager::Register("Resize", this);
}

Screen::~Screen()
{
	LOG->Trace("Deleted screen \"%s\"", m_name.c_str());
}

void Screen::Reset()
{
	m_error = false;
	m_projection = glm::mat4(1.0);
	Init();
}

// Use Init() so that the timer works properly!
void Screen::InitInternal()
{
	lua_State *L = LuaManager::GetSingleton()->Get();
	lua_getglobal(L, m_name.c_str());

	if (!lua_istable(L, -1))
		return;

	// note: lua_next needs the stack to be: -1 = key(nil), -2 = table
	lua_pushnil(L);

	while (lua_next(L, -2)) {
		// -1 = value, -2 = key, -3 = table
		//if (lua_isnumber(L, -2))
		//	LOG->Info("oh hey %d=%d", (int)lua_tonumber(L, -2), (int)lua_tonumber(L, -1));

		lua_pop(L, 1);
	}

	Init();
}

void Screen::Init()
{
}

bool Screen::HandleEvent(const string &name, const IEvent &evt)
{
	if (name == "Input") {
		Event<Nepgear::InputManager*> evt = (Event<Nepgear::InputManager*>)evt;
		while (evt.data->GetNextInput()) {
			LOG->Debug("foo");
		}
	}

	LOG->Trace("Got message: %s", name.c_str());
	return true;
}

void Screen::Push(ActorBase* obj)
{
	m_objects.push_back(obj);
}

void Screen::UpdateInternal(double delta)
{
}

void Screen::Update(double delta)
{
#if 0
	// XXX: Crash
	if (!m_error)
	{
		// (Update stuff)
		lua_State *L = LuaManager::GetSingleton()->Get();
		
		// one of these isn't working - debug here.
		lua_getglobal(L, m_name.c_str());
		lua_getfield(L, -1, "Update");
		
		if (lua_isfunction(L, -1))
		{
			LOG->Debug("is function");
			int err = lua_pcall(L, 0, 0, 0);
			if (err)
			{
				m_error = true;
				LOG->Warn("%s", lua_tostring(L, -1));
				lua_pop(L, 1);
			}
		}
	}
#endif
	UpdateInternal(delta);
}

void Screen::Draw(DrawBuffer db)
{
}

/**
 * Colby Klein, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
