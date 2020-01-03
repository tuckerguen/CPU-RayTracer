package prims

import (
	"math"
	"util"
)

//Represents sphere in scene
type Sphere struct {
	Props  *Props			//Physical properties of sphere
	Center *util.Vertex		//Location of sphere
	Radius float64			//Radius of sphere
}

//Get T from ray r origin to intersection with Sphere s
//If no intersection, return -1
func (s Sphere) Intersect(r *Ray) Intersection {
	var i = Intersection{}

	//Calculate intersection
	dp := util.VertSub(s.Center, r.P0)
	uDotdp := util.Dot(r.U, dp)

	a := math.Pow(uDotdp, 2)
	b := math.Pow(dp.Magnitude(), 2)
	c := math.Pow(s.Radius, 2)

	discr := a - b + c

	if discr < 0 || discr > 300 {
		return i
	}
	s1 := uDotdp + math.Sqrt(discr)
	s2 := uDotdp - math.Sqrt(discr)

	if s2 < 0 {
		s2 = math.MaxFloat64
	}

	t :=  math.Min(s1, s2)

	if t < 0 {
		return i
	}

	//Intersection P, P = P0 + ut
	v := r.U
	v.Scale(t)
	ip := util.VertAdd(r.P0, v.ToVertex())
	i.P = ip

	//Normal
	n := util.VertSub(ip, s.Center)
	n.Normalize()
	i.N = n

	i.Sphere = &s
	i.T = t

	return i
}