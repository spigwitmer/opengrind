#include <GLXW/glxw.h>
#include <GL/glfw3.h>

#include "Nepgear.h"
#include "ScreenManager.h"
#include "screens/Screen.h"
#include "utils/Logger.h"
#include "utils/Timer.h"

using namespace std;

namespace
{
	// Calculate average FPS given a time and delta
	const double UPDATE_INTERVAL = 2.0;

	std::vector<double> times;

	double avg_fps(double time, double delta)
	{
		double avg = 0.0;

		if (!times.empty() && time - times[0] > UPDATE_INTERVAL)
			times.clear();

		// Calculate Average FPS.
		times.push_back(delta);

		for (size_t i = 0; i < times.size(); i++)
			avg += times[i];

		avg /= times.size();

		return 1.0 / avg;
	}
}

MakeScreenMap *ScreenManager::GetMap()
{
	static MakeScreenMap g_ScreenMap;
	return &g_ScreenMap;
}

ScreenManager::ScreenManager()
{
	m_LastUpdateRounded = 0;
	m_LastUpdate = 0.0;
}

ScreenManager::~ScreenManager()
{
	ClearStack();
}

void ScreenManager::Update()
{
	double time = glfwGetTime();
	double delta = time - m_LastUpdate;
	double fps = avg_fps(time, delta);

	if (int(time) % 1 == 0)
	{
		if (m_LastUpdateRounded != int(time))
			LOG->Debug("Avg. FPS: %0.0f", glm::ceil(fps));
		m_LastUpdateRounded = int(time);
	}

	m_LastUpdate = time;

	vector<Screen*>::iterator it = m_vScreenStack.begin();
	for (; it != m_vScreenStack.end(); it++)
		(*it)->Update(delta);
}

void ScreenManager::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	vector<Screen*>::iterator it = m_vScreenStack.begin();
	for (; it != m_vScreenStack.end(); it++)
		(*it)->Draw();
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

		Timer t;

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
