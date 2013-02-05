#include "Nepgear.h"
#include "helpers.h"

#include <GLXW/glxw.h>
#include <GL/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>
#include <map>

#include "utils/File.h"

#include "renderer/common/GLWindow.h"
#include "renderer/common/Error.h"
#include "renderer/gl30/Shader.h"

static void resize(GLFWwindow *w, int width, int height)
{
	glViewport(0, 0, width, height);
	ShaderProgram *p = (ShaderProgram*)glfwGetWindowUserPointer(w);
	p->SetVector2("Viewport", glm::vec2(width, height));
}

#include "utils/Logger.h"

#define IQM_MAGIC "INTERQUAKEMODEL\0"

typedef unsigned uint;
typedef unsigned char uchar;
enum // vertex array type
{
	IQM_POSITION     = 0,  // float, 3
	IQM_TEXCOORD     = 1,  // float, 2
	IQM_NORMAL       = 2,  // float, 3
	IQM_TANGENT      = 3,  // float, 4
	IQM_BLENDINDICES = 4,  // ubyte, 4
	IQM_BLENDWEIGHTS = 5,  // ubyte, 4
	IQM_COLOR        = 6,  // ubyte, 4
	IQM_CUSTOM       = 0x10
};

enum // vertex array format
{
	IQM_BYTE   = 0,
	IQM_UBYTE  = 1,
	IQM_SHORT  = 2,
	IQM_USHORT = 3,
	IQM_INT    = 4,
	IQM_UINT   = 5,
	IQM_HALF   = 6,
	IQM_FLOAT  = 7,
	IQM_DOUBLE = 8
};

struct iqm_vertexarray
{
	uint type;
	uint flags;
	uint format;
	uint size;
	uint offset;
};

struct iqm_triangle
{
	uint vertex[3];
};

struct iqm_mesh
{
	uint name;
	uint material;
	uint first_vertex, num_vertexes;
	uint first_triangle, num_triangles;
};

int main(int argc, char **argv)
{
	Nepgear ng(argc, argv, "model-loader.log");

	File model;
	model.open("models/skydome.iqm", FileAccessMode_Read);

	struct iqm_header
	{
		char magic[16]; // the string "INTERQUAKEMODEL\0", 0 terminated
		uint version; // must be version 2
		uint filesize;
		uint flags;
		uint num_text, ofs_text;
		uint num_meshes, ofs_meshes;
		uint num_vertexarrays, num_vertices, ofs_vertexarrays;
		uint num_triangles, ofs_triangles, ofs_adjacency;
		uint num_joints, ofs_joints;
		uint num_poses, ofs_poses;
		uint num_anims, ofs_anims;
		uint num_frames, num_framechannels, ofs_frames, ofs_bounds;
		uint num_comment, ofs_comment;
		uint num_extensions, ofs_extensions; // these are stored as a linked list, not as a contiguous array
	} hdr;

	/* make sure the iqm file is one we can actually read */
	model.seek(0);
	model.read(hdr.magic, 16, 1);
	model.seek(0);

	model.read((char*)&hdr, 1, sizeof(hdr));
	model.seek(0);

	if (strncmp(hdr.magic, IQM_MAGIC, 16))
	{
		LOG->Error("Bad magic: %s", hdr.magic);
		model.close();
		return 1;
	}

	if (hdr.version != 2)
	{
		LOG->Debug(
			"This file is IQM version %d. IQM version 2 is needed.",
			hdr.version
		);
		model.close();
		return 1;
	}

	LOG->Debug("\n%s\n"
		"version: %d\n"
		"filesize: %d\n"
		"flags: %d",
		hdr.magic, hdr.version,
		hdr.filesize, hdr.flags
	);

	char *file = new char[hdr.filesize];

	// read the rest of the file into the buffer, do the rest in-memory.
	model.read(file, 1, hdr.filesize);
	model.close();

	/*
	std::vector<std::string> strings;

	for (size_t pos = hdr.ofs_text; strings.size() < hdr.num_text; pos++)
	{
		size_t prev = pos;
		while (file[pos] != '\0')
			pos++;
		LOG->Debug("%s", file+prev);
		strings.push_back(std::string(file+prev));
		pos += pos % 4; // align to 4 bytes, increment.
	}
	*/

	iqm_vertexarray* vertex_arrays = new iqm_vertexarray[hdr.num_vertexarrays];
	memcpy(vertex_arrays, &file[hdr.ofs_vertexarrays], hdr.num_vertexarrays*sizeof(iqm_vertexarray));

	iqm_triangle *tris = new iqm_triangle[hdr.num_triangles];
	memcpy(tris, &file[hdr.ofs_triangles], hdr.num_triangles*sizeof(iqm_triangle));

	iqm_mesh *meshes = new iqm_mesh[hdr.num_meshes];
	memcpy(meshes, &file[hdr.ofs_meshes], hdr.num_meshes*sizeof(iqm_mesh));

	float *inposition = NULL, *innormal = NULL, *intangent = NULL, *intexcoord = NULL;
	uchar *inblendindex = NULL, *inblendweight = NULL;

	for(unsigned i = 0; i < hdr.num_vertexarrays; i++)
	{
		iqm_vertexarray &va = vertex_arrays[i];

		switch(va.type)
		{
		case IQM_POSITION:
			if(va.format != IQM_FLOAT || va.size != 3) assert(0);
			inposition = (float *)&file[va.offset]; break;
		case IQM_NORMAL:
			if(va.format != IQM_FLOAT || va.size != 3) assert(0);
			innormal = (float *)&file[va.offset]; break;
		case IQM_TANGENT:
			if(va.format != IQM_FLOAT || va.size != 4) assert(0);
			intangent = (float *)&file[va.offset]; break;
		case IQM_TEXCOORD:
			if(va.format != IQM_FLOAT || va.size != 2) assert(0);
			intexcoord = (float *)&file[va.offset]; break;
		case IQM_BLENDINDICES:
			if(va.format != IQM_UBYTE || va.size != 4) assert(0);
			inblendindex = (uchar *)&file[va.offset]; break;
		case IQM_BLENDWEIGHTS:
			if(va.format != IQM_UBYTE || va.size != 4) assert(0);
			inblendweight = (uchar *)&file[va.offset]; break;
		}

		LOG->Debug(
			"VA %u: type=%u, flags=%u, format=%u, size=%u, offset=%u",
			i, va.type, va.flags, va.format, va.size, va.offset
		);
	}

	struct vertex
	{
		float position[3];
		float texcoord[2];
		float normal[3];
		float tangent[4];
		uchar blendindex[4];
		uchar blendweight[4];
	};

	vertex *verts = new vertex[hdr.num_vertices];

	for(unsigned i = 0; i < hdr.num_vertices; i++)
	{
		vertex &v = verts[i];
		if(inposition) memcpy(v.position, &inposition[i*3], sizeof(v.position));
		if(intexcoord) memcpy(v.texcoord, &intexcoord[i*2], sizeof(v.texcoord));
		if(innormal) memcpy(v.normal, &innormal[i*3], sizeof(v.normal));
		if(intangent) memcpy(v.tangent, &intangent[i*4], sizeof(v.tangent));
		if(inblendindex) memcpy(v.blendindex, &inblendindex[i*4], sizeof(v.blendindex));
		if(inblendweight) memcpy(v.blendweight, &inblendweight[i*4], sizeof(v.blendweight));
	}

	delete[] file;

	GLWindow wnd;
	WindowParams params;
	params.width = 960;
	params.height = 540;
	if (!wnd.open(params, "Model loading test"))
		return 1;

	GLuint buffers[2], vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, buffers);

	glBindVertexArray(vao);

	LOG->Debug("%ldK", hdr.num_triangles*sizeof(vertex)/1024);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, hdr.num_triangles*sizeof(vertex), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, hdr.num_triangles*sizeof(iqm_triangle), tris, GL_STATIC_DRAW);

	for (unsigned i = 0; i < hdr.num_vertexarrays; ++i)
		glEnableVertexAttribArray(i);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(vertex), (void*)(2*sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(vertex), (void*)(5*sizeof(float)));
	glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(vertex), (void*)(9*sizeof(float)));

	CheckError();

	ShaderProgram p("Skydome.Vertex.GL30", "Skydome.Fragment.GL30");
	{
		p.BindAttrib(0, "vPosition");
		p.BindAttrib(1, "vCoords");
		p.BindAttrib(2, "vNormal");
		p.BindAttrib(3, "vTangent");
		p.Link();
		p.Bind();
		glm::mat4 position = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
		glm::mat4 view = glm::lookAt(glm::vec3(0, -35, 2), glm::vec3(0, 0, 0), glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 projection = glm::perspective(55.0f, 960.f/540.f, 0.1f, 100.0f);
		p.SetMatrix4("View", view);
		p.SetMatrix4("ModelViewProjection", projection * view * position);
		p.SetVector2("Viewport", glm::vec2(960, 540));
	}

	GLFWwindow *w = (GLFWwindow*)wnd.handle;
	glfwSetWindowSizeCallback(w, &resize);
	glfwSetWindowUserPointer(w, &p);

	glEnable(GL_DEPTH_TEST);

	while (!glfwGetWindowParam(w, GLFW_SHOULD_CLOSE) && !glfwGetKey(w, GLFW_KEY_ESCAPE))
	{
		glClearColor(0.25, 0.25, 0.25, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (unsigned i = 0; i < hdr.num_meshes; ++i)
		{
			iqm_mesh &m = meshes[i];
			int start = m.first_triangle;
			int end   = m.first_triangle + m.num_triangles;
			int count = end - start;
			LOG->Debug("%d %d %d", start, end, count);
			glDrawRangeElements(GL_TRIANGLES, start*3, end*3, count*3, GL_UNSIGNED_INT, NULL);
		}

		CheckError();

		glfwSwapBuffers(w);
		glfwWaitEvents();
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, buffers);

	p.Cleanup();

	glfwDestroyWindow(w);

	delete[] verts;
	delete[] tris;
	delete[] meshes;
	delete[] vertex_arrays;

	return 0;
}
