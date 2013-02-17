//#include <GLXW/glxw.h>
#include "screen_gameplay.h"
#include "utils/logger.h"
#include "input/input_manager.h"

using namespace std;

REGISTER_SCREEN(ScreenGameplay)

ScreenGameplay::ScreenGameplay(string name) : Screen(name)
{
}

void ScreenGameplay::Init()
{
	
}

ScreenGameplay::~ScreenGameplay()
{
}

bool ScreenGameplay::HandleEvent(const string &name, const IEvent &evt)
{
	if (name == "Input")
	{
		Nepgear::InputManager *im = Nepgear::InputManager::GetSingleton();
		if (im->GetButton(RS_KEY_SPACE)->IsDown())
			LOG->Debug("the final frontier");
	}

	return true;
}

void ScreenGameplay::UpdateInternal(double delta)
{
}

void ScreenGameplay::Draw(DrawBuffer db)
{
}
