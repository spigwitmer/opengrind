#pragma once

#include "renderer/render_system.h"
#include <string>
#include "utils/logger.h"

class ShaderProgram;

class RenderSystem_GL30 : public RenderSystem
{
public:
	RenderSystem_GL30() : m_name("GL30") {}
	virtual ~RenderSystem_GL30() {}

	void Init() {}

	void SetUniforms(ShaderProgram *shader);
	void UpdateViewport();

	std::string GetName() const { return m_name; }

protected:
	std::string m_name;
};
