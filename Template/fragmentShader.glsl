#version 440 core

out vec4 FragColor;
uniform vec2 uResolution;
uniform float uTimer;
in vec3 fNormal;
in vec4 fEyeCoordinates;
in vec3 fFragpos;

float heightMin = 0.4;
float heightMax = 64.0;


const vec3 SEA_BASE = vec3(0.1, 0.19, 0.22);
const vec3 SEA_WATER_COLOR = vec3(0.8, 0.9, 0.6);

vec3 getSkyColor(vec3 e)
{
	e.y = max(0.0, e.y);
	vec3 skyColor = vec3(pow(1.0 - e.y, 2.0), 1.0 - e.y, 0.6 * (1.0 - e.y) + 0.4);

	return skyColor;
}

vec3 waterColorShader(vec3 upwelling, vec3 sky, vec3 air, float nsnell, float kdiffuse , vec3 I , vec3 N)
{
	float reflectivity;
	vec3 i = normalize(I);
	vec3 n = normalize(N);
	float costhetai = abs(dot(i, n));
	float thetai = acos(costhetai);
	float sinthetat = sin(thetai) / nsnell;
	float thetat = asin(sinthetat);
	if (thetai == 0.0)
	{
		reflectivity = (nsnell - 1) / (nsnell + 1);
		reflectivity = reflectivity * reflectivity;
	}
	else
	{
		float fs = sin(thetat - thetai) / sin(thetat + thetai);
		float ts = tan(thetat - thetai) / tan(thetat + thetai);
		reflectivity = 0.5 * (fs * fs + ts * ts);
	}

	vec3 color = (reflectivity * sky + (1.0 - reflectivity) * upwelling) + air;
	
	return color;
}

void main(void)
{
	vec2 uv = gl_FragCoord.xy / uResolution;
	
	vec3 light = normalize(vec3(5.0, 10.0,-1.0)  - fFragpos);
	vec3 normal = normalize(fNormal);
	vec3 viewVector = normalize(-fEyeCoordinates.xyz);

	vec3 ambientFactor = vec3(0.0);
	vec3 diffuseFactor = vec3(1.0);

	vec3 skyColor = vec3(0.65, 0.80, 0.95);

	float NL = dot(normal, viewVector);
	if (NL < 0.0)
	{
		normal = -normal;
	}

	//Ambient
	vec3 ambient = vec3(0.25) * ambientFactor;

	//Height color
	vec3 shallowColor = vec3(0.0, 0.64, 0.68);
	vec3 deepColor = vec3(0.02, 0.05, 0.10);

	float relativeHeight;
	relativeHeight = (fFragpos.y - heightMin) / (heightMax - heightMin);
	vec3 heightColor = relativeHeight * shallowColor + (1.0 - relativeHeight) * deepColor;

	//Spray
	float sprayThresholdUpper = 1.0;
	float sprayThresholdLower = 0.9;
	float sprayRatio = 0;

	if (relativeHeight > sprayThresholdLower)
	{
		sprayRatio = (relativeHeight - sprayThresholdLower) / (sprayThresholdUpper - sprayThresholdLower);
	}
	vec3 sprayBaseColor = vec3(1.0);
	vec3 sprayColor = sprayRatio * sprayBaseColor;

	//Diffuse
	float diff = max(dot(normal,light), 0.0);
	vec3 diffuse = diffuseFactor  * diff;
	//diffuse = vec3(0.0);

	//pseudo reflect ( fresnel)
	float refCoeff = pow(max(dot(normal, viewVector), 0.0), 0.3);		//smaller power will have more concentrated reflect
	vec3 reflectColor = (1.0 - refCoeff) * skyColor;

	//Specular
	vec3 reflectVector = reflect(-light, normal);
	float specCoeff = pow(max(dot(viewVector, reflectVector), 0.0), 64) * 2;	//Over expose

	vec3 specular = vec3(1.0) * specCoeff;

	vec3 combinedColor = ambient + diffuse + heightColor + reflectColor;

	specCoeff = clamp(specCoeff, 0.0, 1.0);
	combinedColor *= (1.0 - specCoeff);
	combinedColor += specular;
	vec4 color = vec4(combinedColor, 1.0);

	/*
	float NL = max(dot(normal, light), 0.0);
	vec3 diffuse = vec3(1.0) * NL;

	vec3 refracted = SEA_BASE + NL * SEA_WATER_COLOR * 0.12;
	vec3 reflected = getSkyColor(reflect(eye, normal));
	float fresnel = clamp(dot(normal, eye), 0.0, 1.0);
	vec3 iLight = eye - light;

	vec3 watercolor = waterColorShader(vec3(0.0,0.2,0.3),reflected,vec3(0.1,0.1,0.1),1.34,0.91,iLight,normal);
	
	vec3 color = mix(reflected, watercolor, pow(smoothstep(0.0, -0.05, uv.y), 0.3));
	*/

	FragColor = vec4(combinedColor,1.0);
}
