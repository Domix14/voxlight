#version 460 core

uniform mat4 uModelMatrix;
uniform mat4 uViewProjectionMatrix;
uniform mat4 uViewProjectionInvMatrix;
uniform vec3 uCameraPos;
uniform vec3 uChunkSize;
uniform float uVoxSize;

in vec3 vertexPos;

out vec4 vHPos;
out vec3 vWorldPos;

out vec3 vCamPos;
out vec3 vCamDir;

void main() {
    vec4 worldPos = (uModelMatrix * vec4(vertexPos, 1.0));
	gl_Position = uViewProjectionMatrix * worldPos;
	vWorldPos = worldPos.xyz;
	vHPos = gl_Position;
}