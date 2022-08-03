// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_LocalPos;

void main()
{
	v_LocalPos = a_Position;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

in vec3 v_LocalPos;

void main()
{
	    float distance = 1.0 - length(v_LocalPos*2.0);
		float circle = smoothstep(0.0, 0.005, distance);
		circle *= smoothstep(0.01 + 0.005, 0.01, distance);

		if (circle == 0.0)
			discard;
	
		color = u_Color;
		color.a *= circle;
}