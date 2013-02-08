#pragma once

#include "event.h"
#include <string>
#include <map>
#include <set>

class EventListener
{
public:
	virtual ~EventListener() {}
	virtual bool HandleEvent(const std::string &name, const IEvent &ev) { return false; }
};

namespace EventManager
{
	bool Broadcast(const std::string &name, const IEvent &ev = IEvent());
	void Register(const std::string &name, EventListener *listener);
	void Unregister(const std::string &name, EventListener *listener);
	void Clear(EventListener *listener);
	void ClearAll();
}
