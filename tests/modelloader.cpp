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

#include "utils/logger.h"
#include "utils/timer.h"

#include "renderer/common/window_gl.h"
#include "renderer/common/error.h"
#include "renderer/gl30/shader.h"
#include "renderer/gl30/postprocess.h"

#include "actors/model.h"

namespace
{

float calc_fxaa_alpha(glm::vec3 in)
{
	return glm::dot(in, glm::vec3(0.299, 0.587, 0.114));
}

}

int main(int argc, char **argv)
{
	Nepgear::Nepgear ng(argc, argv, "model-loader.log");

	Nepgear::File iqm_file, skydome_file;
	Nepgear::Model model, skydome;

	Nepgear::Window_GL wnd;
	Nepgear::WindowParams params;
	params.width = 1280;
	params.height = 800;
	params.fullscreen = 0;
	params.fullscreen_monitor = 0;
	if (!wnd.open(params, "Model loading test"))
		return 1;

	GLFWwindow *w = (GLFWwindow*)wnd.handle;

	iqm_file.open("models/suzanne.iqm");
	model.load_file(iqm_file);
	iqm_file.close();

	skydome_file.open("models/skydome.iqm");
	skydome.load_file(skydome_file);
	skydome_file.close();

	bool fxaa_enabled = true;

// cinnamon leaves a ghost of the window if it crashes too fast.
#ifdef X11
	Display* xdpy = glfwGetX11Display();
	usleep(30000); // sleep for 30ms, it's enough.
#endif

	ShaderProgram sky("Skydome.Vertex.GL30", "Skydome.Fragment.GL30");
	{
		sky.BindAttrib(0, "vPosition");
		sky.BindAttrib(1, "vCoords");
		sky.BindAttrib(2, "vNormal");
		sky.BindAttrib(3, "vTangent");
		sky.Link();

		sky.Bind();
		sky.SetInteger("sky_tex", 0);
	}

	ShaderProgram phong("Phong.Vertex.GL30", "Phong.Fragment.GL30");
	{
		phong.BindAttrib(0, "vPosition");
		phong.BindAttrib(1, "vCoords");
		phong.BindAttrib(2, "vNormal");
		phong.BindAttrib(3, "vTangent");
		phong.Link();
	}

	ShaderProgram outline("Phong.Vertex.Outline.GL30", "Phong.Fragment.Outline.GL30");
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

	glfwSwapInterval(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW); // IQM winds clockwise.

	double now = 0.0, then = 0.0, delta = 0.0;
	double pos = 0.0;
	bool moving = false;

	int button_held = 0;

	CheckError();

	Nepgear::PostProcessEffect fxaa_pp;
	if (fxaa_enabled)
	{
		fxaa_pp.init(1280, 800);
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

	GLuint tex;
	{
		using namespace std;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		size_t size = 256;
		glm::vec4 *pixels = new glm::vec4[size];

		typedef pair<float, glm::vec4> stop;

		vector<stop> stops;
		stops.push_back(stop(0.0, glm::vec4(0.2, 0.7, 1.0, 1.0)));
		stops.push_back(stop(0.5, glm::vec4(0.0, 0.0, 0.0, 1.0)));
		stops.push_back(stop(1.0, glm::vec4(0.0, 0.5, 0.9, 1.0)));

		size_t prev = 0;
		size_t next = 0;

		float factor = float(size / stops.size());
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

		if (glfwGetKey(w, GLFW_KEY_SPACE))
		{
			if (button_held < 2)
				button_held++;
		}
		else button_held = 0;

		if (button_held == 1)
			moving = !moving;

		if (moving) pos += delta;

		glm::mat4 xform_left(1.0);
		xform_left = glm::translate(xform_left, glm::vec3(0.5, 3, 0.0));
		xform_left = glm::rotate<float>(xform_left, glm::mod(pos * 25, 360.0), glm::vec3(0.0, 0.0, 1.0));
		xform_left = glm::translate(xform_left, glm::vec3(0.0, 0, -1.5));

		glm::mat4 xform_right(1.0);
		xform_right = glm::translate(xform_right, glm::vec3(-0.5, 3, 0.0));
		xform_right = glm::rotate<float>(xform_right, glm::mod(pos * 25, 360.0), glm::vec3(0.0, 0.0, 1.0));
		xform_right = glm::translate(xform_right, glm::vec3(0.0, 0, -1.5));

		glm::mat4 view = glm::lookAt(glm::vec3(0, -10, 2.5), glm::vec3(0, 0, 0), glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 projection = glm::perspective(90.0f/2, (1280.f/2.f)/800.f, 0.1f, 100.0f);

		glViewport(0, 0, 1280, 800);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (fxaa_enabled) fxaa_pp.bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, 640, 800);

		glBindTexture(GL_TEXTURE_2D, tex);
		glCullFace(GL_FRONT);
		sky.Bind();
		sky.SetMatrix4("ModelView", view);// * glm::scale(glm::mat4(1.0), glm::vec3(2.0)));
		sky.SetMatrix4("Projection", projection);
		skydome.draw();

		outline.Bind();
		outline.SetMatrix4("ModelView", view * xform_left);
		outline.SetMatrix4("Projection", projection);
		glCullFace(GL_FRONT);
		//model.draw();

		phong.Bind();
		phong.SetMatrix4("ModelView", view * xform_left);
		phong.SetMatrix4("Projection", projection);
		glCullFace(GL_BACK);
		model.draw();

		glViewport(640, 0, 640, 800);

		glBindTexture(GL_TEXTURE_2D, tex);
		glCullFace(GL_FRONT);
		sky.Bind();
		skydome.draw();

		outline.Bind();
		outline.SetMatrix4("ModelView", view * xform_right);
		glCullFace(GL_FRONT);
		//model.draw();

		phong.Bind();
		phong.SetMatrix4("ModelView", view * xform_right);
		glCullFace(GL_BACK);
		model.draw();

		glViewport(0, 0, 1280, 800);

		if (fxaa_enabled)
		{
			fxaa_pp.unbind();
			fxaa_pp.draw();
		}

		CheckError();

		glfwSwapBuffers(w);
		glfwPollEvents();
	}

	phong.Cleanup();
	sky.Cleanup();
	outline.Cleanup();
	fxaa.Cleanup();

	model.cleanup();

	glfwDestroyWindow(w);

	return 0;
}
