package prims

import "util"

//Face of a polygon
//Pointers to 3 vertices of the Face
type Face struct {
	V1 *util.Vertex
	V2 *util.Vertex
	V3 *util.Vertex
}

//Compute Face N w/ barycentric coords of 3 vertices
func (f *Face) normal(){
	util.VertAdd(f.V1, f.V2, f.V3)
}

