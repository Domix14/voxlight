#version 450 core
#extension GL_ARB_texture_barrier : enable

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outNormal;

uniform vec3 minBox;
uniform vec3 maxBox;
uniform vec3 uChunkSize;
uniform vec3 sunPos;
uniform vec3 materials[2];
uniform mat4 worldMatrix;

uniform vec3 uCameraPos;
uniform mat4 uModelMatrix;
uniform mat4 uViewProjectionMatrix;
uniform mat4 uViewProjectionInvMatrix;
uniform float uVoxSize;

layout(binding=0) uniform sampler3D worldTexture;
layout(binding=1) uniform sampler3D chunkTexture;
layout(binding=2) uniform sampler2D paletteTexture;
layout(binding=3) uniform sampler2D depthTexture;

in vec3 vWorldPos;
in vec4 vHPos;
in vec3 vLocalCameraPos;
in vec3 vLocalPos;

const vec4 skyColor = vec4(0.529f, 0.8f,  0.92f, 1.f);

vec3 computeFarVec(vec2 texCoord)
{
	vec4 aa = vec4(texCoord*2.0 - vec2(1.0), 1.0f, 1.0f);
	aa = uViewProjectionInvMatrix * aa;
	return aa.xyz / aa.w - uCameraPos;
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

uint isOccupied(ivec3 pos) {
    vec3 pos0 = pos >> 1;
    ivec3 bitPos = pos & 1;
    vec3 uv = pos0/256.f;
    uint value = uint(texture(worldTexture, uv).r*255);
    return value & (1U << (bitPos.x + bitPos.z*2 + bitPos.y*4));
}

void raycastAABB(vec3 ro, vec3 rd, vec3 volMax, vec3 volMin, out float minDist, out float maxDist) {
    vec3 invRd = 1.f / rd;
    float t1 = (volMin.x - ro.x) * invRd.x;
    float t2 = (volMax.x - ro.x) * invRd.x;
    float t3 = (volMin.y - ro.y) * invRd.y;
    float t4 = (volMax.y - ro.y) * invRd.y;
    float t5 = (volMin.z - ro.z) * invRd.z;
    float t6 = (volMax.z - ro.z) * invRd.z;

    minDist = max(max(max(min(t1, t2), min(t3, t4)), min(t5, t6)), 0.f);
    maxDist = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
}

float getVoxel(vec3 p) {
    vec3 uv = (p+0.5)/uChunkSize;
    return textureLod(chunkTexture, uv, 0).r;
}

bool raycastToTarget(vec3 ro, vec3 target) {
    vec3 rd = normalize(target - ro);
    vec3 pos = floor(ro);
    vec3 step = sign(rd);
    vec3 tDelta = step / rd;
    
    vec3 tMax;
    
    vec3 fr = fract(ro);
    
    tMax.x = tDelta.x * ((rd.x>0.0) ? (1.0 - fr.x) : fr.x);
    tMax.y = tDelta.y * ((rd.y>0.0) ? (1.0 - fr.y) : fr.y);
    tMax.z = tDelta.z * ((rd.z>0.0) ? (1.0 - fr.z) : fr.z);

    const int maxTrace = 100;

    for (int i = 0; i < maxTrace; i++) {
        if (isOccupied(ivec3(pos)) != 0U) {
            return true;
        }

        if (tMax.x < tMax.y) {
            if (tMax.z < tMax.x) {
                tMax.z += tDelta.z;
                pos.z += step.z;
                if(pos.z >= 256 || pos.z < 0) {
                    return false;
                }
            } else {
                tMax.x += tDelta.x;
            	pos.x += step.x;
                if(pos.x >= 256|| pos.x < 0) {
                    return false;
                }
            }
        } else {
            if (tMax.z < tMax.y) {
                tMax.z += tDelta.z;
                pos.z += step.z;
                if(pos.z >= 256 || pos.z < 0) {
                    return false;
                }
            } else {
            	tMax.y += tDelta.y;
            	pos.y += step.y;
                if(pos.y >= 256 || pos.y < 0) {
                    return false;
                }
            }
        }
    }

 	return false;
}

float intersect(vec3 ro, vec3 rd, float maxDist, out vec4 color, out vec3 norm) {    
    vec3 step = sign(rd);
    vec3 tDelta = step * uVoxSize / rd;
    
    vec3 tMax;
    
    vec3 fr = fract(ro) * uVoxSize;
    
    tMax.x = tDelta.x * ((rd.x>0.0) ? (1.0 - fr.x) : fr.x);
    tMax.y = tDelta.y * ((rd.y>0.0) ? (1.0 - fr.y) : fr.y);
    tMax.z = tDelta.z * ((rd.z>0.0) ? (1.0 - fr.z) : fr.z);
    tMax *= uVoxSize;
    

    float d = 0;
    vec3 pos = floor(ro);
    while(d < maxDist) {
        //vec3 pos = floor((ro + rd*d)/uVoxSize);
        float hit = getVoxel(pos)*255;
        if(hit != 0) {
            vec2 uv = vec2((hit-0.5)/256.f, 0.5f);
            color = textureLod(paletteTexture, uv, 0.0f);
            return d;
        }

        if (tMax.x < tMax.y) {
            if (tMax.z < tMax.x) {
                d = tMax.z;
                tMax.z += tDelta.z;
                pos.z += step.z;
                norm = vec3(0, 0,-step.z);
            } else {
                d = tMax.x;
                tMax.x += tDelta.x;
            	pos.x += step.x;
                norm = vec3(-step.x, 0, 0);
            }
        } else {
            if (tMax.z < tMax.y) {
                d = tMax.z;
                tMax.z += tDelta.z;
                pos.z += step.z;
                norm = vec3(0, 0, -step.z);
            } else {
                d = tMax.y;
            	tMax.y += tDelta.y;
            	pos.y += step.y;
                norm = vec3(0, -step.y, 0);
            }
        }
    }
    return maxDist;
}

void main(){

    vec3 localPos = vLocalCameraPos;
	vec3 localDir = (vLocalPos - vLocalCameraPos);
	float maxDist2 = length(localDir);
	localDir /= maxDist2;
    float minDist;
    float maxDist;
    raycastAABB(localPos, localDir, minBox, uChunkSize*uVoxSize, minDist, maxDist);

    vec2 tc = (vHPos.xy/vHPos.w)*0.5 + vec2(0.5);
    vec3 fv = computeFarVec(tc);
	float depth = texture(depthTexture, tc).r;
	float currentMinDepth = length(fv*depth);

    if (minDist > currentMinDepth)
		discard;


    vec4 color;
    vec3 norm;
    float d;
    d = intersect(localPos + localDir*(minDist-0.001), localDir, maxDist-minDist, color, norm);

    if(d == (maxDist-minDist)) {
        discard;
    }

    float uNear = 0.1f;
    float uFar = 1000.0;
    vec4 worldPos4 = uModelMatrix*vec4(localPos, 1.0);
    float linearDepth = (uViewProjectionMatrix * worldPos4).w;
    float currentDepth = (1.0f / (linearDepth+0.1f) - 1.0f / uNear) / (1.0f / uFar - 1.0f / uNear);
    gl_FragDepth = currentDepth;

    outColor = color;
    outNormal = norm;
}