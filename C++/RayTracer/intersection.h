#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "scene_object.h"
#include "utilClasses.h"

class Intersection
{
public:
	SceneObject obj;
	Vertex point;
	Vector normal;
	bool isInside;
	float distance;
};

#endif