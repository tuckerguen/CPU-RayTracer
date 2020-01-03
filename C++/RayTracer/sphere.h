#ifndef SPHERE_H
#define SPHERE_H

#include "scene_object.h"
#include "utilClasses.h"

class Sphere : public SceneObject
{
public:
	Vertex center;
	float radius;
};

#endif
