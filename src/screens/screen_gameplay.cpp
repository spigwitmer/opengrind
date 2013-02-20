#include <GLXW/glxw.h>
#include <GL/glfw3.h>
#include "screen_gameplay.h"
#include "utils/logger.h"
#include "input/input_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "renderer/common/error.h"
#include "renderer/gl30/shader.h"
#include "actors/model.h"
#include "renderer/gl30/postprocess.h"


using namespace std;

namespace
{
	glm::vec3 orientation(0.0, 1.0, 0.0), position(0.0, 10.0, 0.0);
	glm::vec3 forward(0, 1, 0);
	glm::mat4 view(1.0);

	Nepgear::Model *mdl;
	ShaderProgram *phong, *fxaa;
	Nepgear::PostProcessEffect *fxaa_pp;
	bool fxaa_enabled = true, ignore_joystick = false;
	int last_x = 0, last_y = 0;

	float calc_fxaa_alpha(glm::vec3 in)
	{
		return glm::dot(in, glm::vec3(0.299, 0.587, 0.114));
	}
}

REGISTER_SCREEN(ScreenGameplay)

ScreenGameplay::ScreenGameplay(string name) : Screen(name)
{
}

void ScreenGameplay::Init()
{
	glfwSwapInterval(1);

	mdl = new Nepgear::Model();
	mdl->load_file(Nepgear::File("models/suzanne.iqm"));

	phong = new ShaderProgram("Phong.Vertex.GL30", "Phong.Fragment.GL30");
	phong->BindAttrib(0, "vPosition");
	phong->BindAttrib(1, "vCoords");
	phong->BindAttrib(2, "vNormal");
	phong->BindAttrib(3, "vTangent");
	phong->Link();

	phong->Bind();
	phong->SetMatrix4("ModelView", glm::mat4(1.0));

	fxaa = new ShaderProgram("FXAA.Vertex.GL30", "FXAA.Fragment.GL30");
	{
		fxaa->BindAttrib(0, "vPosition");
		fxaa->BindAttrib(1, "vCoords");
		fxaa->Link();
		fxaa->Bind();
		fxaa->SetInteger("Texture", 0);
	}

	glEnable(GL_DEPTH_TEST);

	fxaa_pp = new Nepgear::PostProcessEffect();
	if (fxaa_enabled)
	{
		fxaa_pp->init(640, 800);
		fxaa_pp->set_material(fxaa);
	}

	CheckError();

	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR_MODE, GLFW_CURSOR_CAPTURED);
	glfwGetCursorPos(glfwGetCurrentContext(), &last_x, &last_y);
}

ScreenGameplay::~ScreenGameplay()
{
	delete mdl;
	delete phong;
	delete fxaa;
	delete fxaa_pp;
}

void ScreenGameplay::Input(Nepgear::InputManager *im)
{

}

bool ScreenGameplay::HandleEvent(const string &name, const IEvent &evt)
{
	if (name == "Input")
	{
		Nepgear::InputManager *im = Nepgear::InputManager::GetSingleton();
		Input(im);
		if (im->GetButton(RS_KEY_SPACE)->IsDown())
		{
			LOG->Debug("the final frontier");
			ignore_joystick = !ignore_joystick;
		}
	}

	return true;
}

struct JoystickDevice {
	std::string name;
	int id;
};

void ScreenGameplay::UpdateInternal(double delta)
{
	std::vector<JoystickDevice> joysticks;

	for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
	{
		if (!glfwGetJoystickParam(i, GLFW_PRESENT))
			continue;

		JoystickDevice js;
		js.name = glfwGetJoystickName(i);
		js.id = i;

		// Blacklist MBP accelerometers.
		if (js.name == "applesmc")
			continue;

		joysticks.push_back(js);
	}

	view = glm::mat4(1.0);
	glm::mat4 projection = glm::perspective(90.0f/2, (1280.f/2.f)/800.f, 0.1f, 100.0f);

	phong->Bind();
	phong->SetMatrix4("Projection", projection);

	glm::vec2 j1, j2, tr;

	if (joysticks.size() == 0 || ignore_joystick)
	{
		GLFWwindow *w = glfwGetCurrentContext();
		// mouse shit
		int x, y;
		glfwGetCursorPos(w, &x, &y);
		j2 = glm::vec2(-float(x - last_x)/25, -float(y - last_y)/25);

		last_x = x;
		last_y = y;

		if (glfwGetKey(w, GLFW_KEY_S))
			j1.y += 1.0;
		if (glfwGetKey(w, GLFW_KEY_W))
			j1.y -= 1.0;
		if (glfwGetKey(w, GLFW_KEY_A))
			j1.x += 1.0;
		if (glfwGetKey(w, GLFW_KEY_D))
			j1.x -= 1.0;
	}
	else
	{
		int num_axes = glfwGetJoystickParam(joysticks[0].id, GLFW_AXES);
		float *axes = new float[num_axes];
		glfwGetJoystickAxes(joysticks[0].id, axes, num_axes);

		j1 = glm::vec2(-axes[0], -axes[1]); // left stick
		j2 = glm::vec2(-axes[2],  axes[3]); // right stick
		tr = glm::vec2(axes[12]*0.5+0.5, -axes[13]*0.5+0.5); // triggers
		delete[] axes;
	}

	// Z is up.
	// TODO: use local axes. this breaks if you turn around/sideways.
	glm::vec3 up = glm::vec3(0, 0, 1);
	forward = glm::rotateZ<float>(forward, j2.x);
	forward = glm::rotateX<float>(forward, j2.y);
	glm::vec3 right = glm::cross(forward, up);

	// movement
	float speed = delta * 3.0;
	position += forward * (j1.y * speed);
	position += right * (j1.x * speed);
	position += up * ((tr.x + (-tr.y)) * speed);

	view = glm::lookAt(-forward, glm::vec3(0), up);
	view = glm::translate(view, position);

	phong->Bind();
	phong->SetMatrix4("ModelView", view);

	fxaa_pp->bind();
	glm::vec4 color(0.1, 0.1, 0.1, 1.0);
	color.a = calc_fxaa_alpha(glm::vec3(color.r, color.g, color.b));
	glClearColor(color.r, color.g, color.b, color.a);
	fxaa_pp->unbind();
}

void ScreenGameplay::Draw(DrawBuffer db, glm::vec4 vp)
{
	fxaa_pp->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (int)vp.z, (int)vp.w);

	const float separation = 0.075;
	glm::mat4 xform(1.0);
	if (db == DrawBuffer_Left)
		xform = glm::translate(xform, glm::vec3(-separation, 0, 0));
	else if (db == DrawBuffer_Right)
		xform = glm::translate(xform, glm::vec3( separation, 0, 0));

	phong->Bind();
	phong->SetMatrix4("ModelView", view * xform);
	mdl->draw();
	fxaa_pp->unbind();

	glViewport((int)vp.x, (int)vp.y, (int)vp.z, (int)vp.w);
	fxaa_pp->draw();
}
