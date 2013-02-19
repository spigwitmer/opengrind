#include "model.h"

#include <GLXW/glxw.h>
#include <cstring>
#include <cassert>

#include "renderer/common/error.h"
#include "utils/logger.h"

namespace Nepgear
{

Model::Model()
{
	num_meshes = 0;
	first_triangle = NULL;
	num_triangles = NULL;
}

Model::~Model()
{
	cleanup();
}

void Model::cleanup()
{
	if (num_meshes)
	{
		delete[] first_triangle;
		delete[] num_triangles;
		num_meshes = 0;
	}
	else return;

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, vbo);
}

bool Model::load_file(File model_file)
{
	iqm_header hdr;
	iqm_vertex *verts = NULL;
	iqm_vertexarray* vertex_arrays = NULL;
	iqm_triangle *tris = NULL;
	iqm_mesh *meshes = NULL;

	/* make sure the iqm file is one we can actually read */
	model_file.seek(0);
	model_file.read(hdr.magic, 16, 1);
	model_file.seek(0);

	model_file.read((char*)&hdr, 1, sizeof(iqm_header));
	model_file.seek(0);

	if (strncmp(hdr.magic, IQM_MAGIC, 16))
	{
		LOG->Error("Bad magic: %s", hdr.magic);
		return 1;
	}

	if (hdr.version != 2)
	{
		LOG->Debug(
			"This file is IQM version %d. IQM version 2 is needed.",
			hdr.version
		);
		return 1;
	}

	/*
	LOG->Debug("\n%s\n"
		"version: %d\n"
		"filesize: %d\n"
		"flags: %d",
		hdr.magic, hdr.version,
		hdr.filesize, hdr.flags
	);
	*/

	char *file = new char[hdr.filesize];

	// read the rest of the file into the buffer, do the rest in-memory.
	model_file.read(file, 1, hdr.filesize);

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

	vertex_arrays = new iqm_vertexarray[hdr.num_vertexarrays];
	memcpy(vertex_arrays, &file[hdr.ofs_vertexarrays], hdr.num_vertexarrays*sizeof(iqm_vertexarray));

	tris = new iqm_triangle[hdr.num_triangles];
	memcpy(tris, &file[hdr.ofs_triangles], hdr.num_triangles*sizeof(iqm_triangle));

	meshes = new iqm_mesh[hdr.num_meshes];
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

		/*
		LOG->Debug(
			"VA %u: type=%u, flags=%u, format=%u, size=%u, offset=%u",
			i, va.type, va.flags, va.format, va.size, va.offset
		);
		*/
	}

	verts = new iqm_vertex[hdr.num_vertices];

	for(unsigned i = 0; i < hdr.num_vertices; i++)
	{
		iqm_vertex &v = verts[i];
		if(inposition) memcpy(v.position, &inposition[i*3], sizeof(v.position));
		if(intexcoord) memcpy(v.texcoord, &intexcoord[i*2], sizeof(v.texcoord));
		if(innormal) memcpy(v.normal, &innormal[i*3], sizeof(v.normal));
		if(intangent) memcpy(v.tangent, &intangent[i*4], sizeof(v.tangent));
		if(inblendindex) memcpy(v.blendindex, &inblendindex[i*4], sizeof(v.blendindex));
		if(inblendweight) memcpy(v.blendweight, &inblendweight[i*4], sizeof(v.blendweight));
	}

	load_buffer(&hdr, verts, tris, meshes);

	delete[] file;
	delete[] verts;
	delete[] tris;
	delete[] meshes;
	delete[] vertex_arrays;

	return false;
}

bool Model::load_buffer(iqm_header *hdr, iqm_vertex *verts, iqm_triangle *tris, iqm_mesh *meshes)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glBindVertexArray(vao);

	LOG->Debug("Uploading %ldKB of mesh data for %ld mesh(es)",
		hdr->num_triangles*sizeof(iqm_vertex)/1024,
		hdr->num_meshes
	);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, hdr->num_vertices*sizeof(iqm_vertex), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, hdr->num_triangles*sizeof(iqm_triangle), tris, GL_STATIC_DRAW);

	for (unsigned i = 0; i < 4; ++i)
		glEnableVertexAttribArray(i);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(iqm_vertex), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(iqm_vertex), (void*)(2*sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(iqm_vertex), (void*)(5*sizeof(float)));
	glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(iqm_vertex), (void*)(9*sizeof(float)));

	CheckError();

	num_meshes		= hdr->num_meshes;
	first_triangle	= new uint[num_meshes];
	num_triangles	= new uint[num_meshes];

	for (uint i = 0; i < hdr->num_meshes; ++i)
	{
		iqm_mesh &m = meshes[i];
		first_triangle[i] = m.first_triangle;
		num_triangles[i] = m.num_triangles;
	}

	return true;
}

void Model::draw()
{
	if (!num_meshes)
	{
		assert(0);
		return;
	}

	for (unsigned i = 0; i < num_meshes; ++i)
	{
		int start = first_triangle[i],
			end   = first_triangle[i] + num_triangles[i],
			count = end - start;

		glBindVertexArray(vao);
		glDrawRangeElements(GL_TRIANGLES, start*3, end*3, count*3, GL_UNSIGNED_INT, NULL);
	}
}

}
