#include <GLXW/glxw.h>
#include "ScreenTestDrawing.h"
#include "utils/Logger.h"

#define GLM_FORCE_SSE2 1
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer/gl30/Shader.h"
#include "renderer/common/Error.h"
#include "renderer/DisplayManager.h"

using namespace std;

#define SCALE(x, l1, h1, l2, h2)	(((x) - (l1)) * ((h2) - (l2)) / ((h1) - (l1)) + (l2))

REGISTER_SCREEN(ScreenTestDrawing)

ShaderProgram *shader;

ScreenTestDrawing::ScreenTestDrawing(string name) : Screen(name)
{
}

void ScreenTestDrawing::Init()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, &buf[0]);
	
	shader = new ShaderProgram("Generic.Vertex.GL30", "Generic.Fragment.GL30");
	shader->BindAttrib(0, "vPosition");
	shader->BindAttrib(1, "vCoords");
	shader->Link();
	shader->Bind();

	glBindVertexArray(vao);
	
	float verts[] = {
		// X Y Z U V
		-1.0, -1.0, 0.0, 0.0, 0.0,
		-1.0,  1.0, 0.0, 0.0, 1.0,
		 1.0, -1.0, 0.0, 1.0, 0.0,
		 1.0,  1.0, 0.0, 1.0, 1.0
	};
	unsigned indices[] = { 0, 1, 2, 3 };
	
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(sizeof(float)*0));	
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(sizeof(float)*3));
	glEnableVertexAttribArray(1);
	
	CheckError();
}

ScreenTestDrawing::~ScreenTestDrawing()
{
	// cleanup:
	glDeleteVertexArrays(1, &vao);	
	glDeleteBuffers(2, buf);	
	
	delete shader;
}

bool ScreenTestDrawing::HandleEvent(const string &name, const IEvent &evt)
{
	return true;
}

void ScreenTestDrawing::UpdateInternal(double delta)
{
	// todo
}

// no sense making this a bunch of times.
static glm::mat4 matrix(1.0);

void ScreenTestDrawing::Draw(DrawBuffer db)
{
	DisplayManager *dm = DisplayManager::GetSingleton();
	dm->GetRenderer()->SetUniforms(shader);

	shader->SetMatrix4("ModelViewProjection", matrix);
	shader->SetInteger("ColorTable", 0);

	glBindVertexArray(vao);	
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, NULL);	
}
