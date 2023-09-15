#version 460 core

uniform mat4 uModelMatrix;
uniform mat4 uViewProjectionMatrix;
uniform vec3 uCameraPos;
uniform vec3 uChunkSize;
uniform float uVoxSize;

in vec3 vertexPos;

out vec4 vHPos;
out vec3 vWorldPos;
out vec3 vLocalCameraPos;
out vec3 vLocalPos;

void main() {
    vec4 worldPos = (uModelMatrix * vec4(vertexPos*uChunkSize*uVoxSize, 1.0));
	gl_Position = uViewProjectionMatrix * worldPos;
	vWorldPos = worldPos.xyz;
	vHPos = gl_Position;
	mat3 volMatrixInv = transpose(mat3(uModelMatrix));
	vLocalCameraPos = volMatrixInv * (uCameraPos - uModelMatrix[3].xyz);
	vLocalPos = volMatrixInv * (vWorldPos - uModelMatrix[3].xyz);
}