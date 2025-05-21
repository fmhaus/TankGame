#version 130

out vec2 texCoords;
uniform mat4 transform;

void main(void) {
    gl_Position = transform * vec4(gl_Vertex.xy, 0.0, 1.0);
    texCoords = gl_MultiTexCoord0.xy;
}