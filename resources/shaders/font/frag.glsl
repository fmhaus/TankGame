#version 330

in vec2 pass_Tex;

out vec4 out_Color;

uniform sampler2D sampler;
uniform vec4 color;
uniform vec4 edges;
uniform vec4 outlineColor;

void main() {
    float signed_dist = 1 - texture(sampler, pass_Tex).r;
    out_Color = mix(color, outlineColor, smoothstep(edges.z, edges.w, signed_dist));
    out_Color.a *= 1 - smoothstep(edges.x, edges.y, signed_dist);
}