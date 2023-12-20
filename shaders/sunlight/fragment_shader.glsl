#version 450 core
#extension GL_ARB_texture_barrier : enable

layout (location = 0) out vec4 outColor;

uniform vec2 uInvResolution;
uniform mat4 uMagicMatrix;
uniform vec3 uSunPos;

layout(binding=0) uniform sampler3D uWorldTexture;
layout(binding=1) uniform sampler2D uAlbedoTexture;
layout(binding=2) uniform sampler2D uDepthTexture;
layout(binding=3) uniform sampler2D uNormalTexture;

uint isOccupied(ivec3 pos) {
    vec3 pos0 = pos >> 1;
    ivec3 bitPos = pos & 1;
    vec3 uv = pos0/256.f;
    uint value = uint(textureLod(uWorldTexture, uv, 0).r*255);
    return value & (1U << (bitPos.x + bitPos.z*2 + bitPos.y*4));
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

    const int maxTrace = 200;

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



void main(){
    vec2 coord = gl_FragCoord.xy * invResolution;
    outColor = texture(uAlbedoTexture, coord);
    float depth = texture(uDepthTexture, coord).r;

    if(depth == 1.0f) {
        return;
    }
    vec2 screenCoord = coord * 2 - 1;
    vec3 fv = computeFarVec(screenCoord);
    vec3 camPos = computeNearVec(screenCoord);
    vec3 rayDir = normalize(fv - camPos);

    vec3 norm = texture(uNormalTexture, coord).xyz;



    float d = length(fv - camPos);
    vec3 target = camPos + rayDir*(depth*d-0.09);


    vec3 sunPos = vec3(1000,1000,1000);
    bool hit = raycastToTarget(target, uSunPos);
    
    if(hit) {
        outColor.rgb *= 0.5;
    }
    // outColor = vec4(vec3(floor(target)/10), 1.0f);
}