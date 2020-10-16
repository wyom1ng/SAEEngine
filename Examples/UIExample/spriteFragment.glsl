#version 400 core

in vec4 col;
in vec2 uvs;

out vec4 color;

uniform sampler2D TextureSampler;

void main() 
{
	color = col * texture(TextureSampler, uvs).rgba;
};
