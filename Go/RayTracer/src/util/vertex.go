package util

//Vertex location (X,Y,Z,H) and normal
type Vertex struct {
	X float64
	Y float64
	Z float64
	H float64
}

//Add vertices
func VertAdd(verts ...*Vertex) *Vertex {
	vout := Vertex{}
	vout.H = 1
	for _, v := range verts {
		vout.X += v.X
		vout.Y += v.Y
		vout.Z += v.Z
	}
	return &vout
}

//Get vector from v2 to v1
func VertSub(v1 *Vertex, v2 *Vertex) *Vector {
	return &Vector {
		v1.X - v2.X,
		v1.Y - v2.Y,
		v1.Z - v2.Z,
	}
}

