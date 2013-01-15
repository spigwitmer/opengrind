-- Vertex.GL30
in vec4 Position;

uniform mat4 ModelViewProjection;

void main()
{
	gl_Position = ModelViewProjection * Position;
}

-- Fragment.GL30
out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0);
}
