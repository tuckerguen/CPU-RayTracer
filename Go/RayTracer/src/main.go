package main

import (
	"image"
	"image/color"
	"image/jpeg"
	"log"
	"os"
	"prims"
	"scene"
	"util"
	"time"
)


func main() {
	res := 2048
	s := setupScene(res)

	start := time.Now()
	data := s.RayTrace(res, res) //make it so you don't have to pass it twice
	elapsed := time.Since(start)
    log.Printf("Ray trace took %s", elapsed)
	writeToImage(data, res, res,"raytraced.jpg")
}

//Setup the objects, lights, and scene properties
func setupScene(res int) scene.Scene {
	light1 := prims.Light {
		Pos:   &util.Vertex{0, 2, -4, 1},
		Color: &prims.Color{1, 1, 1, 1},
	}

	light2 := prims.Light {
		Pos:   &util.Vertex{-5, 2, -10, 1},
		Color: &prims.Color{1, 1, 1, 1},
	}
	
	sphere1 := prims.Sphere {
		Props: &prims.Props{
			Amb:  &prims.Color {1, 0, 0, 0, },
			Diff: &prims.Color {1, 0, 0, 0, },
			Spec: &prims.Color {1, 1, 1, 0, },
			Ka:   0.2,
			Kd:   0.1,
			Ks:   0.8,
			Ns:   100,
			Nr:   1.2,
			KR:   0.0,
			KT:	  1.0,
		},
		Center: &util.Vertex {-0.5, 0, -3, 1, },
		Radius: 0.3,
	}

	sphere2 := prims.Sphere {
		Props: &prims.Props{
			Amb:  &prims.Color {0.3, 1, 1, 0, },
			Diff: &prims.Color {0.3, 1, 1, 0, },
			Spec: &prims.Color {1, 1, 1, 0, },
			Ka:   0.2,
			Kd:   0.4,
			Ks:   0.1,
			Ns:   80,
			Nr:   2.5,
			KR:   0.8,
			KT:   0.0,
		},
		Center: &util.Vertex {0, 0, -9, 1, },
		Radius: 2,
	}

	sphere3 := prims.Sphere {
		Props: &prims.Props{
			Amb:  &prims.Color {1, 0.5, 0.3, 0, },
			Diff: &prims.Color {1, 0.5, 0.3, 0, },
			Spec: &prims.Color {1, 1, 1, 0, },
			Ka:   0.2,
			Kd:   0.1,
			Ks:   0.8,
			Ns:   100,
			Nr:   2.5,
			KR:   0.8,
			KT:   0.0,
		},
		Center: &util.Vertex {-5, 0, -15, 1, },
		Radius: 2,
	}

	s := scene.Scene{
		Lights:	    []prims.Light{ light1, light2 },
		Spheres:    []prims.Sphere{ sphere1, sphere2, sphere3 },
		Cam:		&util.Vertex{0, 0, 0, 1, },
		Depth:      5,
		IPz:	    -8,
		IPWidth:    16,
		IPHeight:   16,
		NumPixX:	res,
		NumPixY: 	res,
		Ambient:    &prims.Color{0.3, 0.3, 0.3, 0, },
	}

	return s
}

//Convert 2D slice of color data to an image file of given resolution
func writeToImage(data [][]prims.Color, resX int, resY int, fname string) {
	w := len(data)
	h := len(data[0])
	if resX % w != 0 || resY % h != 0 {
		log.Fatal("image resolution must be a multiple of width and height")
	}

	img := image.NewRGBA(image.Rect(0, 0, resX, resY))

	// Multiplicative difference between # image plane
	// pixels and # output image pixels
	xScl := resX / w
	yScl := resY / h

	//Scale image plane pixels colors to output image pixels
	for i := 0; i < w; i++{
		for j := 0; j < h; j++ {
			c := data[i][j]
			for x := i * xScl; x < (i + 1) * xScl; x++ {
				for y := j * yScl; y < (j + 1) * yScl; y++ {
					img.Set(x, y, toRGBA(c))
				}
			}
		}
	}

	f, err := os.Create(fname)

	if err != nil {
		panic(err)
	}

	if err := jpeg.Encode(f, img, nil); err != nil {
		panic(err)
	}

	f.Close()
}

//Convert prims.Color to std golang RGBA type
func toRGBA(c prims.Color) color.RGBA {
	if c.R > 1 {
		c.R = 1
	}

	if c.G > 1 {
		c.G = 1
	}
	
	if c.B > 1 {
		c.B = 1
	}

	return color.RGBA{ uint8(c.R * 255), uint8(c.G * 255), uint8(c.B * 255), uint8(c.A * 255)}
}