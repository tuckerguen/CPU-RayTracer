package prims

import (
	"util"
)

//A light in the scene
type Light struct {
	Pos   *util.Vertex 	//Position of light
	Color *Color		//Color of the light
}