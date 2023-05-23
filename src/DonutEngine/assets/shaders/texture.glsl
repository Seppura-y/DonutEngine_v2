#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoordinate;

uniform mat4 u_viewProjectionMatrix;

out vec2 v_TexCoordinate;
out vec4 v_Color;

void main()
{
	v_Color = a_Color;
	v_TexCoordinate = a_TexCoordinate;
	gl_Position = u_viewProjectionMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoordinate;
in vec4 v_Color;

uniform vec4 u_color;
uniform float u_tilingFactor;
uniform sampler2D u_sampler;

void main()
{
	//color = texture(u_sampler, v_TexCoordinate * u_tilingFactor) * u_color;
	//color = texture(u_sampler, v_TexCoordinate * u_tilingFactor) * v_Color;
	color = v_Color;
}