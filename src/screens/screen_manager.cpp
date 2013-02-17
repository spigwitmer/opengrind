#include <GLXW/glxw.h>
#include <GL/glfw3.h>

#include "nepgear.h"
#include "screen_manager.h"
#include "screens/screen.h"
#include "utils/logger.h"
#include "utils/timer.h"

#ifndef _MSC_VER
#include <unistd.h> // usleep
#endif

using namespace std;

namespace
{
	// Calculate average FPS given a time and delta
	int frames = 0;
	int last_fps = -1;
	Nepgear::Timer frametimer;
}

MakeScreenMap *ScreenManager::GetMap()
{
	static MakeScreenMap g_ScreenMap;
	return &g_ScreenMap;
}

ScreenManager::ScreenManager() : m_LastUpdate(0)
{
}

ScreenManager::~ScreenManager()
{
	ClearStack();
}

void ScreenManager::Update(bool focus)
{
#ifndef _MSC_VER
	if (!focus) usleep(1000*75);
#endif

	double time = glfwGetTime();
	double delta = time - m_LastUpdate;

	if (focus)
	{
		if (frametimer.Ago() > 1.0)
		{
			frametimer.Touch();
			last_fps = frames;
			//LOG->Debug("FPS: %d (%0.2fms)", frames, (1.0 / frames)*1000);
			frames = 0;
		}
		frames++;

		// if a frame takes more than twice the average, log it.
		if (last_fps != -1)
		{
			double target = 1.0 / last_fps;
			if (delta > target*2)
			{
				LOG->Debug("Skip: %0.0f (%0.0fms)", delta/target, delta*1000);
			}
		}
	}

	m_LastUpdate = time;

	vector<Screen*>::iterator it = m_vScreenStack.begin();
	for (; it != m_vScreenStack.end(); it++)
		(*it)->Update(delta);
}

void ScreenManager::Draw(StereoscopicMode sm)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glm::vec4 vp = m_renderer->GetViewport();

	vector<Screen*>::iterator it = m_vScreenStack.begin();
	for (; it != m_vScreenStack.end(); it++)
	{
		if (sm == StereoscopicMode_None)
		{
			(*it)->Draw(DrawBuffer_Center);
			continue;
		}
		if (sm == StereoscopicMode_SBS)
		{
			glViewport(0, 0, (int)vp.z, (int)vp.w);
			(*it)->Draw(DrawBuffer_Left);

			glViewport((int)vp.z, 0, (int)vp.z, (int)vp.w);
			(*it)->Draw(DrawBuffer_Right);
			continue;
		}
	}
}

void ScreenManager::PushScreen(const std::string &sName)
{
	lua_State *L = LuaManager::GetSingleton()->Get();

	// Make sure we've got a table to match the given name.
	lua_getglobal(L, sName.c_str());
	if (!lua_istable(L, -1))
	{
		LOG->Error("No screen named \"%s\" found", sName.c_str());
		return;
	}

	// We need the Class field, so that we know what to instantiate.
	lua_getfield(L, -1, "Class");
	if (lua_isstring(L, -1)) {
		string sClass = lua_tostring(L, -1);

		MakeScreenMap::iterator it = GetMap()->find(sClass);

		if (it == GetMap()->end())
		{
			LOG->Error("Invalid screen class: \"%s\"", sClass.c_str());
			return;
		}		
		LOG->Trace("Creating screen \"%s\" [class => \"%s\"]", sName.c_str(), sClass.c_str());

		Nepgear::Timer t;

		Screen* scr = it->second(sName);
		scr->InitInternal();

		LOG->Trace("Loaded \"%s\" in %fs", sName.c_str(), t.Ago());

		m_vScreenStack.push_back(scr);
	}
	else
	{
		LOG->Error("%s has no class! Skipping", sName.c_str());
	}
}

void ScreenManager::PopScreen()
{
	SAFE_DELETE(m_vScreenStack.back());

	m_vScreenStack.pop_back();
}

void ScreenManager::ClearStack()
{
	while (!m_vScreenStack.empty()) PopScreen();

	LOG->Trace("Cleared screen stack");
}

/**
 * Colby Klein, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
