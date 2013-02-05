-- Vertex.GL30
in vec4 vPosition;
in vec3 vNormal;

uniform mat4 ModelViewProjection;

out vec3 normal;

void main()
{
	normal = vNormal;
	gl_Position = ModelViewProjection * vPosition;
}

-- Fragment.GL30
in vec3 normal;

uniform mat4 View;

const vec3 light = normalize(vec3(0.0, -1.0, -0.5));

out vec4 frag_color;

void main()
{
	frag_color = vec4(vec3(dot(normal, mat3(View) * light)), 1.0);
}
