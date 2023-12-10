#version 330
layout(location=0) out vec4 glFragColor;
#line 1 22221
uniform float uRndFrame;
uniform mat4 uVpMatrix;
uniform mat4 uVpInvMatrix;
uniform vec3 uCameraPos;
uniform float uNear;
uniform float uFar;
uniform float uInvFar;
uniform vec2 uPixelSize;
uniform sampler2D uBlueNoise;

uniform vec4 uFogColor;
uniform vec4 uFogParams; // fogStart, fogDist, fogMax, heightExponent

//Golden angle for random numbers
const float alpha1 = 0.618034005;
const vec2 alpha2 = vec2(0.75487762, 0.56984027);
const vec3 alpha3 = vec3(0.819172502, 0.671043575, 0.549700439);


vec4 applyDistanceFog(vec4 color, vec3 dir, float depth)
{
	float tFog = pow(clamp(1.0-dir.y, 0.0, 1.0), uFogParams.w);
	float t = (depth*uFar - uFogParams.x) / uFogParams.y;
	t = clamp(t, 0.0, 1.0);
	t = t*t*tFog;
	t = clamp(t, 0.0, uFogParams.z);
	return pow(mix(pow(color, vec4(1.0 / 2.2)), pow(uFogColor, vec4(1.0 / 2.2)), t), vec4(2.2));
}


vec3 computeFarVec(vec2 texCoord)
{
	vec4 aa = vec4(texCoord*2.0 - vec2(1.0), 1.0f, 1.0f);
	aa = uVpInvMatrix * aa;
	return aa.xyz / aa.w - uCameraPos;
}


float rnd(vec2 co)
{
	return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}


vec2 rnd2(vec2 co)
{
	float x = fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
	float y = fract(sin(dot(co.yx, vec2(12.9898, 78.233))) * 43758.5453);
	return vec2(x, y);
}


vec3 rnd3(vec2 co)
{
	float x = fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
	float y = fract(sin(dot(co.yx, vec2(12.9898, 78.233))) * 43758.5453);
	float z = fract(sin(dot(vec2(x, y), vec2(12.9898, 78.233))) * 43758.5453);
	return vec3(x, y, z);
}


vec2 blueNoiseTc;

void blueNoiseInit(vec2 texCoord)
{
	blueNoiseTc = (texCoord / uPixelSize) / 512.0;
}

float blueNoise()
{
	float n = texture(uBlueNoise, blueNoiseTc).r;
	float v = fract(n + alpha1 * uRndFrame);
	blueNoiseTc += alpha2;
	return v;
}

vec2 blueNoise2()
{
	vec2 n = texture(uBlueNoise, blueNoiseTc).rg;
	vec2 v = fract(n + alpha2 * uRndFrame);
	blueNoiseTc += alpha2;
	return v;
}

vec3 blueNoise3()
{
	vec3 n = texture(uBlueNoise, blueNoiseTc).rgb;
	vec3 v = fract(n + alpha3 * uRndFrame);
	blueNoiseTc += alpha2;
	return v;
}


#line 1 22222
uniform usampler3D uVolTex;
uniform vec3 uVolResolution;
uniform float uVolTexelSize;
uniform vec3 uVolOffset;
uniform sampler2D uDepth;


bool blockedInScreenspace(vec3 start, vec3 dir)
{
	vec3 mid = start + dir*(uVolTexelSize*0.5*blueNoise());
	float thickness = uVolTexelSize*uInvFar;
	vec4 p = uVpMatrix * vec4(mid, 1.0);
	vec2 tc = (p.xy / p.w)*0.5 + vec2(0.5);
	float pDepth = p.w*uInvFar*0.99;
	float depth = texture(uDepth, tc).r;
	if (pDepth > depth && pDepth < depth + thickness)
		return true;
	return false;
}


float raycastShadowVolume(vec3 origin, vec3 dir, float dist)
{
	origin -= uVolOffset;
	vec3 invDir = vec3(1.0) / (abs(dir) + vec3(0.00001));

	vec3 invRes = vec3(1.0) / uVolResolution;
	vec3 halfInvRes = invRes*0.5;

	vec3 tSign = sign(dir);
	vec3 zSign = step(vec3(0.0), tSign);

	int mip = -1;

	float t = 0.0;
	while (t < dist)
	{
		if (mip == -1)
		{
			float texelSize = uVolTexelSize * 0.5;
			vec3 invResolution = invRes * 0.5;
			vec3 tDelta = invDir * texelSize;
			vec3 tPos = (origin + t*dir) / texelSize;
			vec3 ti = floor(tPos);
			vec3 tMax = (invDir * (zSign + tSign*(ti - tPos))) * texelSize + vec3(t);
			ti = (ti + vec3(0.5)) * invResolution;
			vec3 tStep = tSign * invResolution;
			int c = 0;
			while (t < dist && c++ < 8)
			{
				uint a = textureLod(uVolTex, ti, 0.0).x;
				if (a != 0u)
				{
					uint bit = 0u;
					bit += mod(ti.x, invRes.x) > halfInvRes.x ? 1u : 0u;
					bit += mod(ti.y, invRes.y) > halfInvRes.y ? 2u : 0u;
					bit += mod(ti.z, invRes.z) > halfInvRes.z ? 4u : 0u;
					uint mask = 1u << bit;
					if ((mask & a) != 0u)
					{
						return t;
					}
				}
				vec3 cmp = step(tMax.xyz, tMax.zxy) * step(tMax.xyz, tMax.yzx);
				t = dot(tMax, cmp);
				tMax += tDelta * cmp;
				ti += tStep * cmp;
			}
			mip = 0;
		}
		else
		{
			float mipScale = float(1 << mip);
			float texelSize = uVolTexelSize * mipScale;
			vec3 invResolution = invRes * mipScale;
			vec3 tDelta = invDir * texelSize;
			vec3 tPos = (origin + t*dir) / texelSize;
			vec3 ti = floor(tPos);
			vec3 tMax = (invDir * (zSign + tSign*(ti - tPos))) * texelSize + vec3(t);
			ti = (ti + vec3(0.5)) * invResolution;
			vec3 tStep = tSign * invResolution;

			int c = (mip < 2 ? 8 : 1024);
			while (t < dist)
			{
				if (c-- == 0)
				{
					mip++;
					break;
				}
				uint a = textureLod(uVolTex, ti, mip).x;
				if (a != 0u)
				{
					mip--;
					break;
				}
				vec3 cmp = step(tMax.xyz, tMax.zxy) * step(tMax.xyz, tMax.yzx);
				t = dot(tMax, cmp);
				tMax += tDelta * cmp;
				ti += tStep * cmp;
			}
		}
	}
	return dist;
}


float raycastShadowVolumeSparse(vec3 origin, vec3 dir, float dist)
{
	origin -= uVolOffset;
	float step = uVolTexelSize;
	vec3 invRes = vec3(1.0) / uVolResolution;
	vec3 halfInvRes = invRes*0.5;
	vec3 stepDir = dir * (step / uVolTexelSize) * invRes;
	vec3 pos = (origin / uVolTexelSize) * invRes;

	stepDir *= 0.5;
	step *= 0.5;
	int lod = -1;

	float d = 0.0;
	while (d < dist)
	{
		uint c = textureLod(uVolTex, pos, lod).x;
		if (lod == -1)
		{
			if (c != 0u)
			{
				uint bit = 0u;
				bit += mod(pos.x, invRes.x) > halfInvRes.x ? 1u : 0u;
				bit += mod(pos.y, invRes.y) > halfInvRes.y ? 2u : 0u;
				bit += mod(pos.z, invRes.z) > halfInvRes.z ? 4u : 0u;
				uint mask = 1u << bit;
				if ((mask & c) != 0u)
					return d;
				else
				{
					pos += stepDir;
					d += step;
				}
			}
			else
			{
				lod++;
				stepDir *= 2.0;
				step *= 2.0;
				pos += stepDir;
				d += step;
			}
		}
		else
		{
			if (c != 0u)
			{
				stepDir *= 0.5;
				step *= 0.5;
				lod--;
				pos -= stepDir;
				d -= step;
			}
			else
			{
				if (lod < 2)
				{
					c = textureLod(uVolTex, pos, lod + 1).x;
					if (c == 0u)
					{
						lod++;
						stepDir *= 2.0;
						step *= 2.0;
					}
				}
				pos += stepDir;
				d += step;
			}
		}
	}
	return dist;
}


float raycastShadowVolumeSuperSparse(vec3 origin, vec3 dir, float dist)
{
	origin -= uVolOffset;
	float step = uVolTexelSize;
	vec3 invRes = vec3(1.0) / uVolResolution;
	vec3 halfInvRes = invRes*0.5;
	vec3 stepDir = dir * (step / uVolTexelSize) * invRes;
	vec3 pos = (origin / uVolTexelSize) * invRes;
	float baseDistance = 0.5;
	float d = 0.0;

	//Start in half res and bitmask
	stepDir *= 0.5;
	step *= 0.5;
	while (d < baseDistance)
	{
		uint c = textureLod(uVolTex, pos, 0).x;
		uint bit = 0u;
		bit += mod(pos.x, invRes.x) > halfInvRes.x ? 1u : 0u;
		bit += mod(pos.y, invRes.y) > halfInvRes.y ? 2u : 0u;
		bit += mod(pos.z, invRes.z) > halfInvRes.z ? 4u : 0u;
		uint mask = 1u << bit;
		if ((mask & c) != 0u)
			return d;

		pos += stepDir;
		d += step;
	}

	//Move up to base level
	stepDir *= 2.0;
	step *= 2.0;
	while (d < baseDistance*2.0)
	{
		if (textureLod(uVolTex, pos, 0).x != 0u)
			return d;
		pos += stepDir;
		d += step;
	}

	//First mip
	stepDir *= 2.0;
	step *= 2.0;
	while (d < baseDistance*4.0)
	{
		if (textureLod(uVolTex, pos, 1).x != 0u)
			return d;
		pos += stepDir;
		d += step;
	}

	//Second mip
	stepDir *= 2.0;
	step *= 2.0;
	while (d < dist)
	{
		if (textureLod(uVolTex, pos, 2).x != 0u)
			return d;
		pos += stepDir;
		d += step;
	}

	return dist;
}

#line 1 22223
uniform sampler2D uNormal;

uniform vec3 uLightColor;
uniform vec3 uLightDir;
uniform float uLightSpread;
uniform float uLightLength;

in vec2 vTexCoord;
in vec3 vFarVec;

float getDepth(vec2 texCoord)
{
	return texture(uDepth, texCoord).r;
}

vec3 getPixelPos(vec2 texCoord)
{
	return uCameraPos + vFarVec*getDepth(texCoord);
}

vec3 getPixelNormal(vec2 texCoord)
{
	return texture(uNormal, texCoord).rgb;
}


#if 0
attribute vec2 aPosition;
attribute vec2 aTexCoord;

void main(void)
{
	gl_Position = vec4(aPosition, 0.0, 1.0);
	vTexCoord = aTexCoord;
	vFarVec = computeFarVec(aTexCoord);
}
#endif


#if 1

float raycastDirectional(vec3 pos, vec3 normal, vec3 dir, float dist)
{
	if (blockedInScreenspace(pos, dir))
		return 0.0;

	vec3 jitter = (blueNoise3() - vec3(0.5));
	jitter -= normal*dot(normal, jitter);
	jitter = normalize(jitter) * uVolTexelSize * 0.5 * blueNoise();
	pos += jitter;

	pos += dir * uVolTexelSize*0.5;
	//Continue along normal until one voxel out
	pos += normal * uVolTexelSize * (0.6 * (1.0 - max(0.0, dot(dir, normal))));

	return raycastShadowVolumeSparse(pos, dir, dist);
}

void main(void)
{
	blueNoiseInit(vTexCoord);

	vec3 pos = uCameraPos + vFarVec*getDepth(vTexCoord);
	vec3 normal = getPixelNormal(vTexCoord);

	if (dot(normal, normal) == 0.0)
	{
		glFragColor = vec4(0.0);
		return;
	}
	
	glFragColor = vec4(0.0, 0.0, 0.0, 0.0);
	{
		vec3 col = uLightColor;
		vec3 rnd = blueNoise3()*2.0 - vec3(1.0);
		vec3 dir = normalize(uLightDir+rnd*uLightSpread);

		float dist = uLightLength;
		float incoming = dot(dir, normal);
		if (incoming > 0.0)
		{
			float hitDist = raycastDirectional(pos, normal, dir, dist);
			float t = smoothstep(0.8, 1.0, hitDist/dist);	//Fade out shadow towards the end
			glFragColor.rgb += uLightColor * (t*incoming);
			glFragColor.a = 1.0;
		}
	}
}

#endif

