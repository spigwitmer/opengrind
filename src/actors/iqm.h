#pragma once

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

struct iqm_joint
{
    uint name;
    int parent; // parent < 0 means this is a root bone
    float translate[3], rotate[4], scale[3]; 
    // translate is translation <Tx, Ty, Tz>, and rotate is quaternion rotation <Qx, Qy, Qz, Qw>
    // rotation is in relative/parent local space
    // scale is pre-scaling <Sx, Sy, Sz>
    // output = (input*scale)*rotation + translation
};

struct iqm_pose
{
    int parent; // parent < 0 means this is a root bone
    uint channelmask; // mask of which 10 channels are present for this joint pose
    float channeloffset[10], channelscale[10]; 
    // channels 0..2 are translation <Tx, Ty, Tz> and channels 3..6 are quaternion rotation <Qx, Qy, Qz, Qw>
    // rotation is in relative/parent local space
    // channels 7..9 are scale <Sx, Sy, Sz>
    // output = (input*scale)*rotation + translation
};

struct iqm_vertex
{
	float position[3];
	float texcoord[2];
	float normal[3];
	float tangent[4];
	uchar blendindex[4];
	uchar blendweight[4];
};
