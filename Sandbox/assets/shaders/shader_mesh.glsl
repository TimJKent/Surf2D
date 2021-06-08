//Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec4 a_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 v_color;

void main(){	
	v_color = a_Color;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position,1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;

void main(){
	color = v_color;
}
