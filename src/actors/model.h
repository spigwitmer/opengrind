#pragma once
#include "iqm.h"
#include "utils/file.h"

namespace Nepgear
{

class Model
{
public:
	Model();
	virtual ~Model();

	void cleanup();

	bool load_file(File);
	bool load_buffer(iqm_header*, iqm_vertex*, iqm_triangle*, iqm_mesh*);

	void draw();

protected:
	uint vao, vbo[2];
	uint num_meshes;
	uint *first_triangle, *num_triangles;
};

}
