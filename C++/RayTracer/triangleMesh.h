#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "scene_object.h"
#include "utilClasses.h"
#include "scene_object.h"

class TriangleMesh : public SceneObject
{
public:
	TriangleMesh();
	~TriangleMesh();
	void WorldTranslate(float tx, float ty, float tz);
	void WorldRotate(float rx, float ry, float rz);
	void LocalRotate(float rx, float ry, float rz);
	void LocalScale(float s);
	float ModelMatrix[16];
	void Load(char* file, float s, float rx, float ry, float rz, float tx, float ty, float tz);
	Vertex* pVertexList;
	int VertexCount;
	Face* pFaceList;
	int FaceCount;
};

#endif
