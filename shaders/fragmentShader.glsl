#version 450 core
#extension GL_ARB_texture_barrier : enable

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outNormal;

uniform vec3 minBox;
uniform vec3 maxBox;
uniform vec3 chunkSize;
uniform vec3 sunPos;
uniform vec3 materials[2];
uniform mat4 mvp;
uniform mat4 worldMatrix;

layout(binding=0) uniform sampler3D worldTexture;
layout(binding=1) uniform sampler3D chunkTexture;
layout(binding=2) uniform sampler2D paletteTexture;
layout(binding=3) uniform sampler2D depthTexture;

const vec4 skyColor = vec4(0.529f, 0.8f,  0.92f, 1.f);


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
    vec3 uv = (p+0.5)/chunkSize;
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
    vec3 pos = floor(ro);
    
    vec3 step = sign(rd);
    vec3 tDelta = step / rd;
    
    vec3 tMax;
    
    vec3 fr = fract(ro);
    
    tMax.x = tDelta.x * ((rd.x>0.0) ? (1.0 - fr.x) : fr.x);
    tMax.y = tDelta.y * ((rd.y>0.0) ? (1.0 - fr.y) : fr.y);
    tMax.z = tDelta.z * ((rd.z>0.0) ? (1.0 - fr.z) : fr.z);

    float d = 0;
    while(d < maxDist) {
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
    vec2 coord = vec2(gl_FragCoord.x/1280, gl_FragCoord.y/720) * 2 - 1;
    vec4 start = worldMatrix*vec4(coord, -1.f, 1.f);
    
    start /= start.w;
    vec4 end = worldMatrix*vec4(coord, 1.f, 1.f);
    
    end /= end.w;
    vec3 ro = start.xyz;
    vec3 rd = normalize(end.xyz - start.xyz);

    vec4 color;
    vec3 norm;
    float d;
    float minDist;
    float maxDist;

    float uNear = 0.1f;
    float uFar = 1000.0;
    raycastAABB(ro, rd, minBox, maxBox, minDist, maxDist);
    float d1 = minDist / length(end.xyz - start.xyz);
    float d2 = (1.0 / (d1+0.1) - 1.0 / uNear) / (1.0 / uFar - 1.0 / uNear);
    float dd  = texture(depthTexture, vec2(gl_FragCoord.x/1280, gl_FragCoord.y/720)).r;
    
    if(d2 > dd) {
        discard;
    }

    ro -= minBox;
    d = intersect(ro + rd*(minDist-0.001), rd, maxDist-minDist, color, norm);

    if(d == (maxDist-minDist)) {
        discard;
    }

    float linearDepth = (minDist+d) / length(end.xyz - start.xyz);

    float depth = (1.0f / (linearDepth+0.1f) - 1.0f / uNear) / (1.0f / uFar - 1.0f / uNear);
    gl_FragDepth = depth;

    outColor = color; //vec4(vec3(depth), 1.f);
    outNormal = norm;
}

/*
uniform vec3 invS;

vec4 tree_lookup(
uniform float N,
vec3 M) // Lookup coordinates    
{
    vec4 I = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 MND = M;
    for (float i=0; i<HRDWTREE_MAX_DEPTH; i++) { // fixed # of iterations
        vec3 P;      // compute lookup coords. within current node
        P = (MND + floor(0.5 + I.xyz * 255.0)) * invS;      // access indirection pool
        if (I.w < 0.9)                   // already in a leaf?
            I = texture(worldMap,P);// no, continue to next depth
            #ifdef DYN_BRANCHING // early exit if hardware supports dynamic branching
            if (I.w > 0.9)    // a leaf has been reached          
                break;
            #endif
            if (I.w < 0.1) // empty cell
                discard;      // compute pos within next depth grid
            MND = MND * N;
    }
    return (I);
}
*/