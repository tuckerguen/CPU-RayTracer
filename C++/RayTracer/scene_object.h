#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "color.h"

class SceneObject
{
public:
	Color ambientColor; //Ambient RGB
	Color diffuseColor; //Diffuse RGB
	Color specularColor; //Specular RGB
	float ka; //Ambient illumination coeff
	float kd; //Diffuse illumination coeff
	float ks; //Specular illumination coeff
	float ns; //Specular exponent
	float nr; //Index of refraction
	float kReflect; //Reflection coeff
	float kRefract; //Refraction coeff
};

#endif