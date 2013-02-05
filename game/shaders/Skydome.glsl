-- Vertex.GL30
in vec3 vPosition;
in vec2 vCoords;
in vec3 vNormal;
in vec4 vTangent;

void main()
{
	gl_Position = vec4(vPosition * 0.05, 1.0);
}

-- Fragment.GL30
out vec4 frag_color;

void main()
{
	frag_color = vec4(1.0);
}
