#version 460 core

#define PI 3.14159265358979

out vec4 fragColor;
in vec3 fPos;
uniform sampler2D lightMap;
uniform float uTimer;



const float numWaves = 2.0;
const float Amplitude = 0.4; //amplitude
const float WaveLength = 16.0; // sample width of 10
const float WaveFrequency = sqrt(9.8 * (2 * PI / WaveLength));	//frequency as dipersion ratio
const vec2 DirectionVector = vec2(-0.6, -0.6);	//direction vector
const float speed = 5.6;
const float phase = (speed * 2 * PI) / WaveLength;
const float SteepNess = 0.2;


vec3 calcNormal(float x, float y, float t)
{
	float Q = SteepNess;
	float A = Amplitude;
	vec2 D = DirectionVector;
	float phaseConstant = phase;
	float SPEED = speed;
	float w = WaveFrequency;
	float L = WaveLength;

	float nX = 0.0;
	float nY = 0.0;
	float nZ = 0.0;

	float a1 = w * A * D.x;
	float a2 = w * A * D.y;
	float DdotPos = dot(D, vec2(x, y));
	float cosFnc = cos(w*DdotPos + phaseConstant * t);
	float sinFnc = sin(w*DdotPos + phaseConstant * t);

	//first octave
	nX = a1 * cosFnc;
	nY = a2 * cosFnc;
	nZ = Q * w * A * sinFnc;

	//second octave
	A /= 2.0;
	w = sqrt(9.8 * (2 * PI / L * 0.5));
	Q = 1.0 / (w * A * 2 * PI * numWaves);
	D = vec2(0.2, -0.6);
	phaseConstant -= 2.0;
	SPEED = 0.2;


	a1 = w * A * D.x;
	a2 = w * A * D.y;
	DdotPos = dot(D, vec2(x, y));
	cosFnc = cos(w * DdotPos + phaseConstant * t);
	sinFnc = sin(w * DdotPos + phaseConstant * t);

	nX += a1 * cosFnc;
	nY += a2 * cosFnc;
	nZ += Q * w * A * sinFnc;

	//third octave
	A /= 2.2;
	w = sqrt(9.8 * (2 * PI / L * 2.0));
	Q = 1.0 / (w * A * 2 * PI * numWaves);
	D = vec2(0.9, 0.9);
	phaseConstant -= 0.1;

	a1 = w * A * D.x;
	a2 = w * A * D.y;
	DdotPos = dot(D, vec2(x, y));
	cosFnc = cos(w * DdotPos + phaseConstant * t);
	sinFnc = sin(w * DdotPos + phaseConstant * t);

	nX += a1 * cosFnc;
	nY += a2 * cosFnc;
	nZ += Q * w * A * sinFnc;

	//fourth octave
	A /= 2.0;
	w = sqrt(9.8 * (2 * PI / L * 4.0));
	Q = 1.0 / (w * A * 2 * PI * numWaves);
	D = vec2(0.2, 0.8);
	phaseConstant += 1.0;
	SPEED *= 0.6;

	a1 = w * A * D.x;
	a2 = w * A * D.y;
	DdotPos = dot(D, vec2(x, y));
	cosFnc = cos(w * DdotPos + phaseConstant * t);
	sinFnc = sin(w * DdotPos + phaseConstant * t);

	nX += a1 * cosFnc;
	nY += a2 * cosFnc;
	nZ += Q * w * A * sinFnc;

	return vec3(-nX, 1.0 - nZ, -nY);
}


vec3 line_plane_intercept(vec3 lineP, vec3 lineN, vec3 planeN, float planeD)
{
	float distance = (planeD - lineP.z) / lineN.z;

	return lineP + lineN * distance;
}



void main(void)
{
	vec2 dxdz = calcNormal(fPos.x, fPos.z, uTimer).xz;
	vec3 intercept = line_plane_intercept(fPos.xyz * 0.005 , vec3(dxdz, 1.0), vec3(0.0, 0.0, -1.0), -0.5);

	vec4 color;
	color.rgb = texture(lightMap, intercept.xy).rgb;
	color.rgb += vec3(0.25);
	color.a = 1.0;

	fragColor = vec4(color);
}

