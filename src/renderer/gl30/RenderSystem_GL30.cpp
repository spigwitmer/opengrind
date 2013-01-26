#include "RenderSystem_GL30.h"
#include <GLXW/glxw.h>
#include "renderer/DisplayManager.h"
#include "renderer/gl30/Shader.h"

void RenderSystem_GL30::SetUniforms(ShaderProgram *shader)
{
	shader->SetVector2("Viewport", glm::vec2(m_viewport.z, m_viewport.w));
}

void RenderSystem::SetViewport(glm::vec4 vp)
{
	m_viewport = vp;
}

glm::vec4 RenderSystem::GetViewport()
{
	Window *w = DisplayManager::GetSingleton()->GetWindow();
	glm::vec4 view = m_viewport;

	if (w->params.stereoscopic_mode == StereoscopicMode_SBS)
		view.z /= 2.0;

	return view;
}

void RenderSystem_GL30::UpdateViewport()
{
	DisplayManager *dm = DisplayManager::GetSingleton();
	SetViewport(dm->GetViewport());
	glm::vec4 view = GetViewport();
	glViewport((GLsizei)view.x, (GLsizei)view.y, (GLsizei)view.z, (GLsizei)view.w);
}
