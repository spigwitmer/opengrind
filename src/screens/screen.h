#pragma once

#include <string>
#include <glm/glm.hpp>

#include "actors/actor_base.h"
#include "screens/screen_manager.h"
#include "lua/lua_manager.h"
#include "event/event_manager.h"

class Screen : public EventListener
{
public:
	// Set name on creation.
	Screen(std::string sName);
	virtual ~Screen();
	void Reset();

	void Update(double delta);
	virtual void Draw(DrawBuffer db, glm::vec4 vp);

	// TODO
	void Push(ActorBase* obj);
	
	// Calls Init() on subclasses.
	void InitInternal();

	std::string GetName() const { return m_name; }
	glm::mat4 *GetProjection() { return &m_projection; }

	// For derived classes only.
	virtual void Init();
	virtual void UpdateInternal(double delta);
	virtual bool HandleEvent(const std::string &name, const IEvent &evt);
	
protected:
	bool m_error;
	
	std::string m_name;
	glm::mat4 m_projection;

	std::vector<ActorBase*> m_objects;
};

/**
 * Colby Klein (c) 2013
 * Licensed under the terms of the MIT license. See License.txt.
 */
