#include "graphics_device.h"
#include <cstddef>
//#include "vertex_buffer.h"
//#include "shader_program.h"
//#include "render_target.h"

namespace Nepgear
{

class VertexBuffer {};
class ShaderProgram {};
class RenderTarget {};

GraphicsDevice::GraphicsDevice()
{
}

GraphicsDevice::~GraphicsDevice()
{
	for (size_t i = 0; i < m_vertex_buffers.size(); ++i)
		delete m_vertex_buffers[i];
	for (size_t i = 0; i < m_shader_programs.size(); ++i)
		delete m_shader_programs[i];
	for (size_t i = 0; i < m_render_targets.size(); ++i)
		delete m_render_targets[i];
}

const VertexBuffer* GraphicsDevice::create_vertex_buffer()
{
	VertexBuffer *p = new VertexBuffer();
	m_vertex_buffers.push_back(p);
	return p;
}

const ShaderProgram *GraphicsDevice::create_shader_program()
{
	ShaderProgram *p = new ShaderProgram();
	m_shader_programs.push_back(p);
	return p;
}

const RenderTarget *GraphicsDevice::create_render_target()
{
	RenderTarget *p = new RenderTarget();
	m_render_targets.push_back(p);
	return p;
}

}
