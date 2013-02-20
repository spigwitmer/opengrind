-- Vertex.GL30
in vec4 vPosition;
in vec3 vNormal;

uniform mat4 ModelView;
uniform mat4 Projection;

out vec4 vneye;
out vec4 vpeye;

void main()
{
	vpeye = ModelView * vPosition;
	vneye = normalize(vec4(mat3(ModelView) * vNormal, 0));

	gl_Position = Projection * ModelView * vPosition;
}

-- Fragment.GL30
in vec4 vneye;
in vec4 vpeye;

uniform mat4 ModelView;

out vec4 frag_color;

vec3 light_dirs[3];
vec4 light_cols[3];

void main()
{
	light_dirs[0] = normalize(vec3( -0.5, 0, 0.25));
	light_dirs[1] = normalize(vec3( 0.5,  0.5, 0.5));
	light_dirs[2] = normalize(vec3( 0.0, -0.25, -0.5));

	light_cols[0] = vec4(1.0, 0.9, 0.8, 1.0);
	light_cols[1] = vec4(0.3, 0.4, 0.5, 1.0);
	light_cols[2] = vec4(0.3, 0.1, 0.1, 1.0);

	float power = 7.0;
	vec4 ambient = vec4(0.15);
	vec4 color = vec4(1.0);

	vec4 v_eye = normalize(-vpeye);

	vec4 color_out = ambient;
	for (int i = 0; i < 3; i++)
	{
		vec4 s_eye = vec4(light_dirs[i], 1.0);
		vec4 h_eye = normalize(v_eye + s_eye);

		vec4 Id = light_cols[i] * color * max(dot(s_eye, vneye), 0.0);
		vec4 Is = vec4(pow(max(dot(h_eye, vneye), 0.0), power)) * 0.125;

		color_out += Id + Is;
	}

	color_out.a = dot(color_out.rgb, vec3(0.299, 0.587, 0.114));

	frag_color = color_out;
}

-- Vertex.Outline.GL30
in vec4 vPosition;
in vec3 vNormal;

uniform mat4 ModelView;
uniform mat4 Projection;

void main()
{
	vec4 position = vec4(vPosition.xyz + vNormal * 0.015, 1.0);
	gl_Position = Projection * ModelView * position;
}

-- Fragment.Outline.GL30
out vec4 frag_color;

void main()
{
	vec4 color_out = vec4(0.1, 0.0, 0.0, 1.0);
	color_out.a = dot(color_out.rgb, vec3(0.299, 0.587, 0.114));

	frag_color = color_out;
}

-- old frag stuff
in vec4 vneye;
in vec4 vpeye;

uniform mat4 ModelView;

const vec3 key_dir = normalize(vec3(-0.5, -1.0, 0.5));
const vec3 fil_dir = normalize(vec3( 0.5,  0.5, 0.5));
const vec3 rim_dir = normalize(vec3( 0.0, -0.25, -0.5));

const vec4 key_col = vec4(1.0, 0.9, 0.8, 1.0);
const vec4 fil_col = vec4(0.3, 0.4, 0.5, 1.0);
const vec4 rim_col = vec4(0.3, 0.1, 0.1, 1.0);

out vec4 frag_color;

void main()
{
	vec4 ambient = vec4(0.15);
	vec4 color = ambient;

	color += key_col * dot(vneye, ModelView * vec4(key_dir, 1.0));
	color += fil_col * dot(vneye, ModelView * vec4(fil_dir, 1.0));
	color += rim_col * dot(vneye, ModelView * vec4(rim_dir, 1.0));

	color.a -= ambient.a;
	color.a /= 3.0;

	vec3 r = reflect(-key_dir, vpeye.xyz);
 	float spec = max(pow(dot(r, vneye.xyz), 7.0), 0.0);
 	spec *= 0.5;

 	float power = 10.0;

 	vec4 color_out = color + spec;
	color_out.a = dot(color_out.rgb, vec3(0.299, 0.587, 0.114));

	frag_color = color_out;
}