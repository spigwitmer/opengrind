#pragma once

#include <GLXW/glxw.h>
#include <string>
#include "utils/Logger.h"
#include <glm/glm.hpp>

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

	virtual std::string GetName() const { return m_name; }

protected:
	std::string m_name;
};

