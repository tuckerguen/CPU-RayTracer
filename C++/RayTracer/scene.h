#ifndef SCENE_H
#define SCENE_H

#include "triangleMesh.h"
#include "frame_buffer.h"
#include "sphere.h"
#include "light.h"
#include "utilClasses.h"
#include "color.h"

class Scene
{
public:
	void Load(char* file);
	void RayTrace(FrameBuffer* fb);
	Color ShootRay(Vertex P0, Vector u, int depth, float attenuationFactor);
	Sphere* sphereList;
	TriangleMesh* triangleMeshList;
	Light* lightList;
	int sphereCount;
	int triangleMeshCount;
	int lightCount;
	
	int maxDepth;
	float attenuationFactor;
	float imagePlaneZ;
	int imagePlaneWidth;
	int imagePlaneHeight;
	Color Ia;
};

#endif