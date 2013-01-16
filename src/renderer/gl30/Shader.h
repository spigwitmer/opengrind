#pragma once

#include <glm/glm.hpp>
#include <GLXW/glxw.h>
#include <vector>
#include <string>

enum ShaderType {
	SHADER_VERTEX = 0,
	SHADER_FRAGMENT,
	SHADER_GEOMETRY,
	SHADER_INVALID
};

class ShaderBase
{
public:
	GLuint GetObject() const { return m_object; }

protected:
	std::string m_name;
	GLuint m_object;
};

// Used in construction and destruction of Programs.
class ShaderStage : public ShaderBase
{
public:
	ShaderStage();
	virtual ~ShaderStage();
	
	bool Load(ShaderType type, std::string key);
	void LoadString(ShaderType, std::string source, std::string name = "");
	
	ShaderType GetType() const { return m_type; }

protected:
	void LoadInternal(ShaderType type, const char *source, std::string name);
	bool Compile();

	bool m_cached;
	ShaderType m_type;
};

class ShaderProgram : public ShaderBase
{
public:
	virtual ~ShaderProgram();
	void Cleanup();

	ShaderProgram();
	ShaderProgram(std::string vss, std::string fss);


	void SetFloat(std::string name, float f);
	void SetInteger(std::string name, int i);
	void SetVector3(std::string name, glm::vec3 vec);
	void SetMatrix4(std::string name, glm::mat4 matrix);
	
	void Attach(ShaderStage *shader);
	bool Link();
	
	void Bind();
	void BindAttrib(int id, std::string loc);

protected:
	std::vector<ShaderStage*> m_shaders;
};
