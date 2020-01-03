#include "utilClasses.h"
#include "color.h"
#include "scene_object.h"
#include "light.h"
#include "intersection.h"
#include "sphere.h"
#include "triangleMesh.h"
#include <math.h>
#include <cfloat>
#include <stdio.h>

using namespace std;

// Transform a point with an arbitrary matrix
Vertex Transform(float* matrix, Vertex& point)
{
	Vertex temp;
	temp.x = matrix[0] * point.x + matrix[4] * point.y + matrix[8] * point.z + matrix[12] * point.h;
	temp.y = matrix[1] * point.x + matrix[5] * point.y + matrix[9] * point.z + matrix[13] * point.h;
	temp.z = matrix[2] * point.x + matrix[6] * point.y + matrix[10] * point.z + matrix[14] * point.h;
	temp.h = matrix[3] * point.x + matrix[7] * point.y + matrix[11] * point.z + matrix[15] * point.h;
	return temp;
}

Vector multiply(Vector v, float s) {
	Vector result = Vector();
	result.i = v.i * s;
	result.j = v.j * s;
	result.k = v.k * s;
	return result;
}

Vector cross(Vector v1, Vector v2) {
	Vector result = Vector();
	result.i = v1.j * v2.k - v1.k * v2.j;
	result.j = -(v1.i * v2.k - v1.k * v2.i);
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

Vector subtract(Vector v1, Vector v2) {
	Vector result = Vector();
	result.i = v1.i - v2.i;
	result.j = v1.j - v2.j;
	result.k = v1.k - v2.k;
	return result;
}

Vector add(Vector v1, Vector v2) {
	Vector result = Vector();
	result.i = v1.i + v2.i;
	result.j = v1.j + v2.j;
	result.k = v1.k + v2.k;
	return result;
}

float dot(Vector v1, Vector v2) {
	return v1.i * v2.i + v1.j * v2.j + v1.k * v2.k;
}

float computeColorIntensity(Vector N, Vector L, Vector H, float cDiff, float cSpec, float Il, float kd, float ks, float ns) {
	float NdotL = dot(N, L);
	float NdotH = dot(N, H);
	float Idiff = kd * NdotL * cDiff;
	float Ispec = ks * pow(NdotH, ns) * cSpec;
	float I = Il * (Idiff + Ispec);
	if (I < 0)
		return 0;
	return I;
}

Color phongIllumination(Vertex v, Vector N, SceneObject obj, Light* lights, int lightCount, Color Ia) {
	Color result = Color();
	for (int i = 0; i < lightCount; i++) {
		Light light = lights[i];

		//light vector
		Vector L = subtract(light.position, v);
		L.normalize();

		//viewing vector
		Vertex camPos = Vertex();
		camPos.x = 0.0; camPos.y = 0.0; camPos.z = 0.0;
		Vector V = multiply(subtract(v, camPos), -1.0);
		V.normalize();

		//halfway vector
		Vector H = multiply(add(L, V), 1 / add(L, V).magnitude());;
		H.normalize();

		const float c = 3;
		float fangatten = c;
		if (light.isPoint == 0) {
			Vector dir = light.position.toVector();
			dir.normalize();
			float LDotDir = dot(L, dir);
			float thetaL = 45.0;
			float al = 1.0;
			if (LDotDir > cos(thetaL))
				fangatten = 0.0;
			else
				fangatten = fabs(pow(LDotDir, al)) * c;
		}

		//Compute Red
		float red = computeColorIntensity(N, L, H, obj.diffuseColor.r, obj.specularColor.r, light.color.r, obj.kd, obj.ks, obj.ns);
		result.r += obj.ka * Ia.r * obj.ambientColor.r + red * fangatten;

		//Compute Green
		float green = computeColorIntensity(N, L, H, obj.diffuseColor.g, obj.specularColor.g, light.color.g, obj.kd, obj.ks, obj.ns);
		result.g += obj.ka * Ia.g * obj.ambientColor.g + green * fangatten;

		//Compute Blue
		float blue = computeColorIntensity(N, L, H, obj.diffuseColor.b, obj.specularColor.b, light.color.b, obj.kd, obj.ks, obj.ns);
		result.b += obj.ka * Ia.b * obj.ambientColor.b + blue * fangatten;
	}
	return result;
}

Intersection intersectSphere(Vertex P0, Vector u, Sphere sphere) {
	Intersection i = Intersection();
	i.distance = FLT_MAX;
	
	Vector deltaP = subtract(sphere.center, P0);
	float UdotDP = dot(u, deltaP);

	float a = pow(UdotDP, 2);
	float b = pow(deltaP.magnitude(), 2);
	float c = pow(sphere.radius, 2);
	float discr = a - b + c;

	if (discr < 0) return i;

	float s1 = UdotDP + sqrt(discr);
	float s2 = UdotDP - sqrt(discr);

	//Solutions are nan or 
	if (s1 != s1 && s2 != s2) 
		return i;

	if (s1 <= 0 && s2 <= 0) return i;
	
	if (s1 > 0 && s2 > 0)
		i.distance = fmin(s1, s2);
	else if (s1 < 0) i.distance = s2;
	else i.distance = s1;

	//Get intersection point
	Vector P0Vec = P0.toVector();
	//P = P0 + u * t
	Vector ivec = add(P0Vec, multiply(u, i.distance));
	Vertex ip = ivec.toVertex();
	Vector normal = subtract(ip, sphere.center);
	normal.normalize();

	i.normal = normal;
	i.point = ip;
	i.obj = sphere;

	return i;
}

float intersectsTriangle(Vertex v0, Vertex v1, Vertex v2, Vertex P0, Vector dir) {
	float t, u, v;
	
	Vector v0v1 = subtract(v1, v0);
	Vector v0v2 = subtract(v2, v0);
	Vector pvec = cross(dir, v0v2);
	float det = dot(v0v1, pvec);

	if (fabs(det) < FLT_EPSILON)
		return 0;

	float invDet = 1.0 / det;
	Vector tvec = subtract(P0, v0);
	u = dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1)
		return 0;

	Vector qvec = cross(tvec, v0v1);
	v = dot(dir, qvec) * invDet;
	if (v < 0 || u + v > 1)
		return 0;

	t = dot(v0v2, qvec) * invDet;

	return t;
}

Vector computeNormal(TriangleMesh tMesh, int i) {
	Face f1 = tMesh.pFaceList[i];
	Vertex v0 = tMesh.pVertexList[f1.v1];
	Vertex v1 = tMesh.pVertexList[f1.v2];
	Vertex v2 = tMesh.pVertexList[f1.v3];
	Vector n1 = cross(subtract(v1, v0), subtract(v2, v0));

	Vector n2 = Vector();
	if (i != 0) {
		Face f2 = tMesh.pFaceList[i - 1];
		v0 = tMesh.pVertexList[f2.v1];
		v1 = tMesh.pVertexList[f2.v2];
		v2 = tMesh.pVertexList[f2.v3];
		n2 = cross(subtract(v1, v0), subtract(v2, v0));
	}

	Vector n4 = Vector();
	if (i != 1) {
		Face f4 = tMesh.pFaceList[i - 2];
		v0 = tMesh.pVertexList[f4.v1];
		v1 = tMesh.pVertexList[f4.v2];
		v2 = tMesh.pVertexList[f4.v3];
		n4 = cross(subtract(v1, v0), subtract(v2, v0));
	}

	Vector n3 = Vector();
	if (i < tMesh.FaceCount - 1) {
		Face f3 = tMesh.pFaceList[i + 1];
		v0 = tMesh.pVertexList[f3.v1];
		v1 = tMesh.pVertexList[f3.v2];
		v2 = tMesh.pVertexList[f3.v3];
		n3 = cross(subtract(v1, v0), subtract(v2, v0));
	}

	Vector n5 = Vector();
	if (i < tMesh.FaceCount - 2) {
		Face f5 = tMesh.pFaceList[i + 2];
		v0 = tMesh.pVertexList[f5.v1];
		v1 = tMesh.pVertexList[f5.v2];
		v2 = tMesh.pVertexList[f5.v3];
		n5 = cross(subtract(v1, v0), subtract(v2, v0));
	}

	Vector n = add(n3, add(n1, n2));
	n = add(n5, add(n, n4));
	n.normalize();
	return n;
}

Intersection intersectTriangleMesh(Vertex P0, Vector u, TriangleMesh tMesh) {
	Intersection intersection = Intersection();
	intersection.distance = FLT_MAX;

	//Iterate over faces of the mesh
	for (int i = 0; i < tMesh.FaceCount; i++) {
		Face face = tMesh.pFaceList[i];
		Vertex v0 = tMesh.pVertexList[face.v1];
		Vertex v1 = tMesh.pVertexList[face.v2];
		Vertex v2 = tMesh.pVertexList[face.v3];

		float t = intersectsTriangle(v0, v1, v2, P0, u);
		if (t > 0) {
			//P = P0 + u * t
			Vector UTimesT = multiply(u, t);
			Vertex P = add(P0.toVector(), UTimesT).toVertex();

			Vector normal = computeNormal(tMesh, i);

			if (t < intersection.distance) {
				intersection.distance = t;
				intersection.normal = normal;
				intersection.obj = tMesh;
				intersection.point = P;
			}
		}

	}
	return intersection;
}

