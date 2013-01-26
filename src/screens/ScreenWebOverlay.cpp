#include <GLXW/glxw.h>
#include "utils/Logger.h"
#include "screens/ScreenManager.h"
#include "ScreenWebOverlay.h"
#include "renderer/DisplayManager.h"
#include "renderer/gl30/Shader.h"
#include "renderer/common/Error.h"
#include <physfs.h>
#include "input/InputManager.h"

#include <Awesomium/WebCore.h>

using namespace std;

REGISTER_SCREEN(ScreenWebOverlay)

ScreenWebOverlay::ScreenWebOverlay(string name) :
	Screen(name), web_core(NULL), web_view(NULL), m_ready(false)
{
	DisplayManager *dm = DisplayManager::GetSingleton();
	viewport = dm->GetRenderer()->GetViewport();

	string log_path = PHYSFS_getRealDir("/");

	Awesomium::WebCoreConfig cfg;
	cfg.setEnablePlugins(false);
	cfg.setUserDataPath(log_path);
	cfg.setLogPath(log_path+"/logs");

	web_core = new Awesomium::WebCore(cfg);
	web_view = web_core->createWebView(
		(int)viewport.z, (int)viewport.w
	);
	web_view->setTransparent(true);

	assert(web_view);
}

ScreenWebOverlay::~ScreenWebOverlay()
{
	web_view->destroy();
	delete web_core;
}

void ScreenWebOverlay::Init()
{
//	web_view->loadURL("http://www.google.com");
	string theme_path = "/themes/default/";
	const char* phys_path = PHYSFS_getRealDir(theme_path.c_str());
	web_core->setBaseDirectory(phys_path+theme_path);

	web_view->loadFile(m_name + "/index.html");
	web_view->focus();

	wk_shader = new ShaderProgram(
		"Awesomium.Vertex.GL30", "Awesomium.Fragment.GL30"
	);
	wk_shader->BindAttrib(0, "vPosition");
	wk_shader->BindAttrib(1, "vCoords");
	wk_shader->Link();
	wk_shader->Bind();

	// basic quad
	glGenBuffers(2, buf);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	CheckError();
	
	float verts[] = {
		// X Y Z U V. V is flipped.
		-1.0, -1.0, 0.0, 0.0, 1.0,
		-1.0,  1.0, 0.0, 0.0, 0.0,
		 1.0, -1.0, 0.0, 1.0, 1.0,
		 1.0,  1.0, 0.0, 1.0, 0.0
	};
	unsigned indices[] = { 0, 1, 2, 3 };
	
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	CheckError();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(sizeof(float)*0));	
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(sizeof(float)*3));
	glEnableVertexAttribArray(1);

	CheckError();

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	CheckError();
}

bool ScreenWebOverlay::HandleEvent(const string &name, const IEvent &evt)
{
	if (name == "Input")
	{
		InputManager *im = InputManager::GetSingleton();
		if (im->GetButton(RS_KEY_F2)->IsDown())
		{
			web_view->reload();
		}
	}

	if (name == "Resize")
	{
		DisplayManager *dm = DisplayManager::GetSingleton();
		viewport = dm->GetRenderer()->GetViewport();

		web_view->resize((int)viewport.z, (int)viewport.w, false);
	}
	return true;
}

void ScreenWebOverlay::UpdateInternal(double delta)
{
	web_core->update();

	if (web_view->isLoadingPage())
		return;

	if (web_view->isDirty())
	{
		const Awesomium::RenderBuffer* rb = web_view->render();

		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rb->width, rb->height,
			0, GL_BGRA, GL_UNSIGNED_BYTE, rb->buffer
		);

		CheckError();

		m_ready = true;
	}
}

void ScreenWebOverlay::Draw(DrawBuffer db)
{
	glDisable(GL_DEPTH_TEST);

	if (!m_ready)
		return;

	CheckError();

	wk_shader->Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	wk_shader->SetInteger("Texture", 0);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, NULL);

	glEnable(GL_DEPTH_TEST);
}
