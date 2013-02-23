#pragma once

#include <vector>

namespace Nepgear
{

class VertexBuffer;
class ShaderProgram;
class RenderTarget;

class GraphicsDevice
{
public:
	GraphicsDevice();
	~GraphicsDevice();

	const VertexBuffer*		create_vertex_buffer();
	const ShaderProgram*	create_shader_program();
	const RenderTarget*		create_render_target();

//	void set_shader_program(ShaderProgram*);
//	void set_texture_sampler(int sampler_id);
//	void set_depth_test(bool use_depth);
//	void clear(bool color = true, bool depth = true);

private:
	std::vector<VertexBuffer*> m_vertex_buffers;
	std::vector<ShaderProgram*> m_shader_programs;
	std::vector<RenderTarget*> m_render_targets;
};

} // Nepgear
