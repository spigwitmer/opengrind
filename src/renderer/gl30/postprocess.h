#ifndef _NEPGEAR_POSTPROCESS_H_
#define _NEPGEAR_POSTPROCESS_H_

#include <string>
#include "shader.h"

namespace Nepgear
{
class PostProcessEffect
{
public:
	void init(int width, int height);
	void set_material(ShaderProgram *mat);
	void bind();
	void unbind(); // more accurately: bind back buffer.
	void draw();

	// we don't need different ones of these for each post effect;
	// they're just gonna be fullscreen quads,
	static unsigned vao, vbo, ibo;
protected:
	unsigned m_texture, m_fbo;
	ShaderProgram *m_material;
};
} 

#endif
