Mandelbrot!

-- Vertex.GL30
in vec4 Position;

uniform mat4 ModelViewProjection;

void main()
{
	gl_Position = ModelViewProjection * Position;
}

-- Fragment.GL30
precision highp float;

out vec4 FragColor;

uniform sampler2D ColorTable;

const float maxIterations = 128.0;

void main()
{
	vec2 c = vec2(gl_FragCoord.xy / 240.0) - vec2(2.6, 1.125);
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
