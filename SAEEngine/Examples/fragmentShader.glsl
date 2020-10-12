#version 330 core

in vec3 uvs;
uniform sampler2DArray textureSampler;

out vec4 color;

void main() {
	color = texture(textureSampler, uvs);
};
