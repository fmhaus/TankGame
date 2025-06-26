#version 330 core

in vec2 pass_Position;

out vec4 out_Color;

uniform vec4 transform;
uniform vec4 color;
uniform vec4 outline_color;
uniform float outline_width;
uniform float round_corners;

const float soft_edge_corner = 0.02;
const float soft_edge_outline = 0.08;

float dist_sq(vec2 p1, vec2 p2)
{
	return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

float corner_dist(vec2 p, float radius)
{
	// no divergence
	float is_corner = step(
		2.0,
		step(p.x, radius) + step(transform.z - p.x, radius) +
		step(p.y, radius) + step(transform.w - p.y, radius)
	);
	return is_corner * sqrt(min(
		min(dist_sq(p, vec2(radius)), dist_sq(p, vec2(transform.z - radius, radius))),
		min(dist_sq(p, transform.zw - vec2(radius)), dist_sq(p, vec2(radius, transform.w - radius)))
	));
}

void main()
{
	if (outline_width > 0.0) 
	{
		float edge_dist = min(
			min(pass_Position.x, transform.z - pass_Position.x),
			min(pass_Position.y, transform.w - pass_Position.y)
		);

		if (round_corners > 0.0)
		{
			edge_dist = min(edge_dist, round_corners - corner_dist(pass_Position, round_corners));
		}

		vec2 position = pass_Position * transform.zw + transform.xy;
		out_Color = mix(
			outline_color,
			color,
			clamp(edge_dist / (outline_width * soft_edge_outline) - 1.0 / soft_edge_outline + 0.5, 0.0, 1.0)
		);
	} 
	else
	{
		out_Color = color;
	}
	if (round_corners > 0.0)
	{
		out_Color.a *= 1.0 - clamp(
			corner_dist(pass_Position, round_corners) / (round_corners * soft_edge_corner) - 1.0 / soft_edge_corner + 0.5,
			0.0, 1.0
		);
	}
}