-- Vertex.GL30
in vec4 vPosition;
in vec2 vCoords;

out vec2 Coords;

void main()
{
	Coords = vCoords;
	gl_Position = vPosition;
}

-- Fragment.GL30
uniform sampler2D Texture;

in vec2 Coords;

out vec4 FragColor;

void main()
{
	FragColor = texture(Texture, Coords);
}
