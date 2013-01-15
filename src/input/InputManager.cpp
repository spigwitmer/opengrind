#include "InputManager.h"
#include "utils/Logger.h"
#include "events/EventManager.h"

#include <vector>
#include <GL/glfw3.h>

#include <GL/glfw3.h>

// Translate GLFW keys to our own.
static inline RSButtons TranslateGLFW(int i)
{
	switch(i)
	{
		case GLFW_KEY_ESCAPE:		return RS_KEY_ESC;
		case GLFW_KEY_F1:			return RS_KEY_F1;
		case GLFW_KEY_F2:			return RS_KEY_F2;
		case GLFW_KEY_F3:			return RS_KEY_F3;
		case GLFW_KEY_F4:			return RS_KEY_F4;
		case GLFW_KEY_F5:			return RS_KEY_F5;
		case GLFW_KEY_F6:			return RS_KEY_F6;
		case GLFW_KEY_F7:			return RS_KEY_F7;
		case GLFW_KEY_F8:			return RS_KEY_F8;
		case GLFW_KEY_F9:			return RS_KEY_F9;
		case GLFW_KEY_F10:			return RS_KEY_F10;
		case GLFW_KEY_F11:			return RS_KEY_F11;
		case GLFW_KEY_F12:			return RS_KEY_F12;
		case GLFW_KEY_F13:			return RS_KEY_F13;
		case GLFW_KEY_F14:			return RS_KEY_F14;
		case GLFW_KEY_F15:			return RS_KEY_F15;
		case GLFW_KEY_LEFT:			return RS_KEY_LEFT;
		case GLFW_KEY_DOWN:			return RS_KEY_DOWN;
		case GLFW_KEY_RIGHT:		return RS_KEY_RIGHT;
		case GLFW_KEY_UP:			return RS_KEY_UP;
		case GLFW_KEY_LEFT_SHIFT:	return RS_KEY_SHIFT_1;
		case GLFW_KEY_RIGHT_SHIFT:	return RS_KEY_SHIFT_1;
		case GLFW_KEY_LEFT_CONTROL:	return RS_KEY_CTRL_1;
		case GLFW_KEY_RIGHT_CONTROL:return RS_KEY_CTRL_2;
		case GLFW_KEY_LEFT_ALT:		return RS_KEY_ALT_1;
		case GLFW_KEY_RIGHT_ALT:	return RS_KEY_ALT_2;
		case GLFW_KEY_LSUPER:		return RS_KEY_SUPER_1;
		case GLFW_KEY_RSUPER:		return RS_KEY_SUPER_2;
		case GLFW_KEY_TAB:			return RS_KEY_TAB;
		case GLFW_KEY_ENTER:		return RS_KEY_ENTER;
		case GLFW_KEY_BACKSPACE:	return RS_KEY_BACKSPACE;
		case GLFW_KEY_INSERT:		return RS_KEY_INSERT;
		case GLFW_KEY_DEL:			return RS_KEY_DELETE;
		case GLFW_KEY_PAGEUP:		return RS_KEY_PGUP;
		case GLFW_KEY_PAGEDOWN:		return RS_KEY_PGDOWN;
		case GLFW_KEY_HOME:			return RS_KEY_HOME;
		case GLFW_KEY_END:			return RS_KEY_END;
		case GLFW_KEY_KP_0:			return RS_KEY_KP_0;
		case GLFW_KEY_KP_1:			return RS_KEY_KP_1;
		case GLFW_KEY_KP_2:			return RS_KEY_KP_2;
		case GLFW_KEY_KP_3:			return RS_KEY_KP_3;
		case GLFW_KEY_KP_4:			return RS_KEY_KP_4;
		case GLFW_KEY_KP_5:			return RS_KEY_KP_5;
		case GLFW_KEY_KP_6:			return RS_KEY_KP_6;
		case GLFW_KEY_KP_7:			return RS_KEY_KP_7;
		case GLFW_KEY_KP_8:			return RS_KEY_KP_8;
		case GLFW_KEY_KP_9:			return RS_KEY_KP_9;
		case GLFW_KEY_KP_DIVIDE:	return RS_KEY_KP_SLASH;
		case GLFW_KEY_KP_MULTIPLY:	return RS_KEY_KP_ASTERISK;
		case GLFW_KEY_KP_SUBTRACT:	return RS_KEY_KP_HYPHEN;
		case GLFW_KEY_KP_ADD:		return RS_KEY_KP_PLUS;
		case GLFW_KEY_KP_DECIMAL:	return RS_KEY_KP_PERIOD;
		case GLFW_KEY_KP_ENTER:		return RS_KEY_KP_ENTER;

		case 'A':	return RS_KEY_A;
		case 'B':	return RS_KEY_B;
		case 'C':	return RS_KEY_C;
		case 'D':	return RS_KEY_D;
		case 'E':	return RS_KEY_E;
		case 'F':	return RS_KEY_F;
		case 'G':	return RS_KEY_G;
		case 'H':	return RS_KEY_H;
		case 'I':	return RS_KEY_I;
		case 'J':	return RS_KEY_J;
		case 'K':	return RS_KEY_K;
		case 'L':	return RS_KEY_L;
		case 'M':	return RS_KEY_M;
		case 'N':	return RS_KEY_N;
		case 'O':	return RS_KEY_O;
		case 'P':	return RS_KEY_P;
		case 'Q':	return RS_KEY_Q;
		case 'R':	return RS_KEY_R;
		case 'S':	return RS_KEY_S;
		case 'T':	return RS_KEY_T;
		case 'U':	return RS_KEY_U;
		case 'V':	return RS_KEY_V;
		case 'W':	return RS_KEY_W;
		case 'X':	return RS_KEY_X;
		case 'Y':	return RS_KEY_Y;
		case 'Z':	return RS_KEY_Z;
		case '1':	return RS_KEY_1;
		case '2':	return RS_KEY_2;
		case '3':	return RS_KEY_3;
		case '4':	return RS_KEY_4;
		case '5':	return RS_KEY_5;
		case '6':	return RS_KEY_6;
		case '7':	return RS_KEY_7;
		case '8':	return RS_KEY_8;
		case '9':	return RS_KEY_9;
		case '0':	return RS_KEY_0;
		case '`':	return RS_KEY_GRAVE;
		case '-':	return RS_KEY_HYPHEN;
		case '=':	return RS_KEY_EQUALS;
		case '[':	return RS_KEY_LEFT_SQ_BRACKET;
		case ']':	return RS_KEY_RIGHT_SQ_BRACKET;
		case '\\':	return RS_KEY_BACK_SLASH;
		case ';':	return RS_KEY_SEMICOLON;
		case '\'':	return RS_KEY_APOSTROPHE;
		case ',':	return RS_KEY_COMMA;
		case '.':	return RS_KEY_PERIOD;
		case '/':	return RS_KEY_FORWARD_SLASH;
		case ' ':	return RS_KEY_SPACE;

		case GLFW_MOUSE_BUTTON_1:	return RS_MOUSE_1;
		case GLFW_MOUSE_BUTTON_2:	return RS_MOUSE_2;
		case GLFW_MOUSE_BUTTON_3:	return RS_MOUSE_3;
		case GLFW_MOUSE_BUTTON_4:	return RS_MOUSE_4;
		case GLFW_MOUSE_BUTTON_5:	return RS_MOUSE_5;
		case GLFW_MOUSE_BUTTON_6:	return RS_MOUSE_6;
		case GLFW_MOUSE_BUTTON_7:	return RS_MOUSE_7;
		case GLFW_MOUSE_BUTTON_8:	return RS_MOUSE_8;

		default:
			break;
	}
	return RS_KEY_INVALID;
}

// Our internal list of buttons
static ButtonState *g_pButtons[RS_INPUT_MAX];

// Our static queue, and its pointer
static std::vector<ButtonState*> g_vQueue;
static unsigned g_iQueuePtr = 0;

// The mouse position
static glm::ivec2 g_iMousePos;

// Define our callbacks
static void KeyboardCallback(GLFWwindow*, int, int);
static void MouseButtonCallback(GLFWwindow*, int, int);
static void MousePosCallback(GLFWwindow*, int, int);
static void MouseScrollCallback(GLFWwindow*, double, double);

InputManager::InputManager()
{
	// Initialize all our buttons
	for (unsigned i = 0; i < RS_INPUT_MAX; i++)
		g_pButtons[i] = new ButtonState(i);
	
	// Reserve 16 slots in the queue initially
	g_vQueue.reserve(16);
}

void InputManager::connect(GLFWwindow *w)
{
	m_window = w;
	// Set the callbacks up
	glfwSetKeyCallback(w, KeyboardCallback);
	glfwSetMouseButtonCallback(w, MouseButtonCallback);
	glfwSetCursorPosCallback(w, MousePosCallback);
	glfwSetScrollCallback(w, MouseScrollCallback);
}

InputManager::~InputManager()
{	
	DiscardQueue();
	
	// Deallocate the buttons
	for (unsigned i = 0; i < RS_INPUT_MAX; i++)
	{
		delete g_pButtons[i];
		g_pButtons[i] = NULL;
	}
}

void InputManager::DiscardQueue()
{
	g_vQueue.clear();
	g_iQueuePtr = 0;
}

void InputManager::Update()
{
	DiscardQueue();
	glfwPollEvents();
	
	if (!g_vQueue.empty()) {
		Event<InputManager*> m;
		m.data = this;
		EventManager::Broadcast("Input", (IEvent)m);
	}
}

ButtonState *InputManager::GetNextInput()
{
	if (g_vQueue.empty())
		return NULL;
	
	if (g_iQueuePtr >= g_vQueue.size())
		return NULL;
	
	return g_vQueue[g_iQueuePtr++];
}

ButtonState *InputManager::GetButton(RSButtons b) const
{
	if (b >= RS_INPUT_MAX)
		return NULL;
	
	return g_pButtons[b];
}

glm::ivec2 &InputManager::GetMousePos() const
{
	// Just get the position
	return g_iMousePos;
}

static void KeyboardCallback(GLFWwindow*, int iButton, int iDown)
{
	// Translate the button
	RSButtons b = TranslateGLFW(iButton);
	
	if (b == RS_KEY_INVALID)
	{
		LOG->Warn("[Input] Unknown GLFW Key '%i'", iButton);
		return;
	}
	
	g_pButtons[b]->Press(!!iDown);
	g_vQueue.push_back(g_pButtons[b]);
}

// Same thing, but for mouse.
static void MouseButtonCallback(GLFWwindow*, int iButton, int iDown)
{
	RSButtons b = TranslateGLFW(iButton);
	
	if (b == RS_KEY_INVALID)
	{
		LOG->Warn("[Input] Unknown GLFW Mouse button '%i'", iButton);
		return;
	}
	
	g_pButtons[b]->Press(!!iDown);	
	g_vQueue.push_back(g_pButtons[b]);
}

static void MousePosCallback(GLFWwindow*, int iPosX, int iPosY)
{
	g_iMousePos.x = iPosX;
	g_iMousePos.y = iPosY;
}

static void MouseScrollCallback(GLFWwindow*, double iHorizontal, double iVertical)
{
	/* Okay, we will press it, and add it to the queue...But it doesn't
	 * get unpressed */
	if (iHorizontal > 0)
	{
		g_pButtons[RS_SCROLL_RIGHT]->Press(true);
		g_vQueue.push_back(g_pButtons[RS_SCROLL_RIGHT]);
	}
	else if (iHorizontal < 0)
	{
		g_pButtons[RS_SCROLL_LEFT]->Press(true);
		g_vQueue.push_back(g_pButtons[RS_SCROLL_LEFT]);
	}
	
	if (iVertical > 0)
	{
		g_pButtons[RS_SCROLL_UP]->Press(true);
		g_vQueue.push_back(g_pButtons[RS_SCROLL_UP]);
	}
	else if (iVertical < 0)
	{
		g_pButtons[RS_SCROLL_DOWN]->Press(true);
		g_vQueue.push_back(g_pButtons[RS_SCROLL_DOWN]);
	}
}

/**
 * Colby Klein, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
