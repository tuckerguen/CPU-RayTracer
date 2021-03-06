// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>
#include <map>
#include "triangleMesh.h"
#include "utilMethods.h"

using namespace std;

TriangleMesh::TriangleMesh() {
	ModelMatrix[0] = ModelMatrix[5] = ModelMatrix[10] = ModelMatrix[15] = 1;
	ModelMatrix[1] = ModelMatrix[2] = ModelMatrix[3] = ModelMatrix[4] =
		ModelMatrix[6] = ModelMatrix[7] = ModelMatrix[8] = ModelMatrix[9] =
		ModelMatrix[11] = ModelMatrix[12] = ModelMatrix[13] = ModelMatrix[14] = 0;
}

TriangleMesh::~TriangleMesh() {
	//delete[] pVertexList;
	//delete[] pFaceList;
}

// Load an object (.obj) file
void TriangleMesh::Load(char* file, float s, float rx, float ry, float rz,
	float tx, float ty, float tz)
{
	FILE* pObjectFile = fopen(file, "r");
	if (!pObjectFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char DataType;
	float a, b, c;

	// Scan the file and count the faces and vertices
	VertexCount = FaceCount = 0;
	while (!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%c %f %f %f\n", &DataType, &a, &b, &c);
		if (DataType == 'v')
			VertexCount++;
		else if (DataType == 'f')
			FaceCount++;
	}
	pVertexList = new Vertex[VertexCount];
	pFaceList = new Face[FaceCount];

	fseek(pObjectFile, 0L, SEEK_SET);

	cout << "Number of vertices: " << VertexCount << endl;
	cout << "Number of faces: " << FaceCount << endl;

	// Load and create the faces and vertices
	int CurrentVertex = 0, CurrentFace = 0;

	while (!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%c %f %f %f\n", &DataType, &a, &b, &c);
		if (DataType == 'v')
		{
			pVertexList[CurrentVertex].x = a;
			pVertexList[CurrentVertex].y = b;
			pVertexList[CurrentVertex].z = c;
			CurrentVertex++;
		}
		else if (DataType == 'f')
		{
			// Convert to a zero-based index for convenience
			pFaceList[CurrentFace].v1 = (int)a - 1;
			pFaceList[CurrentFace].v2 = (int)b - 1;
			pFaceList[CurrentFace].v3 = (int)c - 1;

			CurrentFace++;
		}
	}

	// Apply the initial transformations in order
	LocalScale(s);
	WorldRotate((float)(M_PI * rx / 180.0), (float)(M_PI * ry / 180.0), (float)(M_PI * rz / 180.0));
	WorldTranslate(tx, ty, tz);

	for (int i = 0; i < VertexCount; i++) {
		pVertexList[i].h = 1.0;
		pVertexList[i] = Transform(ModelMatrix, pVertexList[i]);
	}
}

// Do world-based translation
void TriangleMesh::WorldTranslate(float tx, float ty, float tz)
{
	ModelMatrix[12] += tx;
	ModelMatrix[13] += ty;
	ModelMatrix[14] += tz;
}

// Perform world-based rotations in x,y,z order (intended for one-at-a-time use)
void TriangleMesh::WorldRotate(float rx, float ry, float rz)
{
	float temp[16];

	if (rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for (int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[1] = temp[1] * cosx - temp[2] * sinx;
		ModelMatrix[2] = temp[2] * cosx + temp[1] * sinx;
		ModelMatrix[5] = temp[5] * cosx - temp[6] * sinx;
		ModelMatrix[6] = temp[6] * cosx + temp[5] * sinx;
		ModelMatrix[9] = temp[9] * cosx - temp[10] * sinx;
		ModelMatrix[10] = temp[10] * cosx + temp[9] * sinx;
		ModelMatrix[13] = temp[13] * cosx - temp[14] * sinx;
		ModelMatrix[14] = temp[14] * cosx + temp[13] * sinx;
	}

	if (ry != 0)
	{
		float cosy = cos(ry), siny = sin(ry);
		for (int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0] * cosy + temp[2] * siny;
		ModelMatrix[2] = temp[2] * cosy - temp[0] * siny;
		ModelMatrix[4] = temp[4] * cosy + temp[6] * siny;
		ModelMatrix[6] = temp[6] * cosy - temp[4] * siny;
		ModelMatrix[8] = temp[8] * cosy + temp[10] * siny;
		ModelMatrix[10] = temp[10] * cosy - temp[8] * siny;
		ModelMatrix[12] = temp[12] * cosy + temp[14] * siny;
		ModelMatrix[14] = temp[14] * cosy - temp[12] * siny;
	}

	if (rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for (int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0] * cosz - temp[1] * sinz;
		ModelMatrix[1] = temp[1] * cosz + temp[0] * sinz;
		ModelMatrix[4] = temp[4] * cosz - temp[5] * sinz;
		ModelMatrix[5] = temp[5] * cosz + temp[4] * sinz;
		ModelMatrix[8] = temp[8] * cosz - temp[9] * sinz;
		ModelMatrix[9] = temp[9] * cosz + temp[8] * sinz;
		ModelMatrix[12] = temp[12] * cosz - temp[13] * sinz;
		ModelMatrix[13] = temp[13] * cosz + temp[12] * sinz;
	}
}

// Perform locally-based rotations in x,y,z order (intended for one-at-a-time use)
void TriangleMesh::LocalRotate(float rx, float ry, float rz)
{
	float temp[16];

	if (rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for (int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[4] = temp[4] * cosx + temp[8] * sinx;
		ModelMatrix[5] = temp[5] * cosx + temp[9] * sinx;
		ModelMatrix[6] = temp[6] * cosx + temp[10] * sinx;
		ModelMatrix[7] = temp[7] * cosx + temp[11] * sinx;
		ModelMatrix[8] = temp[8] * cosx - temp[4] * sinx;
		ModelMatrix[9] = temp[9] * cosx - temp[5] * sinx;
		ModelMatrix[10] = temp[10] * cosx - temp[6] * sinx;
		ModelMatrix[11] = temp[11] * cosx - temp[7] * sinx;
	}

	if (ry != 0)
	{
		float cosy = cos(ry), siny = sin(ry);
		for (int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0] * cosy - temp[8] * siny;
		ModelMatrix[1] = temp[1] * cosy - temp[9] * siny;
		ModelMatrix[2] = temp[2] * cosy - temp[10] * siny;
		ModelMatrix[3] = temp[3] * cosy - temp[11] * siny;
		ModelMatrix[8] = temp[8] * cosy + temp[0] * siny;
		ModelMatrix[9] = temp[9] * cosy + temp[1] * siny;
		ModelMatrix[10] = temp[10] * cosy + temp[2] * siny;
		ModelMatrix[11] = temp[11] * cosy + temp[3] * siny;
	}

	if (rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for (int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0] * cosz + temp[4] * sinz;
		ModelMatrix[1] = temp[1] * cosz + temp[5] * sinz;
		ModelMatrix[2] = temp[2] * cosz + temp[6] * sinz;
		ModelMatrix[3] = temp[3] * cosz + temp[7] * sinz;
		ModelMatrix[4] = temp[4] * cosz - temp[0] * sinz;
		ModelMatrix[5] = temp[5] * cosz - temp[1] * sinz;
		ModelMatrix[6] = temp[6] * cosz - temp[2] * sinz;
		ModelMatrix[7] = temp[7] * cosz - temp[3] * sinz;
	}
}

// Do locally-based uniform scaling
void TriangleMesh::LocalScale(float s)
{
	for (int i = 0; i <= 11; i++)
		ModelMatrix[i] = s * ModelMatrix[i];
}