#pragma once

template <class T>
struct Singleton
{
	static T *GetSingleton()
	{
		static T ptr;
		return &ptr;
	}
};
