#include <GLXW/glxw.h>
#include <vector>
#include "utils/Logger.h"
#include "renderer/DisplayManager.h"

#include "Plane.h"

using namespace std;
using namespace glm;

Plane::Plane() :
	m_init(false)
{
	RenderSystem* renderer = DisplayManager::GetSingleton()->GetRenderer();

	GLubyte indices[] = { 1, 2, 3, 4 };
	GLData verts[4];

	verts[0].pos = vec3(-0.5, -0.5, 0);
	verts[1].pos = vec3( 0.5, -0.5, 0);
	verts[2].pos = vec3(-0.5,  0.5, 0);
	verts[3].pos = vec3( 0.5,  0.5, 0);

	verts[0].tex = vec2(0, 0);
	verts[1].tex = vec2(1, 0);
	verts[2].tex = vec2(0, 1);
	verts[3].tex = vec2(1, 1);

	//renderer->GenerateObject();
	renderer->GenerateBuffers(2, m_VBO);
	renderer->Upload(m_VBO[0], VERTEX_BUFFER, sizeof(GLData)*4, &verts[0].pos.x);
	renderer->Upload(m_VBO[1], INDEX_BUFFER,  sizeof(GLData)*4, &indices);

	/*	
	glGenBuffers(2, m_VBO);

	// Oh, also this will probably crash or otherwise not work.
	if (!glIsBuffer(m_VBO[0]) || !glIsBuffer(m_VBO[1]))
	{
		LOG->Warn("Could not allocate buffers.");
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLData)*4, &verts[0].pos.x, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint8_t)*4, &indices, GL_STATIC_DRAW);
	*/
	m_init = true;
}

Plane::~Plane()
{
	glDeleteBuffers(2, m_VBO);
}

void Plane::DrawInternal()
{
	if (!m_init)
		return;

	// vao bind...
	//glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[1]);

	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, NULL);
}

/**
 * Colby Klein, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
