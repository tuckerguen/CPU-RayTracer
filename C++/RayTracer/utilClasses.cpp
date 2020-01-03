#include "utils.h"
#include "color.h"
#include "light.h"
#include "scene_object.h"
#include "triangleMesh.h"
#include "sphere.h"
#include <math.h>
#include <cfloat>

Vertex::Vertex()
{
	x = y = z = 0;
}

void Vector::normalize() {
	float m = magnitude();
	i = i / m;
	j = j / m;
	k = k / m;
}

float Vector::magnitude() {
	return sqrt(i * i + j * j + k * k);
}

Vector Vertex::toVector() {
	Vector v = Vector();
	v.i = x;
	v.j = y;
	v.k = z;
	return v;
}

Vertex Vector::toVertex() {
	Vertex v = Vertex();
	v.x = i;
	v.y = j;
	v.z = k;
	return v;
}

