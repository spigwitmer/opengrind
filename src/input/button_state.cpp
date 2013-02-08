#include "button_state.h"
#include "utils/logger.h"

ButtonState::ButtonState( unsigned iButton ) : m_iButton( iButton ), m_State( PRESS_NONE )
{
	/* This space intentionally left blank */
}

double ButtonState::GetHeldTime() const
{
	// Test
	if( m_State == PRESS_NONE )
		return 0;
	
	// This is simple, <3 timer
	return m_Timer.Ago();
}

void ButtonState::Press( bool b )
{
	// Pressed?
	if( !b )
	{
		m_State = PRESS_NONE;
		return;
	}
	
	// Now, are we just being pressed?
	switch( m_State )
	{
		case PRESS_NONE:
			m_State = PRESS_PRESSED;
			m_Timer.Touch();
			break;
		
		case PRESS_PRESSED:
			m_State = PRESS_HELD;
			break;
		
		case PRESS_HELD:
			break;
		
		default:
			LOG->Warn( "[ButtonState]  You should never see this :<" );
			break;
	}
}
