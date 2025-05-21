#version 330 core

in vec2 in_Pos;
in vec2 in_Tex;

out vec2 pass_Tex;

uniform mat3 transform;

void main() {
	gl_Position = vec4(transform * vec3(in_Pos, 1.0), 1.0);
	pass_Tex = in_Tex;
}