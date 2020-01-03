#ifndef UTILMETHODS_H
#define UTILMETHODS_H

#include "utilClasses.h"
#include "color.h"
#include "intersection.h"
#include "scene_object.h"
#include "light.h"
#include "sphere.h"
#include "triangleMesh.h"
#include "intersection.h"

Vector multiply(Vector v, float s);
Vector cross(Vector v1, Vector v2);
Vector subtract(Vertex v1, Vertex v2);
Vector subtract(Vector v1, Vector v2);
Vector add(Vector v1, Vector v2);

float dot(Vector v1, Vector v2);
Color phongIllumination(Vertex v, Vector N, SceneObject obj, Light* lights, int lightCount, Color Ia);
float computeColorIntensity(Vector N, Vector L, Vector H, float cDiff, float cSpec, float Il, float kd, float ks, float ns);
bool hasQuadraticSolution(float a, float b, float c, float& x0, float& x1);

Intersection intersectSphere(Vertex P0, Vector u, Sphere sphere);
float intersectsTriangle(Vertex v0, Vertex v1, Vertex v2, Vertex P0, Vector u);
Vector computeNormal(Vertex v0, Vertex v1, Vertex v2);
Intersection intersectTriangleMesh(Vertex P0, Vector u, TriangleMesh tMesh);

Vertex Transform(float* matrix, Vertex& point);
#endif

