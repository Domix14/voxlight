#version 460 core
#extension GL_ARB_texture_barrier : enable

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outDepth;

uniform vec2 uInvResolution;
uniform vec3 uMinBox;
uniform vec3 uMaxBox;
uniform vec3 uChunkSize;
uniform mat4 uMagicMatrix;

layout(binding=0) uniform sampler3D uChunkTexture;

vec3 computeFarVec(vec2 texCoord)
{
	vec4 aa = vec4(texCoord, 1.0f, 1.0f);
	aa = uMagicMatrix * aa;
	return aa.xyz / aa.w;
}

vec3 computeNearVec(vec2 texCoord)
{
	vec4 aa = vec4(texCoord, -1.0f, 1.0f);
	aa = uMagicMatrix * aa;
	return aa.xyz / aa.w;
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
    return textureLod(uChunkTexture, uv, 0).r;
}

float intersect(vec3 ro, vec3 rd, float maxDist, out vec4 color, out vec3 norm) {    
    vec3 step = sign(rd);
    vec3 tDelta = step / rd;
    
    vec3 tMax;
    
    vec3 fr = fract(ro);
    
    tMax.x = tDelta.x * ((rd.x>0.0) ? (1.0 - fr.x) : fr.x);
    tMax.y = tDelta.y * ((rd.y>0.0) ? (1.0 - fr.y) : fr.y);
    tMax.z = tDelta.z * ((rd.z>0.0) ? (1.0 - fr.z) : fr.z);
    

    float d = 0;
    vec3 pos = floor(ro);
    uint counter = 0;
    while(d < maxDist && counter < 200) {
        counter++;
        
        //vec3 pos = floor((ro + rd*d)/uVoxSize);
        float hit = getVoxel(pos)*255;
        if(hit != 0) {
            vec2 uv = vec2((hit-0.5)/256.f, 0.5f);
            // color = textureLod(uPaletteTexture, uv, 0.0f);
            color = vec4(1, 1, 0, 1);

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
    float minDist;
    float maxDist;

    vec2 coord = gl_FragCoord.xy * uInvResolution;
    vec2 screenCoord = coord * 2 - 1;
    vec3 fv = computeFarVec(screenCoord);
    vec3 camPos = computeNearVec(screenCoord);
    vec3 camDir = fv-camPos;
    float depthLength = length(camDir);
    camDir /= depthLength;

    raycastAABB(camPos, camDir, uMinBox, uMaxBox, minDist, maxDist);

    vec4 color;
    vec3 norm;
    float d;
    vec3 rayStart = camPos - uMinBox;
    d = intersect(rayStart + camDir*(minDist-0.001), camDir, maxDist-minDist, color, norm);

    outColor = vec4(color.rgb, 1);
    outNormal = norm;
}