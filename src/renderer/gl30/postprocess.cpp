#include <GLXW/glxw.h>
#include "utils/logger.h"
#include "postprocess.h"
#include "renderer/common/error.h"

namespace Nepgear
{
void PostProcessEffect::init(int width, int height)
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, 0
	);

	// renderbuffer handle
	GLuint rbf;
	
	// generate renderbuffers
	glGenRenderbuffers(1, &rbf);
	glBindRenderbuffer(GL_RENDERBUFFER, rbf);
	glRenderbufferStorage(
		GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
		width, height
	);

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0
	);
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbf
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (vao != 0) return;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	GLfloat post_effect_verts[] = {
		 1.0f, 1.0f, 0.0f,       1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
		 1.0f,-1.0f, 0.0f,       1.0f, 0.0f,
		-1.0f,-1.0f, 0.0f,       0.0f, 0.0f,
	};
	GLuint post_effect_indices[] = { 0,1,2, 2,1,3 };

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*5, post_effect_verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*3, post_effect_indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));
}

void PostProcessEffect::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void PostProcessEffect::set_material(ShaderProgram *mat)
{
	m_material = mat;
}

void PostProcessEffect::draw()
{
	m_material->Bind();

	glFrontFace(GL_CCW);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glDisable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
}

void PostProcessEffect::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned PostProcessEffect::vao = 0;
unsigned PostProcessEffect::vbo = 0;
unsigned PostProcessEffect::ibo = 0;

}
