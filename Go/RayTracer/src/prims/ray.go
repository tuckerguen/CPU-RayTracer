package prims

import "util"

//Light ray with a source and direction
type Ray struct {
	P0 *util.Vertex    //Source point
	U  *util.Vector	   //Direction of ray
}
