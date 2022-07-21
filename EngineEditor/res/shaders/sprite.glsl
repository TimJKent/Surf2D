// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform vec2 u_Scale;
uniform vec2 u_Offset;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord.x = (a_TexCoord.x - 0.5) * u_Scale.x + (0.5 * u_Scale.x) - u_Offset.x;
	v_TexCoord.y = (a_TexCoord.y - 0.5) * u_Scale.y + (0.5 * u_Scale.y) - u_Offset.y;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture, v_TexCoord) * u_Color;
}