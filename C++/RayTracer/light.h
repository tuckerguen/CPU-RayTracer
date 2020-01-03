#ifndef LIGHT_H
#define LIGHT_H

#include "utilClasses.h"
#include "color.h"

class Light
{
public:
	int isPoint; //0 if directional, 1 if point
	Vertex position;
	Color color; //RGB color of light
};

#endif