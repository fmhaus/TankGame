#version 330 core

in vec2 in_Position;
out vec2 pass_Position;

uniform vec2 window_size;
uniform vec4 transform;

void main()
{
	vec2 offset = vec2(2.0, -2.0) * (transform.xy + transform.zw * vec2(0.5)) / window_size + vec2(-1.0, 1.0);
	vec2 scale = 2.0f * transform.zw / window_size;
	gl_Position = vec4(in_Position * scale + offset, 0.0, 1.0);
	pass_Position = (in_Position + vec2(0.5)) * transform.zw;
}