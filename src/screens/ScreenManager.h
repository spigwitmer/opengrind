#pragma once

#include <deque>
#include <vector>
#include <string>
#include <map>
#include "renderer/RenderSystem.h"

class Screen;

// from Tanarux (thanks Matt)
typedef Screen* (*MakeScreenFn)(const std::string &sName);
typedef std::pair<std::string, MakeScreenFn> MakeScreenEntry;
typedef std::map<std::string, MakeScreenFn> MakeScreenMap;

class ScreenManager
{
public:
	ScreenManager();
	virtual ~ScreenManager();

	void Update(bool focus);
	void Draw(StereoscopicMode sm);

	void PushScreen(const std::string &sName);
	void PopScreen();
	void ClearStack();
	
	Screen *GetTopScreen() const { return m_vScreenStack.back(); }
	void SetRenderer(RenderSystem *renderer) { m_renderer = renderer; }

	// Static map stuff, for registering
	static MakeScreenMap *GetMap();

private:
	double m_LastUpdate;
	long m_LastUpdateRounded;
	std::deque<double> m_Times;

	std::vector<Screen*> m_vScreenStack;
	std::map<std::string, Screen*> m_vScreenTypes;
	
	RenderSystem *m_renderer;
	
	// Avoids including GLFW in this file.
	void *m_window;
};

struct RegisterScreen
{
	RegisterScreen(const std::string &sName, MakeScreenFn fn)
	{
		ScreenManager::GetMap()->insert(MakeScreenEntry(sName, fn));
	}
};

#define REGISTER_SCREEN( name ) \
	Screen *Make##name( const string &sName ) { Screen *pRet = new name( sName ); return pRet; } \
	static RegisterScreen g_##name( #name, Make##name );

/**
 * Colby Klein, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
