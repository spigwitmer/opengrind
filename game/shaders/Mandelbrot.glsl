Mandelbrot!

-- Vertex.GL30
in vec4 vPosition;
in vec2 vCoords;

out vec2 Coords;

uniform mat4 ModelViewProjection;

void main()
{
	Coords = vCoords;
	gl_Position = ModelViewProjection * vPosition;
}

-- Fragment.GL30
in vec2 Coords;

out vec4 FragColor;

uniform sampler2D ColorTable;
uniform vec2 Viewport;

const float maxIterations = 128.0;

vec2 calc_aspect(vec2 view)
{
	if (view.x > view.y)
		return vec2(view.x / view.y, 1.0);
	return vec2(1.0, view.y / view.x);
}

void main()
{
	//FragColor = vec4(Coords.xy, 0.0, 1.0);
	vec2 aspect = calc_aspect(Viewport);
	vec2 c = (Coords - 0.5) * aspect * 2.5 - vec2(0.65, 0.0);
	vec2 z = c;

	FragColor = texture(ColorTable, vec2(1.0, 0.0));

	for (float i = 0.0; i < maxIterations; i += 1.0)
	{
		z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
		
		if (dot(z, z) > 5.0)
		{
			// Use a var here or mesa gets mad and crashes the program.
			vec4 color = texture(
				ColorTable,
				vec2(max(i, 0.01) / maxIterations, 0.0)
			);
			FragColor = color;
		}
	}
}
