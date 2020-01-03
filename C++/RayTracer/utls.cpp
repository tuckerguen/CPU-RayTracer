#include "utils.h"
#include <math.h>

using namespace std;

Vertex::Vertex()
{
	x = y = z = 0;
}

Vector::Normalize() {
	float magnitude = sqrt()
}

Vector cross(Vector v1, Vector v2) {
	Vector result = Vector();
	result.i = v1.j * v2.k - v1.k * v2.j;
	result.j = v1.k * v2.i - v1.i * v2.k;
	result.k = v1.i * v2.j - v1.j * v2.i;
	return result;
}

Vector subtract(Vertex v1, Vertex v2) {
	Vector result = Vector();
	result.i = v1.x - v2.x;
	result.j = v1.y - v2.y;
	result.k = v1.z - v2.z;
	return result;
}