#define GLM_SWIZZLE
#include "RenderSystem_GL30.h"
#include <GLXW/glxw.h>
#include "renderer/DisplayManager.h"
#include "renderer/gl30/Shader.h"

void RenderSystem_GL30::SetUniforms(ShaderProgram *shader)
{
	shader->SetVector2("Viewport", m_viewport.zw);
}

void RenderSystem_GL30::UpdateViewport()
{
	m_viewport = DisplayManager::GetSingleton()->GetViewport();
	glm::vec4 view = m_viewport; // we may need to change this in case of 3D
	glViewport((GLsizei)view.x, (GLsizei)view.y, (GLsizei)view.z, (GLsizei)view.w);
}
