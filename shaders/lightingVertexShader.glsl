#version 460 core

uniform mat4 uMVPMatrix;
in vec3 vertexPos;

void main() {
	gl_Position = vec4(vertexPos, 1.0);
}