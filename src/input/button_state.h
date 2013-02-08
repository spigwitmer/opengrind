#pragma once

#include "utils/timer.h"

enum PressType
{
	PRESS_NONE,
	PRESS_PRESSED,
	PRESS_HELD,
	NUM_PRESS
};

class ButtonState
{
public:
	// Construction
	ButtonState( unsigned iButton );
	
	// Button test
	unsigned GetButton() const	{ return m_iButton; }
	
	// Are we pressed?
	bool IsDown() const		{ return ( m_State == PRESS_PRESSED || m_State == PRESS_HELD ); }
	bool IsPressed() const		{ return m_State == PRESS_PRESSED; }
	bool IsHeld() const		{ return m_State == PRESS_HELD; }
	PressType GetState() const	{ return m_State; }
	
	// Press it
	void Press( bool b );
	
	// Time since Press( true )
	double GetHeldTime() const;
	
private:
	// The button we are
	const unsigned m_iButton;
	
	// The pressed variable
	PressType m_State;
	
	// Our timer
	Timer m_Timer;
};
