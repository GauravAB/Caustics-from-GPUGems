#include <windows.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "glFiles.h"
#include "vmath.h"
#include "setShader.h"
#include "mesh.h"
#include "resource.h"


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glu32.lib")

using namespace vmath;
using namespace std;

//global vars
HWND ghwnd;
HDC ghdc;
HGLRC ghrc;
FILE *gpFile = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;
bool gbFullScreen = false;

enum
{
	GAB_ATTRIBUTE_VERTEX = 0,
	GAB_ATTRIBUTE_COLOR,
	GAB_ATTRIBUTE_NORMAL ,
	GAB_ATTRIBUTE_TEXTURE0,
	GAB_ATTRIBUTE_INDICES
};

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVertexShaderObjectPlane;
GLuint gFragmentShaderObjectPlane;
GLuint gShaderProgramObjectPlane;

GLuint gVAO;
GLuint gVBO;
GLuint gVEO;
GLuint gModelUniform;
GLuint gViewUniform;
GLuint gProjectionUniform;
GLuint gResolution;
GLuint gTimer;

GLuint gModelUniformPlane;
GLuint gViewUniformPlane;
GLuint gProjectionUniformPlane;
GLuint gResolutionPlane;
GLuint gTimerPlane;
GLuint gTextureLightMap;


mat4 gPerspectiveProjectionMatrix;
vec2 resolutionUniform;
float timerUniform;

unsigned int gNumVertices;
unsigned int gNumElements;

float gWidth  = 0.0;
float gHeight = 0.0;




LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	void initialize(void);
	void uninitialize(void);
	void resize(int, int);
	void display(void);

	bool bDone = false;
	HWND hwnd;
	WNDCLASSEX wndclass;
	MSG msg;
	TCHAR szAppName[] = TEXT("Perspective Triangle");

	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("OGL window"), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("Failed to create window"), TEXT("error"), MB_OK);
		return 1;
	}

	ghwnd = hwnd;
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbIsActiveWindow == true)
			{
				if (gbIsEscapeKeyPressed == true)
				{
					bDone = true;
				}
			}
			display();
		}

	}

	uninitialize();
	return(msg.wParam);

}

void initialize(void)
{
	void uninitialize(void);
	int LoadGLTextures(GLuint *texture, TCHAR imageResourceId[]);
	void resize(int, int);
	void initStates(void);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormat;
	int err = fopen_s(&gpFile, "log.txt", "w");
	if (err != 0)
	{
		MessageBox(NULL, TEXT("file open failed"), TEXT("error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "File Created Successfully");
	}


	//set required pfd
	pfd.nVersion = 0;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 64;

	ghdc = GetDC(ghwnd);

	iPixelFormat = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormat == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		MessageBox(NULL, TEXT("iPixelFormat is 0"), TEXT("Error"), MB_OK);
		exit(1);
	}

	if (SetPixelFormat(ghdc, iPixelFormat, &pfd) == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		MessageBox(NULL, TEXT("SetPixelFormat failed"), TEXT("Error"), MB_OK);
		exit(1);
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		wglDeleteContext(ghrc);
		ReleaseDC(ghwnd, ghdc);
		MessageBox(NULL, TEXT("Failed to Create Context"), TEXT("Error"), MB_OK);
		exit(1);
	}

	if ((wglMakeCurrent(ghdc, ghrc)) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		wglDeleteContext(ghrc);
		exit(1);
	}

	GLenum err_no = glewInit();
	if (err_no != GLEW_OK)
	{
		fprintf(gpFile, "Error: %s\n", glewGetErrorString(err));
		fclose(gpFile);
		exit(0);
	}
	const GLubyte *version, *glslVersion, *vendor;

	version = glGetString(GL_VERSION);
	vendor = glGetString(GL_VENDOR);
	glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	fprintf(gpFile, "OpenGL vendor: %s\n", vendor);
	fprintf(gpFile, "OpenGL version: %s\n", version);
	fprintf(gpFile, "GLSLVersion: %s\n", glslVersion);

	
	//Create vertex shader
	gVertexShaderObject = setShader("vertex", "vertexShaderPlane.glsl");

	GLint iCompileStatus;
	GLint iInfoLogLength;
	char *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			GLsizei written;
			glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "Vertex Shader compile error Log : %s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(0);
		}
	}

	//Create fragment shader
	gFragmentShaderObject = setShader("fragment", "fragmentShaderPlane.glsl");

	iCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			GLsizei written;
			glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "Fragment Shader compile error Log : %s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(0);
		}
	}

	gShaderProgramObject = glCreateProgram();

	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//prelinking vertex data layout 
	glBindAttribLocation(gShaderProgramObject, GAB_ATTRIBUTE_VERTEX, "vPosition");
	glLinkProgram(gShaderProgramObject);
	GLint iShaderLinkStatus;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "program link error log : %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gModelUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	gResolution = glGetUniformLocation(gShaderProgramObject, "uResolution");
	gTimer = glGetUniformLocation(gShaderProgramObject, "uTimer");
	
	

	gVertexShaderObjectPlane = setShader("vertex", "vertexShader.glsl");

	 iCompileStatus = 0;
	 iInfoLogLength = 0;
	 szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObjectPlane, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectPlane, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			GLsizei written;
			glGetShaderInfoLog(gVertexShaderObjectPlane, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "Vertex Shader Plane: compile error Log : %s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(0);
		}
	}

	
	gFragmentShaderObjectPlane = setShader("fragment", "fragmentShader.glsl");

	iCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObjectPlane, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectPlane, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			GLsizei written;
			glGetShaderInfoLog(gFragmentShaderObjectPlane, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "Fragment Shader Plane: compile error Log : %s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(0);
		}
	}


	gShaderProgramObjectPlane = glCreateProgram();

	glAttachShader(gShaderProgramObjectPlane, gVertexShaderObjectPlane);
	glAttachShader(gShaderProgramObjectPlane, gFragmentShaderObjectPlane);

	//prelinking vertex data layout 
	glBindAttribLocation(gShaderProgramObjectPlane, GAB_ATTRIBUTE_VERTEX, "vPosition");
	glLinkProgram(gShaderProgramObjectPlane);
	iShaderLinkStatus = 0;
	glGetProgramiv(gShaderProgramObjectPlane, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPlane, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(gShaderProgramObjectPlane, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, " Shader program Plane: link error log : %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gModelUniformPlane = glGetUniformLocation(gShaderProgramObjectPlane, "u_model_matrix");
	gViewUniformPlane = glGetUniformLocation(gShaderProgramObjectPlane, "u_view_matrix");
	gProjectionUniformPlane = glGetUniformLocation(gShaderProgramObjectPlane, "u_projection_matrix");

	gResolutionPlane = glGetUniformLocation(gShaderProgramObjectPlane, "uResolution");
	gTimerPlane = glGetUniformLocation(gShaderProgramObjectPlane, "uTimer");


	MeshData *mesh = new MeshData();

	createGrid(24.0,24.0,gWidth/10.0 ,gHeight/10.0, *mesh);
	
	gNumVertices = mesh->Indices.size() * 3;
	gNumElements = mesh->Indices.size();


	//create vertex buffer array
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);

	//copy the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->Vertices.size(),&mesh->Vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);
	glVertexAttribPointer(GAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*) (sizeof(float) *3));
	glEnableVertexAttribArray(GAB_ATTRIBUTE_NORMAL);
	glVertexAttribPointer(GAB_ATTRIBUTE_TEXTURE0, 2	, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)(sizeof(float)*6));
	glEnableVertexAttribArray(GAB_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glGenBuffers(1, &gVEO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,gVEO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*mesh->Indices.size(), &mesh->Indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mesh->freeMePlease();
	mesh = NULL;

	LoadGLTextures(&gTextureLightMap, MAKEINTRESOURCE(ID_BITMAP1));

	initStates();
	gPerspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);
}


int LoadGLTextures(GLuint *texture, TCHAR imageResourceId[])
{
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = -1;

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitmap)
	{
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		iStatus = 0;
	}

	return iStatus;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int, int);
	void ToggleFullScreen(void);

	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x46:
			if (gbFullScreen == false)
			{
				ToggleFullScreen();
				gbFullScreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbFullScreen = false;
			}
			break;
		case VK_ESCAPE:
			gbIsEscapeKeyPressed = true;
			break;
		default:
			break;
		}
		break;
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
		{
			gbIsActiveWindow = true;
		}
		else
		{
			gbIsActiveWindow = false;
		}
		break;
	case WM_SIZE:
		gWidth = LOWORD(lParam);
		gHeight = HIWORD(lParam);
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}


void display(void)

{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	mat4 modelMatrix = mat4::identity();
	mat4 ViewMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();
	vec3 eye = vec3(0.0, 2.0, 45.0f);
	vec3 center = vec3(0.0, -1.0, 0.0);
	vec3 up = vec3(0.0, 1.0, 0.0);

	
	glUseProgram(gShaderProgramObject);

	timerUniform += 0.02;
	float radius = 5.0f;
	
	ViewMatrix = lookat(eye,center,up);
	modelMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = vmath::rotate(10.0f,1.0f, 0.0f, 0.0f);
	modelMatrix *= rotationMatrix;
	

	glUniformMatrix4fv(gModelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewUniform, 1, GL_FALSE, ViewMatrix);
	glUniformMatrix4fv(gProjectionUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	glUniform2fv(gResolution, 1, resolutionUniform);
	glUniform1f(gTimer, timerUniform);
	glBindVertexArray(gVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,gVEO);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(0);


	glUseProgram(gShaderProgramObjectPlane);

	ViewMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	
	ViewMatrix = lookat(eye, center, up);
	modelMatrix = vmath::translate(0.0f, 5.0f, -4.0f);
	rotationMatrix = vmath::rotate(10.0f, 1.0f, 0.0f, 0.0f);
	modelMatrix *= rotationMatrix;


	glUniformMatrix4fv(gModelUniformPlane, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewUniformPlane, 1, GL_FALSE, ViewMatrix);
	glUniformMatrix4fv(gProjectionUniformPlane, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	glUniform2fv(gResolutionPlane, 1, resolutionUniform);
	glUniform1f(gTimerPlane, timerUniform);
	glBindVertexArray(gVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVEO);
	glBindTexture(GL_TEXTURE_2D, gTextureLightMap);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(0);
	


	SwapBuffers(ghdc);
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	resolutionUniform = vec2((GLfloat)width,(GLfloat)height);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	
	if (width >= height)
	{

		gPerspectiveProjectionMatrix = perspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	}
	else
	{
		gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)height / (GLfloat)width, 0.1f, 100.0f);
	}
}



void ToggleFullScreen(void)
{
	MONITORINFO mi;

	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void uninitialize(void)
{

	if (gbFullScreen)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (gVAO)
	{
		glDeleteVertexArrays(1, &gVAO);
		gVAO = 0;
	}

	if (gVBO)
	{
		glDeleteBuffers(1, &gVBO);
		gVBO = 0;
	}
	
	if (gVEO)
	{
		glDeleteBuffers(1, &gVEO);
		gVEO = 0;
	}



	glDetachShader(gShaderProgramObject, gVertexShaderObject);

	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	glUseProgram(0);



	if (ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}
	wglDeleteContext(ghrc);
	ghrc = NULL;

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "File Closed Successfully");
		fclose(gpFile);
	}
}


















