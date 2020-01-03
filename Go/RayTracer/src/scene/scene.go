package scene

import (
	"log"
	"math"
	"prims"
	"util"
	"sync"
)

//Represents scene containing objects and lights
type Scene struct {
	Lights   	[]prims.Light	//List of lights in the scene
	Spheres     []prims.Sphere	//List of objects in the scene
	Cam			*util.Vertex	//Location of camera in scene
	Depth    	int				//Depth of trace for ray tracing
	IPz      	float64			//Z location of image plane
	IPWidth  	int				//Width of image plane
	IPHeight 	int				//Height of image plane
	NumPixX		int				//Number of image plane pixels in X
	NumPixY		int				//Number of image plane pixels in Y
	Ambient  	*prims.Color	//Ambient lighting color in the scene
}


//Ray trace the scene with the given scene parameters
func (s *Scene) RayTrace(numPixX int, numPixY int) [][]prims.Color {
	if numPixX % s.IPWidth != 0 || numPixY % s.IPHeight != 0 {
		log.Fatal("image resolution must be a multiple of image plane dimensions")
	}

	pixColorData := make([][]prims.Color, numPixX)
	for i := range pixColorData {
		pixColorData[i] = make([]prims.Color, numPixY)
	}
	
	scd := &prims.SafeColorData { 
		Data : pixColorData, 
	}

	traceWithParallelization(numPixX, numPixY, s, scd)
	return scd.Data
}

func traceWithoutParallelization(numPixX, numPixY int, s *Scene, scd *prims.SafeColorData){
	//Ray trace for each image plane pixel
	for i := 0; i < numPixX; i++ {
		for j := 0; j < numPixY; j++ {
			r := computeRayForTrace(s, numPixX, numPixY, i, j)
			c := trace(r, 5, 1, s)
			scd.Set(i, j, c)
		}
	}
}

func traceWithParallelization(numPixX, numPixY int, s *Scene, scd *prims.SafeColorData){
	var wg sync.WaitGroup 
	//Ray trace for each image plane pixel
	for i := 0; i < numPixX; i++ {
		ii := i 

		//Start goroutines to each fill a row of color data
		wg.Add(1)
		go func(){
			defer wg.Done()
			for j := 0; j < numPixY; j++ {
				r := computeRayForTrace(s, numPixX, numPixY, ii, j)
				c := trace(r, 5, 1, s)
				scd.Set(ii, j, c)
			}
		}()
	}
	wg.Wait()
}

//Compute the ray location and direction based on input pixel
func computeRayForTrace(s *Scene, numPixX, numPixY, i, j int) *prims.Ray {
	//Map number of image plane pixels to locations on image plane
	x := ((float64(s.IPWidth) / float64(numPixX)) * float64(i)) - (float64(s.IPWidth) / 2.0)
	y := ((float64(s.IPHeight) / float64(numPixY)) * float64(j)) - (float64(s.IPHeight) / 2.0)

	//Get center of IP pixel
	pixW := float64(s.IPWidth) / float64(numPixX)
	pixH := float64(s.IPHeight) / float64(numPixY)
	xCenter := x + pixW / 2.0
	yCenter := y + pixH / 2.0

	//Compute ray direction vector = IP pixel pos - point of projection (camera pos)
	pPix := util.Vertex{ xCenter, yCenter, s.IPz, 1 }
	u := util.VertSub(&pPix, s.Cam)
	u.Normalize()

	r := &prims.Ray{
		P0: s.Cam,
		U:  u,
	}
	
	return r
}

//Shoot a ray, compute reflections and refractions and total illumination
func trace(r *prims.Ray, depth int, atten float64, scene *Scene) prims.Color {
	finalColor := prims.Color{}

	//Get closest intersection
	minI := findMinIntersection(r, scene)

	if minI.T == math.MaxFloat64 {
		return finalColor
	}

	//If ray inside object, flip surface normal
	if minI.IsInside {
		minI.N.Scale(-1)
	}

	//Decrement depth of trace
	depth--
	if depth >= 0 {
		//Get illumination at intersection point
		localI := prims.Phong(&minI, scene.Lights, scene.Cam, *scene.Ambient)

		//if intersected object isn't reflective or refractive (ray absorbed)
		if minI.Sphere.Props.KR == 0 && minI.Sphere.Props.KT == 0 {
			return localI
		}

		//if reflective surface
		if minI.Sphere.Props.KR > 0 {
			//Shoot reflected ray from intersection in dir of reflection
			reflected := computeReflectionRay(*r.U, &minI)
			I := trace(reflected, depth, atten, scene)

			finalColor = prims.ColorAdd(localI, finalColor, I)
			finalColor.Scale(minI.Sphere.Props.KR * atten)
		}

		//If object is refractive
		if minI.Sphere.Props.KT > 0 {
			transmitted := computeTransmittedRay(*r.U, &minI)
			I := trace(transmitted, depth, atten, scene)

			finalColor = prims.ColorAdd(finalColor, I)
			finalColor.Scale(minI.Sphere.Props.KT * atten)
		}
		return finalColor
	}
	return finalColor
}

//Find the closest point of intersection along the ray
func findMinIntersection(r *prims.Ray, scene *Scene) prims.Intersection {
	//Find closest intersection
	minI := prims.Intersection{}
	minI.T = math.MaxFloat64

	for _, s := range scene.Spheres {
		i := s.Intersect(r)	
		if i.T < minI.T && i.T > math.SmallestNonzeroFloat64 + 0.0000001 {
			minI = i	
		}
	}

	//Flip normal if vector was cast into object
	if minI.IsInside {
		minI.N.Scale(-1)
	}
	
	return minI
}

//Compute the transmitted (refracted) ray direction and point of origin
func computeTransmittedRay(u util.Vector, minI *prims.Intersection) *prims.Ray {
	//Compute cos of angle between ray direction and surface normal = thetaI
	u.Normalize()
	UDotN := util.Dot(&u, minI.N)
	cosThetaI := UDotN / (u.Magnitude() * minI.N.Magnitude())
	cti2 := 1 - math.Pow(cosThetaI, 2)
	//Compute cos of angle between refracted ray and (negative) surface normal = thetaR
	radicand := (1 - cti2)
	cosThetaR := math.Sqrt(radicand)

	v1 := util.MultiplyByScalar(u, minI.Sphere.Props.Nr)
	s1 := cosThetaR - (minI.Sphere.Props.Nr * cosThetaI)
	v2 := util.MultiplyByScalar(*minI.N, s1)

	//Refraction (transmitted) ray
	T := util.VecSub(&v1, &v2)
	T.Normalize()

	//Shift intersection point to in direction of transmitted ray 
	//to avoid immediate collision with object
	shifted := util.MultiplyByScalar(*T, 0.5)
	minI.P = util.VertAdd(minI.P, shifted.ToVertex())
	minI.IsInside = !minI.IsInside;

	transmitted := &prims.Ray {
		P0: minI.P,
		U:  T,
	}

	return transmitted
}

//Compute the reflected ray direction and point of origin
func computeReflectionRay(u util.Vector, minI *prims.Intersection) *prims.Ray {
	//Compute direction of reflection vector
	//R = U - 2N * (U dot N)
	UdotN := util.Dot(&u, minI.N)
	TwoN := util.MultiplyByScalar(*minI.N, 2)
	TwoN.Scale(UdotN)
	R := util.VecSub(&u, &TwoN)
	R.Normalize()

	//Shift intersection point in direction of reflection to avoid "surface acne"
	shifted := util.MultiplyByScalar(*R, 0.5)
	minI.P = util.VertAdd(minI.P, shifted.ToVertex())

	reflected := &prims.Ray {
		P0: minI.P,
		U:	R,
	}

	return reflected
}