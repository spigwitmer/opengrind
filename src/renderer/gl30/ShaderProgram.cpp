#include "Shader.h"
#include "utils/Logger.h"
#include "renderer/common/Error.h"
#include <glm/gtc/type_ptr.hpp>

using namespace std;

ShaderProgram::~ShaderProgram()
{
	if (!m_clean)
		Cleanup();
}

ShaderProgram::ShaderProgram() : m_clean(false) {}

ShaderProgram::ShaderProgram(std::string vss, std::string fss) : m_clean(false)
{
	ShaderStage *vs = new ShaderStage();
	ShaderStage *fs = new ShaderStage();

	vs->Load(SHADER_VERTEX, vss);
	fs->Load(SHADER_FRAGMENT, fss);

	Attach(vs);
	Attach(fs);

	CheckError();
}

void ShaderProgram::Cleanup()
{
	m_clean = true;

	std::vector<ShaderStage*>::iterator it;
	for (it = m_shaders.begin(); it != m_shaders.end(); ++it)
	{
		glDetachShader(m_object, (*it)->GetObject());
		delete *it;
	}

	if (glIsProgram(m_object))
		glDeleteProgram(m_object);
}

void ShaderProgram::SetFloat(std::string name, float f)
{
	glUniform1f(
		glGetUniformLocation(m_object, name.c_str()), f
	);
}

void ShaderProgram::SetInteger(std::string name, int i)
{
	glUniform1i(
		glGetUniformLocation(m_object, name.c_str()), i
	);
}

void ShaderProgram::SetVector2(std::string name, glm::vec2 vec)
{
	glUniform2fv(
		glGetUniformLocation(m_object, name.c_str()),
		1, glm::value_ptr(vec)
	);
}

void ShaderProgram::SetVector3(std::string name, glm::vec3 vec)
{
	glUniform3fv(
		glGetUniformLocation(m_object, name.c_str()),
		1, glm::value_ptr(vec)
	);
}

void ShaderProgram::SetVector4(std::string name, glm::vec4 vec)
{
	glUniform4fv(
		glGetUniformLocation(m_object, name.c_str()),
		1, glm::value_ptr(vec)
	);
}

void ShaderProgram::SetMatrix4(std::string name, glm::mat4 matrix)
{
	glUniformMatrix4fv(
		glGetUniformLocation(m_object, name.c_str()),
		1, false, glm::value_ptr(matrix)
	);
}


void ShaderProgram::Attach(ShaderStage *shader)
{
	if (!glIsProgram(m_object))
		m_object = glCreateProgram();
	
	glAttachShader(m_object, shader->GetObject());
	CheckError();

	m_shaders.push_back(shader);
}

bool ShaderProgram::Link()
{
	glBindFragDataLocation(m_object, 0, "FragColor");
	CheckError();
	
	glLinkProgram(m_object);
	glValidateProgram(m_object);
	
	CheckError();

	int status = 0;
	glGetProgramiv(m_object, GL_LINK_STATUS, &status);

	if (!status)
	{
		int length = 0;
		string error;

		glGetProgramiv(m_object, GL_INFO_LOG_LENGTH, &length);
		glGetProgramInfoLog(m_object, length, NULL, &error[0]);

		CheckError();

		LOG->Warn("%s", error.c_str());

		return false;
	}
	
	return true;
}

void ShaderProgram::BindAttrib(int id, string loc)
{
	glBindAttribLocation(m_object, id, loc.c_str());
}
// Usage
void ShaderProgram::Bind()
{
	glUseProgram(m_object);
}
