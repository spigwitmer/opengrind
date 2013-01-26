#pragma once

#include <GLXW/glxw.h>
#include <string>
#include "utils/Logger.h"
#include <glm/glm.hpp>
#include "Stereoscopic.h"

class ShaderProgram;

enum BufferType
{
	VERTEX_BUFFER = 0,
	INDEX_BUFFER,
	UNIFORM_BUFFER
};

class RenderSystem
{
public:
	RenderSystem() {}
	virtual ~RenderSystem() {}

	virtual void Init() { }

	virtual void GenerateBuffers(int amount, GLuint *buffers) {}
	virtual void Upload(GLuint buffer, BufferType type, size_t size, void* ptr) {}
	virtual void SetUniforms(ShaderProgram *shader) {}
	virtual void UpdateViewport() {}

	/* Unscaled viewport goes in, scaled viewport comes out (for 3D). */
	void SetViewport(glm::vec4);
	glm::vec4 GetViewport();

	virtual std::string GetName() const { return m_name; }

protected:
	std::string m_name;
	glm::vec4 m_viewport;
};

