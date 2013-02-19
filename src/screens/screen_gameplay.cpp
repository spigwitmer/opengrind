#include <GLXW/glxw.h>
#include <GL/glfw3.h>
#include "screen_gameplay.h"
#include "utils/logger.h"
#include "input/input_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/gl30/shader.h"
#include "actors/model.h"

using namespace std;

namespace
{
	glm::mat4 rotation(1.0);
	glm::mat4 roll(1.0);

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

void ScreenGameplay::UpdateInternal(double delta)
{
	int num_axes = glfwGetJoystickParam(0, GLFW_AXES);
	float *axes = new float[num_axes];
	glfwGetJoystickAxes(0, axes, num_axes);

	glm::vec2 j1 = glm::vec2(-axes[0], -axes[1]);
	glm::vec2 j2 = glm::vec2(-axes[2], -axes[3]);
	//LOG->Debug("%f %f", j1.x, j1.y);

	rotation = glm::rotate<float>(rotation, j1.x * delta * 50, glm::vec3(0, 0, 1));
	rotation = glm::rotate<float>(rotation, j1.y * delta * 50, glm::vec3(1, 0, 0));

	roll = glm::rotate<float>(roll, j2.x * delta * 50, glm::vec3(0, 1, 0));

	glm::mat4 xform(1.0);
	xform = glm::translate(xform, glm::vec3(-0.5, 3, 0.0));
	xform = glm::translate(xform, glm::vec3(0.0, 0, -1.5));
	xform *= rotation;
	xform *= roll;

	glm::mat4 view = glm::lookAt(glm::vec3(0, -10, 2.5), glm::vec3(0, 0, 0), glm::vec3(0.0, 0.0, 1.0));

	glm::mat4 projection = glm::perspective(90.0f/2, (1280.f/2.f)/800.f, 0.1f, 100.0f);

	phong->Bind();
	phong->SetMatrix4("ModelView", view * xform);
	phong->SetMatrix4("Projection", projection);
	delete[] axes;
}

void ScreenGameplay::Draw(DrawBuffer db)
{
	phong->Bind();
	mdl->draw();
}
