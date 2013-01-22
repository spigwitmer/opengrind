#include <GLXW/glxw.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <map>

#include "global.h"
#include "Nepgear.cpp"
#include "utils/File.h"
#include "renderer/common/Error.h"
#include "renderer/common/GLWindow.h"
#include "renderer/gl30/Shader.h"

#include "helpers.h"

static void resize(GLFWwindow *w, int width, int height)
{
	glViewport(0, 0, width, height);
	ShaderProgram *p = (ShaderProgram*)glfwGetWindowUserPointer(w);
	p->SetVector2("Viewport", glm::vec2(width, height));
}

int main(int argc, char **argv)
{
	Nepgear ng(argc, argv, "shader-test.log");

	GLWindow wnd;
	if (!wnd.open(WindowParams { 960, 540 }, "Shader Test"))
		return 1;

	ShaderProgram p("Mandelbrot.Vertex.GL30", "Mandelbrot.Fragment.GL30");
	{
		p.BindAttrib(0, "vPosition");
		p.BindAttrib(1, "vCoords");
		p.Link();
		p.Bind();
		p.SetMatrix4("ModelViewProjection", glm::mat4(1.0));
		p.SetVector2("Viewport", glm::vec2(960, 540));
		p.SetInteger("ColorTable", 0);
	}

	GLFWwindow *w = (GLFWwindow*)wnd.handle;
	glfwSetWindowSizeCallback(w, &resize);
	glfwSetWindowUserPointer(w, &p);

	GLuint vao, buf[2];
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(2, &buf[0]);
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
	}
	
	GLuint tex;
	{
		using namespace std;

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
	}
	CheckError();

	while (!glfwGetWindowParam(w, GLFW_SHOULD_CLOSE) && !glfwGetKey(w, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, NULL);
		glfwSwapBuffers(w);
		glfwWaitEvents();
	}

	// cleanup:
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, buf);

	glDeleteTextures(1, &tex);

	p.Cleanup();

	glfwDestroyWindow(w);

	return 0;
}
