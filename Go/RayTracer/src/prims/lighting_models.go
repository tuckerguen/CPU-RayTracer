package prims

import (
	"math"
	"util"
)

//Computes local illumination at an intersection point in the scene based on the Phong Model
func Phong(minI *Intersection, lights []Light, cam *util.Vertex, amb Color) Color {
	result := Color{ 0, 0, 0, 1 }

	V := util.VertSub(minI.P, cam)
	V.Normalize()
	V.Scale(-1)

	for _, l := range lights {
		L := util.VertSub(l.Pos, minI.P)
		L.Normalize()

		H := util.VecAdd(L, V)
		H.Normalize()

		//Fetch diffuse, specular, light, ambient rgb values
		sprops := minI.Sphere.Props

		r := intensity(minI.N, L, H, sprops.Diff.R, sprops.Spec.R, l.Color.R, sprops)
		g := intensity(minI.N, L, H, sprops.Diff.G, sprops.Spec.G, l.Color.G, sprops)
		b := intensity(minI.N, L, H, sprops.Diff.B, sprops.Spec.B, l.Color.B, sprops)

		result.R += minI.Sphere.Props.Ka * sprops.Amb.R * amb.R + r * 3
		result.G += minI.Sphere.Props.Ka * sprops.Amb.G * amb.G + g * 3
		result.B += minI.Sphere.Props.Ka * sprops.Amb.B * amb.B + b * 3
	}
	return result
}

//Computes color intensity at a single point with single light  based on Phong equation
func intensity(N, L, H *util.Vector, cDiff, cSpec, Il float64, props *Props) float64 {
	NdotL := util.Dot(N, L)
	NdotH := util.Dot(N, H)
	Idiff := props.Kd * NdotL * cDiff
	Ispec := props.Ks * math.Pow(NdotH, props.Ns) * cSpec
	I := Il * (Idiff + Ispec)
	if I < 0 {
		return 0
	}
	return I
}
