#version 400 core

layout(location = 0) in vec3 raw_pos;
layout(location = 1) in vec4 raw_color;

out vec4 col;

uniform mat4 ProjectionMatrix;

void main() {
	col = raw_color;
	gl_Position = (ProjectionMatrix) * vec4(raw_pos.xyz, 1.0);
};
