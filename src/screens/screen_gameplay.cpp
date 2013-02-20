#include <GLXW/glxw.h>
#include <GL/glfw3.h>
#include "screen_gameplay.h"
#include "utils/logger.h"
#include "input/input_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "renderer/gl30/shader.h"
#include "actors/model.h"

using namespace std;

namespace
{
	glm::vec3 orientation(0.0, 1.0, 0.0), position(0.0, 10.0, 0.0);
	glm::vec3 forward(0, 1, 0);

	Nepgear::Model *mdl;
	ShaderProgram *phong;
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

	glEnable(GL_DEPTH_TEST);
}

ScreenGameplay::~ScreenGameplay()
{
	delete mdl;
	delete phong;
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
			LOG->Debug("the final frontier");
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

	glm::mat4 xform(1.0);
	glm::mat4 view(1.0);
	glm::mat4 projection = glm::perspective(90.0f/2, (1280.f/2.f)/800.f, 0.1f, 100.0f);

	phong->Bind();
	phong->SetMatrix4("Projection", projection);

	if (joysticks.size() == 0)
	{
		view = glm::translate(view, position);
		phong->SetMatrix4("ModelView", view * xform);
		return;
	}

	int num_axes = glfwGetJoystickParam(joysticks[0].id, GLFW_AXES);
	float *axes = new float[num_axes];
	glfwGetJoystickAxes(joysticks[0].id, axes, num_axes);

	glm::vec2 j1 = glm::vec2(-axes[0], -axes[1]); // left stick
	glm::vec2 j2 = glm::vec2(-axes[2],  axes[3]); // right stick
	glm::vec2 tr = glm::vec2(axes[12]*0.5+0.5, -axes[13]*0.5+0.5); // triggers

	// Z is up.
	glm::vec3 up = glm::vec3(0, 0, 1);
	forward = glm::normalize(glm::rotateZ<float>(forward, j2.x));
	forward = glm::normalize(glm::rotateX<float>(forward, j2.y));
	glm::vec3 right = glm::cross(forward, up);

	// movement
	float speed = delta * 3.0;
	position += forward * (j1.y * speed);
	position += right * (j1.x * speed);
	position += up * ((tr.x + (-tr.y)) * speed);

	view = glm::lookAt(-forward, glm::vec3(0), up);
	view = glm::translate(view, position);

	phong->Bind();
	phong->SetMatrix4("ModelView", view * xform);
	delete[] axes;
}

void ScreenGameplay::Draw(DrawBuffer db)
{
	phong->Bind();
	mdl->draw();
}
