#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoordinate;
layout(location = 3) in float a_TextureId;
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_viewProjectionMatrix;

out vec2 v_TexCoordinate;
out vec4 v_Color;
out float v_TextureId;
out float v_TilingFactor;

void main()
{
	v_Color = a_Color;
	v_TexCoordinate = a_TexCoordinate;
	v_TextureId = a_TextureId;
	v_TilingFactor = a_TilingFactor;
	gl_Position = u_viewProjectionMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoordinate;
in vec4 v_Color;
in float v_TilingFactor;
in float v_TextureId;

uniform sampler2D u_samplers[32];

void main()
{
	color = texture(u_samplers[int(v_TextureId)], v_TexCoordinate * v_TilingFactor) * v_Color;
}