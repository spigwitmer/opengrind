#include "nepgear.h"
#include "helpers.h"

#include <GLXW/glxw.h>
#include <GL/glfw3.h>

// enables alt+tab support and cinnamon hack.
#if 0
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#define X11
#include <GL/glfw3native.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>
#include <map>

#include <unistd.h>

#include "utils/file.h"
#include "utils/logger.h"

#include "renderer/common/window_gl.h"
#include "renderer/common/error.h"
#include "renderer/gl30/shader.h"
#include "renderer/gl30/postprocess.h"

#include "actors/iqm.h"
#include "actors/model.h"

#include "utils/timer.h"

namespace
{

void resize(GLFWwindow *w, int width, int height)
{
	glViewport(0, 0, width, height);
	ShaderProgram *p = (ShaderProgram*)glfwGetWindowUserPointer(w);
	p->SetVector2("Viewport", glm::vec2(width, height));
}

float calc_fxaa_alpha(glm::vec3 in)
{
	return glm::dot(in, glm::vec3(0.299, 0.587, 0.114));
}

}

int main(int argc, char **argv)
{
	Nepgear::Nepgear ng(argc, argv, "model-loader.log");

	Nepgear::File iqm_file;
	Nepgear::Model model;

	Nepgear::Window_GL wnd;
	Nepgear::WindowParams params;
	params.width = 1280;
	params.height = 720;
	params.fullscreen = 0;
	params.fullscreen_monitor = 0;
	if (!wnd.open(params, "Model loading test"))
		return 1;

	iqm_file.open("models/suzanne.iqm", Nepgear::FileAccessMode_Read);
	model.load_file(iqm_file);
	iqm_file.close();

	bool fxaa_enabled = true;

// cinnamon leaves a ghost of the window if it crashes too fast.
#ifdef X11
	Display* xdpy = glfwGetX11Display();
	usleep(17000); // sleep for 17ms, it's enough.
#endif

	ShaderProgram p("Skydome.Vertex.GL30", "Skydome.Fragment.GL30");
	{
		p.BindAttrib(0, "vPosition");
		p.BindAttrib(1, "vCoords");
		p.BindAttrib(2, "vNormal");
		p.BindAttrib(3, "vTangent");
		p.Link();
	}

	ShaderProgram outline("Skydome.Vertex.Outline.GL30", "Skydome.Fragment.Outline.GL30");
	{
		outline.BindAttrib(0, "vPosition");
		outline.BindAttrib(1, "vCoords");
		outline.BindAttrib(2, "vNormal");
		outline.BindAttrib(3, "vTangent");
		outline.Link();
	}

	ShaderProgram fxaa("FXAA.Vertex.GL30", "FXAA.Fragment.GL30");
	{
		fxaa.BindAttrib(0, "vPosition");
		fxaa.BindAttrib(1, "vCoords");
		fxaa.Link();
		fxaa.Bind();
		fxaa.SetInteger("Texture", 0);
	}

	CheckError();

	GLFWwindow *w = (GLFWwindow*)wnd.handle;
	glfwSetWindowSizeCallback(w, &resize);
	glfwSetWindowUserPointer(w, &p);
	glfwSwapInterval(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW); // IQM winds clockwise.

	double now = 0.0, then = 0.0, delta = 0.0;
	double pos = 0.0;
	bool moving = true;

	int button_held = 0;

	CheckError();

	Nepgear::PostProcessEffect fxaa_pp;
	if (fxaa_enabled)
	{
		fxaa_pp.init(1280, 720);
		fxaa_pp.set_material(&fxaa);
		fxaa_pp.bind();
	}

	CheckError();

	glm::vec4 color(0.25, 0.25, 0.25, 1.0);
	color.a = calc_fxaa_alpha(glm::vec3(color.r, color.g, color.b));
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glfwSetInputMode(w, GLFW_CURSOR_MODE, GLFW_CURSOR_CAPTURED);

	int frames = 0;
	int last_fps;
	Nepgear::Timer frametimer;

	while (!glfwGetWindowParam(w, GLFW_SHOULD_CLOSE) && !glfwGetKey(w, GLFW_KEY_ESCAPE))
	{
		now = glfwGetTime();
		delta = now - then;
		then = now;

		if (frametimer.Ago() > 1.0)
		{
			frametimer.Touch();
			last_fps = frames;
			LOG->Debug("FPS: %d (%0.2fms)", frames, (1.0 / frames)*1000);
			frames = 0;
		}
		frames++;

		// if a frame takes more than twice the average, log it.
		if (last_fps != -1)
		{
			double target = 1.0 / last_fps;
			// don't log "skips" which are stupidly small.
			if (delta > target*2 && target > 0.005)
			{
				LOG->Debug("Skip: %0.0f (%0.0fms)", delta/target, delta*1000);
			}
		}

#ifdef X11
		/* Nasty hack to support alt+tab with fullscreen windows.
		 * essentially, it's because X is horrible. */
		if (params.fullscreen)
		{
			if (glfwGetKey(w, GLFW_KEY_LEFT_ALT) ||
				glfwGetKey(w, GLFW_KEY_RIGHT_ALT))
			{
				XUngrabPointer(xdpy, CurrentTime);
				XFlush(xdpy);
			}
			else
			{
				glfwSetInputMode(w, GLFW_CURSOR_MODE, GLFW_CURSOR_CAPTURED);
			}
		}
#endif

		if (fxaa_enabled) fxaa_pp.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(w, GLFW_KEY_SPACE))
		{
			if (button_held < 2)
				button_held++;
		}
		else button_held = 0;

		if (button_held == 1)
			moving = !moving;

		if (moving) pos += delta;

		glm::mat4 xform(1.0);
		xform = glm::translate(xform, glm::vec3(0, 3, 0.0));
		xform = glm::rotate<float>(xform, glm::mod(pos * 25, 360.0), glm::vec3(0.0, 0.0, 1.0));
		xform = glm::translate(xform, glm::vec3(0.0, 0, -1.5));
		glm::mat4 view = glm::lookAt(glm::vec3(0, -10, 2.5), glm::vec3(0, 0, 0), glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 projection = glm::perspective(40.0f, 960.f/540.f, 0.1f, 100.0f);

		outline.Bind();
		outline.SetMatrix4("ModelView", view * xform);
		outline.SetMatrix4("Projection", projection);
		glCullFace(GL_FRONT);
		model.draw();

		p.Bind();
		p.SetMatrix4("ModelView", view * xform);
		p.SetMatrix4("Projection", projection);
		glCullFace(GL_BACK);
		model.draw();

		if (fxaa_enabled)
		{
			fxaa_pp.unbind();
			fxaa_pp.draw();
		}

		CheckError();

		glfwSwapBuffers(w);
		glfwPollEvents();
	}

	p.Cleanup();
	outline.Cleanup();
	fxaa.Cleanup();

	model.cleanup();

	glfwDestroyWindow(w);

	return 0;
}
