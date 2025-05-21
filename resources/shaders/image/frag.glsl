#version 330 core

in vec2 pass_Tex;

out vec4 out_Color;

uniform sampler2D sampler;
uniform float opacity;

void main() {
	out_Color = texture(sampler, pass_Tex);
	out_Color.a *= opacity;
}