#version 130

in vec2 texCoords;
out vec4 out_Color;

uniform sampler2D sampler;

uniform vec4 fogColor;
uniform vec2 step;

const float weight[5] = float[] (0.227027 / 2.0, 0.1945946, 0.1216216, 0.054054, 0.016216);
const int count = 5;

void main(void) {
	float result = 0;
	for (int x = 0; x < count; x++) {
		float left = 0, right = 0;
		for (int y = 0; y < count; y++) {
			left += (texture(sampler, texCoords + vec2(-x, -y) * step).r + texture(sampler, texCoords + vec2(-x, y) * step)).r * weight[y];
			right += (texture(sampler, texCoords + vec2(x, -y) * step).r + texture(sampler, texCoords + vec2(x, y) * step)).r * weight[y];
		}
		result += (left + right) * weight[x];
	}

	float alpha = 1 - result;
	out_Color = vec4(fogColor.rgb, alpha * fogColor.a);
}