#include <cstdlib>
#include <iostream>
#include <fstream>
#include "setShader.h"
#include "glFiles.h"

using namespace std;

//function protypes

//Function to read text file
char* readTextFile(char* fileName);


int setShader(char* shaderType, char* shaderFile)
{
	int shaderId;

	char* shaderSourceFile = readTextFile(shaderFile);

	if (shaderType == "vertex")
	{
		shaderId = glCreateShader(GL_VERTEX_SHADER);

	}
	else if (shaderType == "tessControl")
	{
		shaderId = glCreateShader(GL_TESS_CONTROL_SHADER);

	}
	else if (shaderType == "tessEvaluation")
	{
		shaderId = glCreateShader(GL_TESS_EVALUATION_SHADER);

	}
	else if (shaderType == "geometry")
	{
		shaderId = glCreateShader(GL_GEOMETRY_SHADER);
	}
	else if (shaderType == "fragment")
	{
		shaderId = glCreateShader(GL_FRAGMENT_SHADER);
	}

	glShaderSource(shaderId, 1, (const char**)&shaderSourceFile, NULL);
	glCompileShader(shaderId);

	return shaderId;
}



char* readTextFile(char* fileName)
{
	FILE* fp = fopen(fileName, "rb");
	char* content = NULL;
	long numVal = 0;
	
	//go till end
	fseek(fp, 0L, SEEK_END);

	//get count bytes
	numVal = ftell(fp);
	
	fseek(fp, 0L, SEEK_SET); //reset
	content = (char*)malloc( (numVal+1) * sizeof(char));

	fread(content, 1, numVal, fp);	//read into buffer
	content[numVal] = '\0';	//end with null termination

	fclose(fp);
	return content;
}











