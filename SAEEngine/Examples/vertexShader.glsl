#version 330 core

layout(location = 0) in vec3 raw_pos;
layout(location = 1) in vec3 raw_uvs;

out vec3 uvs;

uniform mat4 ProjectionMat;

void main() {
	uvs = raw_uvs;
	gl_Position = ProjectionMat * vec4(raw_pos.xyz, 1.0);
};
