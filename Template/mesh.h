#pragma once
#include <iostream>
#include <vector>
#include "vmath.h"

using std::vector;
using vmath::vec3;
using vmath::vec2;


struct Vertex
{
	Vertex() {}
	Vertex(const vec3 &p, const vec3 &n, const  vec3 &t, const vec2 &uv) : position(p), normal(n), texture(uv) {}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float  u, float v)
		:position(px, py, pz), normal(nx, ny, nz), texture(u, v) {}

	vec3 position;
	vec3 normal;
	vec2 texture;
	vec3 material[3];
};


struct MeshData
{
	vector<Vertex> Vertices;
	vector<int> Indices;
	void freeMePlease(void) { free(this); }
};

void createGrid(float width, float depth, unsigned int m, unsigned int n, MeshData& meshData);

