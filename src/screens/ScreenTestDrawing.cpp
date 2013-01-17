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

GLuint tex;

void ScreenTestDrawing::Init()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, &buf[0]);
	
	shader = new ShaderProgram("Mandelbrot.Vertex.GL30", "Mandelbrot.Fragment.GL30");
	shader->BindAttrib(0, "vPosition");
	shader->BindAttrib(1, "vCoords");
	shader->Link();
	shader->Bind();

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	size_t size = 128;
	glm::vec4 *pixels = new glm::vec4[size];

	typedef pair<float, glm::vec4> stop;

	vector<stop> stops;
	stops.push_back(stop(0.00, glm::vec4(0.0, 0.0, 0.0, 1.0)));
	stops.push_back(stop(1.00, glm::vec4(0.2, 0.5, 1.0, 1.0)));

	size_t prev = 0;
	size_t next = 0;

	float factor = size / stops.size();
	for (size_t i = 0; i < size; ++i)
	{
		float progress = float(i)/size;

		if (progress > stops[next].first)
		{
			prev = next;
			next++;
		}

		float prev_scaled = prev * factor;
		float next_scaled = next * factor;

		float stop_progress = glm::max(float(i), 0.001f);
		stop_progress -= prev_scaled;
		stop_progress /= next_scaled;

		glm::vec4 color = glm::mix(
			stops[prev].second, stops[next].second,
			stop_progress
		);
		pixels[i] = color;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, 1, 0, GL_RGBA, GL_FLOAT,
		glm::value_ptr(pixels[0])
	);

	delete[] pixels;

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

void ScreenTestDrawing::Draw()
{
	DisplayManager *dm = DisplayManager::GetSingleton();
	dm->GetRenderer()->SetUniforms(shader);

	shader->SetMatrix4("ModelViewProjection", matrix);
	shader->SetInteger("ColorTable", 0);

	glBindVertexArray(vao);	
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, NULL);	
}
