package prims

//RGBA Color
type Color struct {
	R float64
	G float64
	B float64
	A float64
}

//Add colors
func ColorAdd(colors ...Color) Color {
	cout := Color{}
	cout.A = 1
	for _, c := range colors {
		cout.R += c.R
		cout.G += c.G
		cout.B += c.B
	}
	return cout
}

//Scale color values by a scalar
func (c *Color) Scale(s float64) {
	c.R *= s
	c.G *= s
	c.B *= s
}