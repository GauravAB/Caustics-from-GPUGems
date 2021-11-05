#version 440 core
#define PI 3.14159265358979

in vec4 vPosition;
uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;

uniform float uTimer;

out vec3 fNormal;
out vec4 fEyeCoordinates;
out vec3 fFragpos;


 const float numWaves = 4.0;
 const float Amplitude = 0.5; //amplitude
 const float WaveLength = 50.0; // sample width of 10
 const float WaveFrequency = sqrt(9.8 * (2 * PI / WaveLength) );	//frequency as dipersion ratio
 const vec2 DirectionVector = vec2(-0.6, -0.6);	//direction vector
 const float speed = 5.6;	
 const float phase = (speed *  2 * PI) / WaveLength;
 const float SteepNess = 0.2;

vec3 getWaves(float x, float y, float t)
{
		float Q = SteepNess;
		float A = Amplitude;
		vec2 D = DirectionVector;
		float phaseConstant = phase;
		float SPEED = speed;
		float w = WaveFrequency;
		float L = WaveLength;

		float dX = 0.0;
		float dY = 0.0;
		float dZ = 0.0;

		float a1 = Q * A * D.x;
		float a2 = Q * A * D.y;
		float DdotPos = dot(D, vec2(x, y));
		float cosFnc = cos(w*DdotPos + phaseConstant * t);
		float sinFnc = sin(w*DdotPos + phaseConstant * t);

		dX = a1 * cosFnc;
		dY = a2 * cosFnc;
		dZ = 2.0*A * pow(((sinFnc+1.0)/2.0),2.5);
		
		//second octave
		A /= 2.0;
		w = sqrt(9.8 * (2 * PI / L * 0.5));
		Q = 1.0 / (w * A * 2 * PI * numWaves);
		D = vec2(0.2, -0.6);
		phaseConstant -= 2.0;
		SPEED = 0.2;


		a1 = Q * A * D.x;
		a2 = Q * A * D.y;
		DdotPos = dot(D, vec2(x, y));
		cosFnc = cos(w*DdotPos + phaseConstant * t);
		sinFnc = sin(w*DdotPos + phaseConstant * t);

		dX += a1 * cosFnc;
		dY += a2 * cosFnc;
		dZ += 2.0*A * pow(((sinFnc + 1.0) / 2.0), 2.5);

		//third octave
		A /= 2.2;
		w = sqrt(9.8 * (2 * PI / L * 2.0));
		Q = 1.0 / (w * A * 2 * PI * numWaves);
		D = vec2(0.9, 0.5);
		phaseConstant -= 0.1;

		a1 = Q * A * D.x;
		a2 = Q * A * D.y;
		DdotPos = dot(D, vec2(x, y));
		cosFnc = cos(w*DdotPos + phaseConstant * t);
		sinFnc = sin(w*DdotPos + phaseConstant * t);

		dX += a1 * cosFnc;
		dY += a2 * cosFnc;
		dZ += 2.0*A * pow(((sinFnc + 1.0) / 2.0), 2.5);

		//fourth octave
		A /= 2.0;
		w = sqrt(9.8 * (2 * PI / L*4.0 ));
		Q = 1.0 / (w * A * 2 * PI * numWaves);
		D = vec2(0.2, 0.8);
		phaseConstant += 1.0;
		SPEED *= 0.6;

		a1 = Q * A * D.x;
		a2 = Q * A * D.y;
		DdotPos = dot(D, vec2(x, y));
		cosFnc = cos(w*DdotPos + phaseConstant * t);
		sinFnc = sin(w*DdotPos + phaseConstant * t);

		dX += a1 * cosFnc;
		dY += a2 * cosFnc;
		dZ += 2.0*A * pow(((sinFnc + 1.0) / 2.0), 2.5);

	return vec3(x + dX, y + dY, dZ);
}


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

	float a1 = w* A * D.x;
	float a2 = w* A * D.y;
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

	return vec3(-nX, 1.0 - nZ , -nY);
}

void main(void)
{
	vec4 pos = vPosition;
	fEyeCoordinates = u_view_matrix * u_model_matrix * pos;

	vec3 hMap = getWaves(pos.x , pos.z , uTimer);

	pos.x = hMap.x;
	pos.y = hMap.z;
	pos.z = hMap.y;
	mat3 normalMatrix = mat3(transpose(inverse(u_model_matrix)));
	fFragpos = vec3(u_model_matrix * pos);

	vec3 normal = calcNormal(pos.x, pos.z, uTimer);
	fNormal = normalMatrix * normal;


	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * pos;
}


