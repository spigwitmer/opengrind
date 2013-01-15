#pragma once

// Vyhd magic
class IEvent
{
public:
	IEvent() { }
	virtual ~IEvent() { }
};

template<class T>
class Event : public IEvent
{
public:
	Event(T mydata = T()) : data(mydata) { }
	virtual ~Event() { }

	T data;
};
