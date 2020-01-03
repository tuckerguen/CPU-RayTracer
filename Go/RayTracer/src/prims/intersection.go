package prims

import "util"

//Intersection point indicating information 
//about the ray and point of intersection
type Intersection struct {
	Sphere    *Sphere			//The object of intersection
	P         *util.Vertex		//Point of intersection
	N         *util.Vector		//Normal at point of intersection
	IsInside  bool				//If the incoming ray is from inside object
	T         float64			//From parametric eqn P = P0 + U*T indicating distance along ray to reach this intersection point
}
