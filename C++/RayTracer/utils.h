#ifndef UTILS_H
#define UTILS_H

class Vector;

class Vertex
{
public:
	Vertex();
	Vector toVector();
	float x, y, z;
};

class Vector
{
public:
	Vertex toVertex();
	void normalize();
	float magnitude();
	float i, j, k;
};

class Face
{
public:
	int v1, v2, v3;
};

#endif