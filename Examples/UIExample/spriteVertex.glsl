#version 400 core

layout(location = 0) in vec3 raw_pos;
layout(location = 1) in vec4 raw_color;
layout(location = 2) in vec2 raw_uvs;

out vec4 col;
out vec2 uvs;

uniform mat4 ProjectionMatrix;

void main() 
{
	col = raw_color;
	uvs = raw_uvs;
	gl_Position = (ProjectionMatrix) * vec4(raw_pos.xyz, 1.0);
};
