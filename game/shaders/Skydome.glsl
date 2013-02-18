-- Vertex.GL30
in vec4 vPosition;
in vec2 vCoords;
in vec3 vNormal;

uniform mat4 ModelView;
uniform mat4 Projection;

out vec4 vneye;
out vec4 vpeye;
out vec2 coords;

void main()
{
	vpeye = ModelView * vPosition;
	vneye = vec4(mat3(ModelView) * vNormal, 0);
	coords = vCoords;

	gl_Position = Projection * ModelView * vPosition;
}

-- Fragment.GL30
in vec4 vneye;
in vec4 vpeye;
in vec2 coords;

uniform mat4 ModelView;
uniform sampler2D sky_tex;

out vec4 frag_color;

void main()
{
	// xxx
	vec4 color_out = texture(sky_tex, coords / 11.0 + 1.5);
	color_out.a = dot(color_out.rgb, vec3(0.299, 0.587, 0.114));

	frag_color = color_out;
}
