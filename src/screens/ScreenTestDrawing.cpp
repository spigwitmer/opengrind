#include <GLXW/glxw.h>
#include "ScreenTestDrawing.h"
#include "utils/Logger.h"

#define GLM_FORCE_SSE2 1
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer/gl30/Shader.h"
#include "renderer/common/Error.h"

using namespace std;

REGISTER_SCREEN(ScreenTestDrawing)

ShaderProgram *shader;

ScreenTestDrawing::ScreenTestDrawing(string name) : Screen(name)
{
}

GLuint tex;

void ScreenTestDrawing::Init()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, &buf[0]);
	
	shader = new ShaderProgram("Mandelbrot.Vertex.GL30", "Mandelbrot.Fragment.GL30");
	shader->BindAttrib(0, "Position");
	shader->Link();
	shader->Bind();
	


	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int size = 128;
	unsigned *pixels = new unsigned[size];

	for (int i = 0; i < size; ++i)
	{
		pixels[i] = (int((float(i)/size)*255) << 8) + (0xFF << 24);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, 1, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

	delete[] pixels;

	glBindVertexArray(vao);
	
	float verts[] = { -1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0 };
	unsigned indices[] = { 0, 1, 2, 3 };
	
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);	
	glEnableVertexAttribArray(0);
	
	CheckError();
}

ScreenTestDrawing::~ScreenTestDrawing()
{
	// cleanup:
	glDeleteVertexArrays(1, &vao);	
	glDeleteBuffers(2, buf);	
	
	delete shader;
}

void ScreenTestDrawing::HandleMessage(const string &name, const IEvent &evt)
{
	// todo
}

void ScreenTestDrawing::UpdateInternal(double delta)
{
	// todo
}

// no sense making this a bunch of times.
static glm::mat4 matrix(1.0);

void ScreenTestDrawing::Draw()
{
	shader->SetMatrix4("ModelViewProjection", matrix);
	shader->SetInteger("ColorTable", 0);

	glBindVertexArray(vao);	
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, NULL);	
}
