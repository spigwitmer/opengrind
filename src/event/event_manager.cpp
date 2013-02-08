#include "event_manager.h"

using namespace std;

std::map<std::string, std::set<EventListener*> > m_Listeners;

typedef map<string, set<EventListener*> >::iterator ListenerIterator;
typedef set<EventListener*>::iterator ListenerSetIterator;

bool EventManager::Broadcast(const string &name, const IEvent &ev)
{
	set<EventListener*> &data = m_Listeners[name];

	for (ListenerSetIterator it = data.begin(); it != data.end(); it++)
	{
		if (!(*it)->HandleEvent(name, ev))
			return false;
	}

	return true;
}

void EventManager::Register(const string &name, EventListener *listener)
{
	m_Listeners[name].insert(listener);
}

void EventManager::Unregister(const string &name, EventListener *listener)
{
	m_Listeners[name].erase(listener);
}

void EventManager::Clear(EventListener *listener)
{
	ListenerIterator it;
	for(it = m_Listeners.begin(); it != m_Listeners.end(); it++)
	{
		(*it).second.erase(listener);
	}
}

void EventManager::ClearAll()
{
	m_Listeners.clear();
}
