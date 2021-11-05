#include "mesh.h"

void createGrid(float width, float depth, unsigned int m, unsigned int n, MeshData& meshData)
{

	unsigned int vertexCount = m * n;
	unsigned int faceCount = (m - 1)*(n - 1) * 2;

	//create the vertices

	float halfWidth = 0.5f*width;
	float halfDepth = 0.5f*depth;

	//horizontal cell space
	float dx = width / (n - 1);

	//cell space in z 
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	meshData.Vertices.resize(vertexCount);

	for (unsigned int i = 0; i < m; ++i)
	{
		//mapping z values on screen
		float z = halfDepth + i *dz;

		for (unsigned int j = 0; j < n; ++j)
		{
			//map x values from - half x and onwards
			float x = -halfWidth + j*dx;

			meshData.Vertices[i*n + j].position[0] = x;
			meshData.Vertices[i*n + j].position[1] = 0.0f;
			meshData.Vertices[i*n + j].position[2] = z;

			meshData.Vertices[i*n + j].normal[0] = 0.0f;
			meshData.Vertices[i*n + j].normal[1] = 1.0f;
			meshData.Vertices[i*n + j].normal[2] = 0.0f;

			//stretch texture over grid
			meshData.Vertices[i*n + j].texture[0] = j * du;
			meshData.Vertices[i*n + j].texture[1] = j * dv;
		}
	}

	//create the indices

	meshData.Indices.resize(faceCount * 3);

	//Iterate over each quad to compute the indices
	unsigned int k = 0;

	for (unsigned int i = 0; i < m - 1; i++)
	{
		for (unsigned int j = 0; j < n - 1; j++)
		{
			meshData.Indices[k] = i*n + j;
			meshData.Indices[k + 1] = (i + 1)*n + j;
			meshData.Indices[k + 2] = (i)*n + j + 1;

			meshData.Indices[k + 3] = (i)*n + j + 1;
			meshData.Indices[k + 5] = (i + 1)*n + j + 1;
			meshData.Indices[k + 4] = (i + 1)*n + j;

			k += 6; //next quad
		}
	}
}
