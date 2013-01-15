#include <GLXW/glxw.h>
#include <GL/glfw3.h>
#include <unistd.h>
#include <glm/glm.hpp>

#include "global.h"
#include "utils/File.cpp"
#include "utils/Logger.cpp"
#include "utils/StringUtils.cpp"
#include "renderer/gl30/ShaderStage.cpp"
#include "renderer/gl30/ShaderProgram.cpp"

#include "helpers.h"

const char *Nepgear::Arg0;

int main(int argc, char **argv)
{
	Nepgear::Arg0 = argv[0];
	LOG = new Logger("logs/shader-test.txt");

	glfwInit();
	GLFWwindow *w = glfwCreateWindow(960, 540, "Shader Test", NULL, NULL);
	glfwMakeContextCurrent(w);
	glxwInit();

	ShaderProgram p("Generic.Vertex.GL30", "Generic.Fragment.GL30");
	{
		p.BindAttrib(0, "Position");
		p.Link();
		p.Bind();
		p.SetMatrix4("ModelViewProjection", glm::mat4(1.0));
	}

	GLuint vao, buf[2];
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(2, &buf[0]);
		glBindVertexArray(vao);
		
		float verts[] = { -1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0 };
		unsigned indices[] = { 0, 1, 2, 3 };
		
		glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);	
		glEnableVertexAttribArray(0);
	}
	
	CheckError();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, NULL);
	glfwSwapBuffers(w);

	while (!glfwGetWindowParam(w, GLFW_SHOULD_CLOSE) && !glfwGetKey(w, GLFW_KEY_ESCAPE))
	{
		glfwWaitEvents();
	}

	// cleanup:
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, buf);

	p.Cleanup();

	glfwDestroyWindow(w);

	SAFE_DELETE(LOG);

	deinit_physfs();
}
