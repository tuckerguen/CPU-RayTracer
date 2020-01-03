package util

import "math"

//Vector direction with I, J, K components
type Vector struct {
	I float64
	J float64
	K float64
}

//Add vectors
func VecAdd(verts ...*Vector) *Vector {
	vout := Vector{}
	for _, v := range verts {
		vout.I += v.I
		vout.J += v.J
		vout.K += v.K
	}
	return &vout
}

//Dot product of two vectors
func Dot(v1 *Vector, v2 *Vector) float64 {
	return v1.I * v2.I + v1.J * v2.J + v1.K * v2.K
}

//Cross product of two vectors
func Cross(v1 *Vector, v2 *Vector) *Vector {
	return &Vector{
		v1.J * v2.K - v1.K * v2.J,
		-(v1.I * v2.K - v1.K * v2.I),
		v1.I * v2.J - v1.J * v2.I,
	}
}

//Subtract vectors from each other
//v1 - v2 - ... - vn
func VecSub(vecs ...*Vector) *Vector {
	vout := *vecs[0]

	for i := 1; i < len(vecs); i++ {
		v := vecs[i]
		vout.I -= v.I
		vout.J -= v.J
		vout.K -= v.K
	}
	return &vout
}

//Scale this Vector by s
func (v *Vector) Scale(s float64) {
	v.I *= s
	v.J *= s
	v.K *= s
}

//Multiply vector by a scalar and return the resulting vector
func MultiplyByScalar(v Vector, s float64) Vector {
	return Vector{
		v.I * s,
		v.J * s,
		v.K * s,
	}
}

//Magnitude of this Vector
func (v *Vector) Magnitude() float64 {
	return math.Sqrt(Dot(v, v))
}

//Normalize this Vector
func (v *Vector) Normalize() {
	m := v.Magnitude()

	v.I /= m
	v.J /= m
	v.K /= m
}

//Convert vector to vertex
func (v *Vector) ToVertex() *Vertex{
	return &Vertex {
		v.I,
		v.J,
		v.K,
		1,
	}
}
