#ifndef UTILS_H
#define UTILS_H

class Vertex;

class Vector
{
public:
	Vertex toVertex();
	void normalize();
	float magnitude();
	float i, j, k;
};

class Vertex
{
public:
	Vertex();
	Vector toVector();
	float x, y, z, h;
};

class Face
{
public:
	int v1, v2, v3;
};

#endif