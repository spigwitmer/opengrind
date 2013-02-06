#include "Shader.h"
#include "renderer/DisplayManager.h"
#include "renderer/common/Error.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"
#include "utils/File.h"

#include <map>
#include <sstream>

using namespace std;

// cache shaders which have already been built.
namespace
{
	// map of opengl-generated shader id's
	map<GLuint, int> g_shaderHandles;
	typedef map<GLuint, int>::iterator shaderHandle_iterator;

	// map of previously loaded shader stages.
	map<string, ShaderStage> g_shaderNames;
	typedef map<string, ShaderStage>::iterator shaderName_iterator;

	string GetPath(string key)
	{
		string path;
		path += "/shaders/";
		path += utils::chop(key, ".");
		path += ".glsl";
		return path;
	}

	string GetToken(string key)
	{
		return utils::chop(key, ".", true);
	}

	string GetShaderSource(string path, string token)
	{
		// Read in file from the VFS, convert to std::string
		string src;
		{
			File f(path);
			if (!f.open())
				LOG->Warn(f.get_last_error());

			src = f.read_string();
			f.close();
		}

		vector<string> lines = utils::split(src, '\n');
		size_t begin = 0, end = lines.size();
		bool found = false;

		for (size_t i = 0; i < lines.size(); ++i)
		{
			if (!(lines[i][0] == '-' && lines[i][1] == '-'))
				continue;

			string line = lines[i].substr(2);
			utils::trim(line);
			if (line == token)
			{
				found = true;
				begin = i+1; // block starts on *next* line
			}
			// only set the end if we've already found the token.
			// this stops end from being set before start.
			else if (found)
			{
				end = i-1;
				break;
			}
		}

		if (!found)
			return string();

		vector<string> directives;
		directives.push_back("#version 130");

		stringstream ss;
		ss << "#line " << begin + directives.size() - 1;
		directives.push_back(ss.str());

		src.clear();
		for (size_t i = 0; i < directives.size(); ++i)
			src += directives[i] + "\n";

		for (size_t i = begin; i < end; ++i)
			src += lines[i] + "\n";

		return src;
	}

	string ShaderToString(ShaderType shader)
	{
		string ret;
		switch(shader)
		{
			case SHADER_VERTEX:
				ret = "vertex";
				break;
			case SHADER_FRAGMENT:
				ret = "fragment";
				break;
			case SHADER_GEOMETRY:
				ret = "geometry";
				break;
			case SHADER_INVALID:
			default:
				assert(0);
				break;
		}
		return ret;
	}

	GLenum ShaderToGLenum(ShaderType shader)
	{
		GLenum ret;
		switch (shader)
		{
			case SHADER_VERTEX:
				ret = GL_VERTEX_SHADER;
				break;
			case SHADER_FRAGMENT:
				ret = GL_FRAGMENT_SHADER;
				break;
			case SHADER_GEOMETRY:
				ret = GL_GEOMETRY_SHADER;
				break;
			case SHADER_INVALID:
			default:
				assert(0);
				break;
		}
		return ret;
	}
}

ShaderStage::ShaderStage() :
	m_cached(false)
{
}

ShaderStage::~ShaderStage()
{
	// If the shader was never loaded.
	if (!glIsShader(m_object)) {
		LOG->Warn("Tried to delete a deleted shader!");
		return;
	}

	if (g_shaderHandles[m_object] <= 0)
	{
		glDeleteShader(m_object);

		shaderHandle_iterator it = g_shaderHandles.find(m_object);

		if (it != g_shaderHandles.end())
			g_shaderHandles.erase(it);
	}
}

bool ShaderStage::Load(ShaderType type, string key)
{
	LOG->Trace("Loading " + ShaderToString(type) + " shader \"" + key + "\"");

	string token = GetToken(key);
	string path  = GetPath(key);
	string src   = GetShaderSource(path, token);

	LoadInternal(type, src.c_str(), key);

	CheckError();

	if (Compile())
	{
		LOG->Debug("Successfully loaded \"" + key + "\"");
		return true;
	}

	LOG->Error("Compile failed for \"%s:%s\"", path.c_str(), token.c_str());

	return false;
}

// Use const char* internally to minimize pointless conversions.
void ShaderStage::LoadInternal(ShaderType type, const char *source, string name)
{
	m_name = name;

	m_object = glCreateShader(ShaderToGLenum(type));
	g_shaderHandles[m_object]++;

	glShaderSource(m_object, 1, &source, NULL);
}

bool ShaderStage::Compile()
{
	// Already have a copy - no need to rebuild.
	if (m_cached)
		return true;

	glCompileShader(m_object);

	int status = 0;
	glGetShaderiv(m_object, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		int length = 0;
		string error;

		glGetShaderiv(m_object, GL_INFO_LOG_LENGTH, &length);
		error.resize(length);
		glGetShaderInfoLog(m_object, length, NULL, &error[0]);

		CheckError();

		LOG->Warn("%s", error.c_str());

		CheckError();

		return false;
	}

	return true;
}
