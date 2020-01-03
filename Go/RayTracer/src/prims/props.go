package prims

//Physical lighting properties of an object
type Props struct {
	Amb  *Color		//Ambient reflection color
	Diff *Color		//Diffuse reflection color
	Spec *Color		//Specular reflection color
	Ka   float64	//Coefficient of ambient reflection (illumination)
	Kd   float64	//Coefficient of diffuse reflection (illumination)
	Ks   float64	//Coefficient of specular reflection (illumination)
	Ns   float64	//Specular exponent
	Nr   float64	//Index of refraction
	KR   float64	//Coefficient of reflection
	KT   float64	//Coefficient of refraction
}
