#pragma once

namespace Nepgear
{

class Timer
{
public:
	Timer();
	
	// Reset
	void Touch();
	
	// This returns the difference between now and touch
	double Ago() const;
	
private:
	// The time we were created, or touched
	double m_start;
};

}
